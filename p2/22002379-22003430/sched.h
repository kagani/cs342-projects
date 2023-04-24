#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "readyqueue.h"

typedef struct SchedProps SchedProps;

typedef struct SchedProps
{
    // Define the default values of the input parameters
    int N;  // Number of proessors (default 2)
    int Q;  // Quantum time for RR algorithm (default 20 ms)
    int T;  // Mean interarrival time (default 200 ms)
    int T1; // Minimum interarrival time (default 10 ms)
    int T2; // Maximum interarrival time (default 1000 ms)
    int L;  // Mean burst length (default 100 ms)
    int L1; // Minimum burst length (default 10 ms)
    int L2; // Maximum burst length (default 500 ms)
    int PC; // Number of bursts to simulate (default 10)
    int outmode;
    char sap[2];
    char qs[3];
    char alg[3];
    char infile[256];
    char outfile[256];
    int queueSize;
    ReadyQueue **queues; // Array of queues for each processor (size N) (1 for single queue)
} SchedProps;

typedef struct ThreadArgs
{
    int id;
    SchedProps *schedProps;
} ThreadArgs;

void *cpu(void *arg);
void schedule(SchedProps *schedProps);
void sched_file(SchedProps *schedProps);
void sched_random(SchedProps *schedProps);
long int getTime(struct timeval *start);