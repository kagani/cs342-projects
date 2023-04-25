#include "sched.h"

void *cpu(void *arg)
{
    // Get the arguments
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    int cpuIdx = threadArgs->id;
    SchedProps *schedProps = threadArgs->schedProps;
    struct timeval start = schedProps->start;
    ReadyQueue *queue;

    if (schedProps->sap == SAP_SINGLE)
    {
        queue = schedProps->queues[0];
    }
    else
    {
        queue = schedProps->queues[cpuIdx];
    }

    while (1) // Place a flag here to stop the thread when the queue is empty and parsing is done
    {
        if (queue->head && queue->head->data.pid == -1)
        {
            return;
        }
        // Wait for a job to arrive
        while (queue->size == 0)
        {
            if (queue->head && queue->head->data.pid == -1)
            {
                return;
            }
            usleep(1000);
        }

        if (queue->head && queue->head->data.pid == -1)
        {
            return;
        }

        // Lock the queue
        pthread_mutex_lock(&queue->mutex);
        printf("Locked queue\n");
        fflush(stdout);

        printf("Queue size: %d", queue->size);
        fflush(stdout);

        // Do the job
        BurstItem bi = queue->head->data; // NOT HEAD SELECT ACCORDING TO TIME
        printf("[+] CPU #%d is executing process #%d\n", cpuIdx, bi.pid);
        fflush(stdout);
        usleep(bi.remainingTime * 1000);
        bi.remainingTime = 0;
        bi.finishTime = get_time_diff(&start);
        bi.turnaroundTime = bi.finishTime - bi.arrivalTime;
        printf("[+] CPU #%d has finished executing process #%d\n", cpuIdx, bi.pid);
        fflush(stdout);
        printf("[+] Process #%d has finished\n", bi.pid);
        fflush(stdout);
        printf("[+] Process #%d has a turnaround time of %d ms\n", bi.pid, bi.turnaroundTime);
        fflush(stdout);
        dequeue(queue);

        // Unlock the queue
        pthread_mutex_unlock(&queue->mutex);
        printf("Unlocked queue\n");
        fflush(stdout);
    }
}

void schedule(SchedProps *schedProps)
{
    // Get the arguments
    int queuesSize = schedProps->N;
    int N = schedProps->N;

    // Create N queues
    if (schedProps->sap == SAP_SINGLE)
    {
        queuesSize = 1;
    }

    schedProps->queuesSize = queuesSize;
    ReadyQueue *queues[queuesSize];
    schedProps->queues = queues;
    for (int i = 0; i < queuesSize; i++)
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
    if (schedProps->source == SOURCE_RANDOM)
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
    for (int i = 0; i < N && i < queuesSize; i++)
    {
        free(queues[i]);
    }
}

