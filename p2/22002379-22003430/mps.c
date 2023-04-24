#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include "burstitem.h"
#include "readyqueue.h"
#include "sched.h"

// Method used to generate random interarrival time and burst lengths
int generateRandom(int T, int T1, int T2)
{
    srand(time(NULL));
    double x1 = T2 + 1;
    int ix1 = 0;
    while (ix1 < T1 || ix1 > T2)
    {
        double r1 = 1.0 / T;
        double u1 = 1.0 * rand() / RAND_MAX;
        x1 = (-1.0 * log(1.0 - u1)) / r1;
        ix1 = x1;
    }
    return ix1;
}

int main(int argc, char *argv[])
{

    // Get current time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Define the default values of the input parameters
    int N = 2;     // Number of proessors (default 2)
    int Q = 20;    // Quantum time for RR algorithm (default 20 ms)
    int T = 200;   // Mean interarrival time (default 200 ms)
    int T1 = 10;   // Minimum interarrival time (default 10 ms)
    int T2 = 1000; // Maximum interarrival time (default 1000 ms)
    int L = 100;   // Mean burst length (default 100 ms)
    int L1 = 10;   // Minimum burst length (default 10 ms)
    int L2 = 500;  // Maximum burst length (default 500 ms)
    int PC = 10;   // Number of bursts to simulate (default 10)
    int outmode = 1;
    char *sap = "M";
    char *qs = "RM";
    char *alg = "RR";
    char *infile = "";
    char *outfile = "out.txt";

    // Iterate over specified parameters to replace default values
    bool rSpecified = 0;
    bool iSpecified = 0;
    for (int i = 1; i < argc; i++)
    {
        char *cur = argv[i];
        if (strcmp(cur, "-n") == 0)
        {
            N = atoi(argv[++i]);
        }
        else if (strcmp(cur, "-a") == 0)
        {
            sap = argv[++i];
            qs = argv[++i];
        }
        else if (strcmp(cur, "-s") == 0)
        {
            alg = argv[++i];
            Q = atoi(argv[++i]);
        }
        else if (strcmp(cur, "-i") == 0)
        {
            infile = argv[++i];
            iSpecified = 1;
        }
        else if (strcmp(cur, "-m") == 0)
        {
            outmode = atoi(argv[++i]);
        }
        else if (strcmp(cur, "-o") == 0)
        {
            outfile = argv[++i];
        }
        else if (strcmp(cur, "-r") == 0)
        {
            T = atoi(argv[++i]);
            T1 = atoi(argv[++i]);
            T2 = atoi(argv[++i]);
            L = atoi(argv[++i]);
            L1 = atoi(argv[++i]);
            L2 = atoi(argv[++i]);
            rSpecified = 1;
        }
        else
        {
            printf("[-] Ignoring the unknown flag/argument: %s", cur);
        }
    }

    // Init the scheduler properties
    SchedProps *sp = (SchedProps *)malloc(sizeof(SchedProps));
    sp->N = N;
    sp->Q = Q;
    sp->T = T;
    sp->T1 = T1;
    sp->T2 = T2;
    sp->L = L;
    sp->L1 = L1;
    sp->L2 = L2;
    sp->PC = PC;
    sp->outmode = outmode;
    strcpy(sp->sap, sap);
    strcpy(sp->qs, qs);
    strcpy(sp->alg, alg);
    strcpy(sp->infile, infile);
    strcpy(sp->outfile, outfile);

    schedule(sp);

    // Check burst information generation method
    if (iSpecified & !rSpecified)
    {
        printf("\n[+] Generating from file");
    }
    else
    {
        printf("\n[+] Generating randomly");
    }
}