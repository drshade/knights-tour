
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define BOARDSIZE 5
#define THREADS 8

char referenceboard[BOARDSIZE * BOARDSIZE];

int solutions = 0, attempts = 0;

typedef struct { int x; int y; int valid; } position;
position new_position(int x, int y) { position p; p.x = x; p.y = y; p.valid = 1; return p; }

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
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);

	LARGE_INTEGER liStart;
	LARGE_INTEGER liStop;

	QueryPerformanceCounter(&liStart);

    // Set board to zero
    // 
    memset(referenceboard, 0, sizeof(referenceboard));
    
#if THREADS > 1
    printf("[Multithreaded (%d)]\n", THREADS);
    HANDLE threads[THREADS];
    int x;
    for (x = 0; x < THREADS; ++x)
        threads[x] = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            (LPTHREAD_START_ROUTINE)&thread_start_function,       // thread function name
            (LPVOID)x,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);
    
	WaitForMultipleObjects(THREADS, threads, TRUE, INFINITE);

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
    
	QueryPerformanceCounter(&liStop);
    
	LONGLONG llTimeDiff = liStop.QuadPart - liStart.QuadPart;

	// To get duration in milliseconds
	double dftDuration = (double) llTimeDiff * 1000.0 / (double) Frequency.QuadPart;

    printf("Found %d solutions in %fms (%d moves)\n", solutions, dftDuration, attempts);
    return 0;
}

