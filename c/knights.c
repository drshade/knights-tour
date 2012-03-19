

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define BOARDSIZE 5
#define THREADS 4

char referenceboard[BOARDSIZE * BOARDSIZE];

int solutions = 0;

typedef struct { int x; int y; int valid; } position;
position new_position(int x, int y) { position p; p.x = x; p.y = y; p.valid = 1; return p; }

int timeval_subtract (result, x, y) struct timeval *result, *x, *y;
{
    /* Perform the carry for the later subtraction by updating y. */
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
    
    /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    
    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}


position* available_positions(position start, char board[BOARDSIZE * BOARDSIZE])
{
    position possibles[8] = { 
        new_position(start.x - 1, start.y - 2), new_position(start.x - 2, start.y - 1),
        new_position(start.x + 1, start.y - 2), new_position(start.x + 2, start.y - 1),
        new_position(start.x + 1, start.y + 2), new_position(start.x + 2, start.y + 1),
        new_position(start.x - 1, start.y + 2), new_position(start.x - 2, start.y + 1),
    };

    int x;
    for (x = 0; x < 8; ++x)
    {
        position* p = &possibles[x];
        if (!(p->x >= 0 && p->y >= 0 && p->x < BOARDSIZE && p->y < BOARDSIZE && board[(p->x * BOARDSIZE) + p->y] == 0))
            p->valid = 0;
    }
    
    position* possibles_for_return = malloc(sizeof(possibles));
    memcpy(possibles_for_return, possibles, sizeof(possibles));
    
    return possibles_for_return;
}

void hunt(position start, int depth, const char board[BOARDSIZE*BOARDSIZE])
{
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
    
    position* positions = available_positions(start, my_board);
    int x;
    for (x = 0; x < 8; ++x)
    {
        position p = positions[x];
        if (p.valid)
            hunt(p, depth + 1, my_board);
    }
    free(positions);
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
    
    // insert code here...
    printf("Found %d solutions in %ld.%ds\n", solutions, delta.tv_sec, delta.tv_usec);
    return 0;
}

