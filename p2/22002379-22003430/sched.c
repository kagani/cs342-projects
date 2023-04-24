#include "sched.h"

void *cpu(void *arg)
{
    // Get the arguments
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    int cpuIdx = threadArgs->id;
    SchedProps *schedProps = threadArgs->schedProps;
    ReadyQueue *queue = schedProps->queues[0];
    if (schedProps->queueSize > 1)
        queue = schedProps->queues[cpuIdx];
    struct timeval start; // REMOVE THIS LATER INCORRECT

    while (1)
    {
        // Wait for a job to arrive
        while (queue->size == 0)
        {
            usleep(1000);
        }

        // Lock the queue
        pthread_mutex_lock(&queue->mutex);

        // Do the job
        BurstItem bi = queue->head->data;
        printf("\n[+] CPU #%d is executing process #%d", cpuIdx, bi.pid);
        usleep(bi.remainingTime * 1000);
        bi.remainingTime = 0;
        bi.finishTime = getTime(&start);
        bi.turnaroundTime = bi.finishTime - bi.arrivalTime;
        printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, bi.pid);
        printf("\n[+] Process #%d has finished", bi.pid);
        printf("\n[+] Process #%d has a turnaround time of %d ms", bi.pid, bi.turnaroundTime);
        dequeue(queue);
        // Unlock the queue
        pthread_mutex_unlock(&queue->mutex);
    }
}

void schedule(SchedProps *schedProps)
{
    // Get the arguments
    int queueSize = schedProps->N;
    int N = schedProps->N;

    // Create N queues
    if (strcmp(schedProps->sap, "S") == 0)
    {
        queueSize = 1;
    }
    schedProps->queueSize = queueSize;
    ReadyQueue *queues[queueSize];
    schedProps->queues = queues;
    for (int i = 0; i < queueSize; i++)
    {
        queues[i] = (ReadyQueue *)malloc(sizeof(struct ReadyQueue));
        queues[i]->size = 0;
        queues[i]->head = NULL;
        queues[i]->tail = NULL;
        pthread_mutex_init(&queues[i]->mutex, NULL);
    }

    // Create N threads
    pthread_t threads[N];
    ThreadArgs *threadArgs[N];
    for (int i = 0; i < N; i++)
    {
        printf("Here!\n");
        fflush(stdout);
        threadArgs[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        threadArgs[i]->id = i;
        threadArgs[i]->schedProps = schedProps;
        pthread_create(&threads[i], NULL, cpu, threadArgs[i]);
    }

    // Start the scheduler
    if (strcmp(schedProps->infile, "") == 0)
    {
        sched_random(schedProps);
    }
    else
    {
        sched_file(schedProps);
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Free queues
    for (int i = 0; i < N && i < queueSize; i++)
    {
        free(queues[i]);
    }
}

void parse_and_enqueue(SchedProps *props, struct timeval *start)
{
    // Parse the input file and create jobs
    FILE *file = fopen(props->infile, "r");
    int nextPid = 1;
    int cpucnt = 0;

    if (!file)
        printf("[-] File does not exist.\n");

    fseek(file, 0, SEEK_END);
    long flength = ftell(file);
    fseek(file, 0, SEEK_SET);
    char text[flength + 1];
    fread(text, 1, flength, file);
    text[flength] = '\0';

    const char exceptions[] = " \t\r\n\v\f";
    char *tkn = strtok(text, exceptions);
    while (tkn != NULL)
    {
        int len = strlen(tkn);
        char word[len + 1];
        strcpy(word, tkn);
        BurstItem *bi;
        if (strcmp(word, "PL") == 0) // Process Length
        {
            tkn = strtok(NULL, exceptions);
            int burstLength = atoi(tkn);
            bi = (BurstItem *)malloc(sizeof(BurstItem));
            bi->pid = nextPid++;
            bi->burstLength = burstLength;
            bi->arrivalTime = getTime(start);
            bi->remainingTime = burstLength;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            bi->processorId = cpucnt;
            printf("\n[+] Enqueuing process #%d", nextPid);
            // enqueue(rq, *bi);
        }
        else if (strcmp(word, "IAT") == 0) // Interarrival Time
        {
            tkn = strtok(NULL, exceptions);
            int sleepTime = atoi(tkn);
            sleep(sleepTime / 1000);
        }
        else // Should not happen
        {
            printf("[-] Unknown token.");
        }

        tkn = strtok(NULL, exceptions); // Get the next token
    }
    fclose(file);
}

/**
 * @brief When the scheduler is invoked with the -i flag, this function is called.
 * Each line has information about another burst (PL) or another interarrival time
 * (IAT). After reading a burst information (a PL line) from the file, the program
 * (main thread) will create and prepare a burst item (a C re). A burst item
 * represents a cpu burst (i.e., a process) to be simulated (like a PCB).
 * @param SchedProps
 */
void sched_file(SchedProps *schedProps)
{
    struct timeval start;
    if (strcmp(schedProps->infile, "") == 0)
    {
        printf("[-] No input file specified. (sched_file)\n");
        exit(1);
    }
}

/**
 * @brief When the scheduler is invoked with the -r flag, this function is called.
 * If -r T T1 T2 L L1 L2 PC option is specified, then interarrival times and
 * burst times will be generated randomly inside the program (default values for
 * the parameters are 200, 10, 1000, 100, 10, 500, 10, respectively). T is the
 * mean interarrival time (approximately), T1 is the minimum acceptable
 * interarrival time and T2 is the maximum acceptable interarrival time. Similarly,
 * L is the mean burst length (approximately), L1 is the minimum acceptable
 * burst length and L2 is the maximum acceptable burst length. PC is the number
 * of bursts to simulate. The unit of all time related parameters is ms.
 * @param SchedProps
 */
void sched_random(SchedProps *SchedProps)
{
    struct timeval start;
}

// Method to compute the simulation time
long int getTime(struct timeval *start)
{
    struct timeval cur;
    gettimeofday(&cur, NULL);
    return (cur.tv_sec - start->tv_sec) * 1000 + (cur.tv_usec - start->tv_usec) / 1000;
}
