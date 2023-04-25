#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "readyqueue.h"

typedef struct SchedProps SchedProps;

typedef enum SchedulingAlgorithm // -s ALG
{
    ALG_FCFS,
    ALG_SJF,
    ALG_RR
} SchedulingAlgorithm;

typedef enum SchedulingApproach // -a SAP
{
    SAP_MULTI,
    SAP_SINGLE
} SchedulingApproach;

typedef enum OutputMode // -m OUTMODE
{
    CONSOLE,
    FILEOUT
} OutputMode;

typedef enum QueueSelection // -a QS
{
    QS_NA, // Single queue goes to end of queue
    QS_RR, // Iterates through queues => i % N
    QS_LB  // Least load => min(sum of burst lengths) & min(id) for ties
} QueueSelection;

typedef enum SchedhuleSource
{
    SOURCE_FILE,
    SOURCE_RANDOM
} SchedhuleSource;

typedef struct SchedProps
{
    // Define the default values of the input parameters
    int N;  // Number of proessors (default 2)
    int Q;  // Quantum time for RR algorithm (default 20 ms) (0 for FCFS, SJF)
    int T;  // Mean interarrival time (default 200 ms)
    int T1; // Minimum interarrival time (default 10 ms)
    int T2; // Maximum interarrival time (default 1000 ms)
    int L;  // Mean burst length (default 100 ms)
    int L1; // Minimum burst length (default 10 ms)
    int L2; // Maximum burst length (default 500 ms)
    int PC; // Number of bursts to simulate (default 10)
    OutputMode outmode;
    SchedulingApproach sap;  // Single or multi queue (default SAP_MULTI)
    QueueSelection qs;       // Queue selection algorithm (default QS_RM)
    SchedulingAlgorithm alg; // Scheduling algorithm (default ALG_RR)
    char infile[256];        // Input file (default "in.txt")
    char outfile[256];       // No default, but if specified, cannot output to console
    SchedhuleSource source;  // Source of schedule (default SOURCE_FILE)
    int queuesSize;          // Size of queues array (N for multi queue, 1 for single queue)
    ReadyQueue **queues;     // Array of queues for each processor (size N) (1 for single queue)
    struct timeval start;    // Start time of simulation (EPOCH timestamp in us)
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
long long get_current_time();
long long get_time_diff(struct timeval *start);