void parse_and_enqueue(SchedProps *props)
{
    // Parse the input file and create jobs
    FILE *file = fopen(props->infile, "r");
    ReadyQueue **queues = props->queues;
    int nextPid = 1;
    int queueIdx = 0; // for RR, Load Balancing needs something else

    if (!file)
    {
        printf("[-] File does not exist.\n");
        return;
    }

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
            bi->arrivalTime = get_time_diff(&props->start);
            bi->remainingTime = burstLength;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            bi->processorId = queueIdx;
            pthread_mutex_lock(&queues[queueIdx]->mutex);
            printf("\n[+] Enqueuing process #%d", nextPid);
            fflush(stdout);

            // Enqueue method
            if (props->qs == QS_RR)
            {
                enqueue(queues[queueIdx], *bi);
                queueIdx = (queueIdx + 1) % props->queuesSize;
            }
            else if (props->qs == QS_LB)
            {
                // Get the shortest queue
                int shortestQueueIdx = 0;
                int shortestQueueLoad = queues[0]->queueLoad;
                for (int i = 1; i < props->queuesSize; i++)
                {
                    if (queues[i]->size < shortestQueueLoad)
                    {
                        shortestQueueIdx = i;
                        shortestQueueLoad = queues[i]->queueLoad;
                    }
                }

                // Enqueue
                enqueue(queues[shortestQueueIdx], *bi);
            }
            else if (props->qs == QS_NA)
            {
                enqueue(queues[0], *bi);
            }

            pthread_mutex_unlock(&queues[queueIdx]->mutex);
        }
        else if (strcmp(word, "IAT") == 0) // Interarrival Time
        {
            tkn = strtok(NULL, exceptions);
            int sleepTime = atoi(tkn);
            usleep(sleepTime * 1000);
        }
        else // Should not happen
        {
            printf("[-] Unknown token.");
        }

        tkn = strtok(NULL, exceptions); // Get the next token
    }

    // Add a dummy item to the end of every queue
    for (int i = 0; i < props->queuesSize; i++)
    {
        BurstItem *bi = (BurstItem *)malloc(sizeof(BurstItem));
        bi->pid = -1;
        bi->burstLength = -1;
        bi->arrivalTime = -1;
        bi->remainingTime = -1;
        bi->finishTime = -1;
        bi->turnaroundTime = -1;
        bi->processorId = -1;
        enqueue(queues[i], *bi);
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
void sched_file(SchedProps *props)
{
    // Capture start time
    gettimeofday(&props->start, NULL);

    if (strcmp(props->infile, "") == 0)
    {
        printf("[-] No input file specified. (sched_file)\n");
        exit(1);
    }

    parse_and_enqueue(props);
}

/**
 * @brief Method to generate a random burst length and interarrival time between
 * the specified parameters specified with the -r flag. The random interarrival 
 * times or burst lengths selected with this method will not be exactly
 * exponentially distributed but random enough.
 * @return int
 */
int generateRandom(int T, int T1, int T2)
{   
    struct timeval t;
    gettimeofday(&t, NULL);
    srand((unsigned)t.tv_usec);
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
void sched_random(SchedProps *props)
{
    // Capture start time
    gettimeofday(&props->start, NULL);
    ReadyQueue **queues = props->queues;
    int nextPid = 1;
    int queueIdx = 0; // for RR, Load Balancing needs something else
    
    for(int i = 0; i < 2*props->PC-1; i++) {
        if(i%2==0) {
            int bl = generateRandom(props->T, props->T1, props->T2);
            pthread_mutex_lock(&queues[queueIdx]->mutex);
            BurstItem* bi = (BurstItem*) malloc(sizeof(BurstItem));
            bi->pid = nextPid++;
            bi->burstLength = bl;
            bi->arrivalTime = get_time_diff(&props->start);
            bi->remainingTime = bl;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            bi->processorId = queueIdx;

            // Enqueue method
            if (props->qs == QS_RR)
            {
                printf("aaa %d", bi->pid);
                enqueue(queues[queueIdx], *bi);
                queueIdx = (queueIdx + 1) % props->queuesSize;
            }
            else if (props->qs == QS_LB)
            {
                // Get the shortest queue
                int shortestQueueIdx = 0;
                int shortestQueueLoad = queues[0]->queueLoad;
                for (int i = 1; i < props->queuesSize; i++)
                {
                    if (queues[i]->size < shortestQueueLoad)
                    {
                        shortestQueueIdx = i;
                        shortestQueueLoad = queues[i]->queueLoad;
                    }
                }

                // Enqueue
                enqueue(queues[shortestQueueIdx], *bi);
            }
            else if (props->qs == QS_NA)
            {
                enqueue(queues[0], *bi);
            }
            pthread_mutex_unlock(&queues[queueIdx]->mutex);
        }
        else {
            usleep(generateRandom(props->L, props->L1, props->L2) * 1000);
        }
    }
}

/**
 * @brief Get the current time in milliseconds EPOCH time
 *
 * @return long long
 */
long long get_current_time()
{
    struct timeval cur;
    gettimeofday(&cur, NULL);
    return (cur.tv_sec) * 1000 + (cur.tv_usec) / 1000;
}

/**
 * @brief Difference between now and start time in milliseconds
 *
 * @return long long
 */
long long get_time_diff(struct timeval *start)
{
    struct timeval cur;
    gettimeofday(&cur, NULL);
    return (cur.tv_sec - start->tv_sec) * 1000 + (cur.tv_usec - start->tv_usec) / 1000;
}
