

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define BOARDSIZE 5
#define THREADS 4

char referenceboard[BOARDSIZE * BOARDSIZE];

int solutions = 0, attempts = 0;

typedef struct { int x; int y; int valid; } position;
position new_position(int x, int y) { position p; p.x = x; p.y = y; p.valid = 1; return p; }

/* Code below unashamedly stolen from 'somewhere' to calculate difference (delta) between 2 timeval's */
int timeval_subtract (result, x, y) struct timeval *result, *x, *y;
{
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    return x->tv_sec < y->tv_sec;
}

void hunt(position start, int depth, const char board[BOARDSIZE*BOARDSIZE])
{
    attempts++;
    
    char my_board[BOARDSIZE * BOARDSIZE];
    memcpy(my_board, board, sizeof(my_board));
    
    my_board[(start.x * BOARDSIZE) + start.y] = 1;
    
    if (depth + 1 == BOARDSIZE * BOARDSIZE)
    {
        // Solution found
        //
        solutions++;
        printf(".");
        fflush(stdout);
        if (solutions % 10000 == 0) printf("Found %d solutions so far...\n", solutions);
        return;
    }
    
    // Determine possible moves
    //
    position possibles[8] = { 
        new_position(start.x - 1, start.y - 2), new_position(start.x - 2, start.y - 1),
        new_position(start.x + 1, start.y - 2), new_position(start.x + 2, start.y - 1),
        new_position(start.x + 1, start.y + 2), new_position(start.x + 2, start.y + 1),
        new_position(start.x - 1, start.y + 2), new_position(start.x - 2, start.y + 1),
    };
    
    int x;
    for (x = 0; x < 8; ++x)
    {
        // If position is within the bounds of the board, and not already taken, then hunt some more!
        //
        position* p = &possibles[x];
        if (p->x >= 0 && p->y >= 0 && p->x < BOARDSIZE && p->y < BOARDSIZE && board[(p->x * BOARDSIZE) + p->y] == 0)
            hunt(*p, depth + 1, my_board);
    }
}

void* thread_start_function(void* arg)
{
    int thread_num = (int)arg;
    printf("[%d]", thread_num);
    
    int skip = 0;
    
    int x, y;
    for (x = 0; x < BOARDSIZE; ++x)
        for (y = 0; y < BOARDSIZE; ++y)
        {
            if (skip == thread_num)
            {
                printf("(%d,%d)", x, y);
                hunt (new_position(x, y), 0, referenceboard);
            }
            
            skip++;
            if (skip >= THREADS) skip = 0;
        }
    
    printf("<%d>", thread_num);
    return 0;
}

int main(int argc, const char * argv[])
{
    struct timeval start;
    gettimeofday(&start, 0);
    
    // Set board to zero
    // 
    memset(referenceboard, 0, sizeof(referenceboard));
    
#if THREADS > 1
    printf("[Multithreaded (%d)]\n", THREADS);
    pthread_t threads[THREADS];
    int x;
    for (x = 0; x < THREADS; ++x)
        pthread_create(&threads[x], NULL, thread_start_function, x);
    for (x = 0; x < THREADS; ++x)
        pthread_join(threads[x], NULL);
#else
    printf("[Single threaded]\n");
    int x, y;
    for (x = 0; x < BOARDSIZE; ++x)
        for (y = 0; y < BOARDSIZE; ++y)
        {
            printf("(%d,%d)", x, y);
            hunt (new_position(x, y), 0, referenceboard);
        }
#endif
    
    struct timeval end;
    gettimeofday(&end, 0);
    
    struct timeval delta;
    timeval_subtract(&delta, &end, &start);
    
    printf("Found %d solutions in %ld.%ds (%d moves)\n", solutions, delta.tv_sec, delta.tv_usec, attempts);
    return 0;
}

