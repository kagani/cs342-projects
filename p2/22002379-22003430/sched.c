#include "sched.h"

void *cpu(void *arg)
{
    // Get the arguments
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    int cpuIdx = threadArgs->id;
    printf("CPU #%d started\n", cpuIdx);
    SchedProps *props = threadArgs->schedProps;
    struct timeval *start = &props->start;
    Queue *queue;

    if (props->sap == SAP_SINGLE)
    {
        queue = props->queues[0];
        printf("CPU #%d is using the single queue\n", cpuIdx);
    }
    else
    {
        queue = props->queues[cpuIdx];
        printf("CPU #%d is using multi queue #%d\n", cpuIdx, cpuIdx);
    }

    while (1) // Place a flag here to stop the thread when the queue is empty and parsing is done
    {
        if (queue->head && queue->head->data->pid == -1)
        {
            break;
        }
        // Wait for a job to arrive
        while (queue->size == 0)
        {
            usleep(1000);
        }

        // Reached the end of the queue and exit the cpu loop
        if (queue->head && queue->head->data->pid == -1)
        {
            break;
        }

        // Lock the queue
        pthread_mutex_lock(&queue->mutex);

        // Do the job
        BurstItem *bi = queue->head->data;
        if (props->outmode == 2)
            printf("\ntime=%lld, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d", get_time_diff(start), bi->processorId, bi->pid, bi->burstLength, bi->remainingTime);
        else if (props->outmode == 3)
        {
            printf("\n[+] BURST PICKED BY CPU #%d: Burst pid = %d, Burst Length = %d, Arrival Time = %d, Remaining Time = %d", cpuIdx, bi->pid, bi->burstLength, bi->arrivalTime, bi->remainingTime);
        }

        if (props->alg == ALG_FCFS)
        {
            bi = queue->head->data;
            if (props->outmode == 3)
            {
                printf("\n[+] CPU #%d is executing process #%d", cpuIdx, bi->pid);
                fflush(stdout);
            }

            usleep(bi->remainingTime * 1000);
            bi->remainingTime = 0;
            bi->finishTime = get_time_diff(start);
            bi->turnaroundTime = bi->finishTime - bi->arrivalTime;
            if (props->outmode == 3)
            {
                printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, bi->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has finished", bi->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has a turnaround time of %d ms", bi->pid, bi->turnaroundTime);
                fflush(stdout);
            }
            dequeue(queue, props->finishedQueue);
        }
        else if (props->alg == ALG_SJF)
        {
            bi = queue->head->data;
            Node *curr = queue->head;
            while (curr != NULL)
            {
                if (curr->data->remainingTime < bi->remainingTime)
                {
                    bi = curr->data;
                }
                curr = curr->next;
            }
            if (props->outmode == 3)
            {
                printf("\n[+] CPU #%d is executing process #%d", cpuIdx, bi->pid);
                fflush(stdout);
            }
            usleep(bi->remainingTime * 1000);
            bi->remainingTime = 0;
            bi->finishTime = get_time_diff(start);
            bi->turnaroundTime = bi->finishTime - bi->arrivalTime;
            if (props->outmode == 3)
            {
                printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, bi->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has finished", bi->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has a turnaround time of %d ms", bi->pid, bi->turnaroundTime);
                fflush(stdout);
            }
            dequeue(queue, props->finishedQueue);
        }
        else if (props->alg == ALG_RR)
        {
            bi = queue->head->data;
            if (props->outmode == 3)
            {
                printf("\n[+] CPU #%d is executing process #%d", cpuIdx, bi->pid);
                fflush(stdout);
            }

            if (bi->remainingTime > props->Q)
            {
                usleep(props->Q * 1000);
                bi->remainingTime -= props->Q;
                requeue(queue);
            }
            else
            {
                usleep(bi->remainingTime * 1000);
                bi->remainingTime = 0;
                bi->finishTime = get_time_diff(start);
                bi->turnaroundTime = bi->finishTime - bi->arrivalTime;
                if (props->outmode == 3)
                {
                    printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, bi->pid);
                    fflush(stdout);
                    printf("\n[+] Process #%d has finished", bi->pid);
                    fflush(stdout);
                    printf("\n[+] Process #%d has a turnaround time of %d ms", bi->pid, bi->turnaroundTime);
                    fflush(stdout);
                }
                dequeue(queue, props->finishedQueue);
            }
        }

        // Unlock the queue
        pthread_mutex_unlock(&queue->mutex);
    }

    if (props->sap == SAP_MULTI) // Destroy when all threads are done for single queue
    {
        pthread_mutex_destroy(&queue->mutex); // Destroy the mutex
    }

    pthread_exit(NULL);
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
    Queue *queues[queuesSize];
    schedProps->queues = queues;
    for (int i = 0; i < queuesSize; i++)
    {
        queues[i] = (Queue *)malloc(sizeof(struct Queue));
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

    // Destroy mutex for single queue
    if (schedProps->sap == SAP_SINGLE)
    {
        pthread_mutex_destroy(&queues[0]->mutex);
    }

    // Free queues
    for (int i = 0; i < N && i < queuesSize; i++)
    {
        free(queues[i]);
    }

    printf("\n\n%3s  %3s  %8s  %3s  %7s  %11s  %10s", "pid", "cpu", "burstlen", "arv", "finish", "waitingtime", "turnaround");
    Node* cur = schedProps->finishedQueue->head;
    while(cur!=NULL) {
        printf("\n%3d  %3d  %8d  %3d  %7d  %11d  %10d", cur->data->pid, cur->data->processorId, cur->data->burstLength, cur->data->arrivalTime, cur->data->finishTime, cur->data->finishTime-cur->data->arrivalTime, cur->data->turnaroundTime);
        cur = cur->next;
    }
    printf("\n");
}

void parse_and_enqueue(SchedProps *props)
{
    // Parse the input file and create jobs
    FILE *file = fopen(props->infile, "r");
    Queue **queues = props->queues;
    int nextPid = 1;
    int queueIdx = 0; // for RR, Load Balancing needs something else
    int curIdx = 0;   // For mutex

    if (!file)
    {
        printf("\n[-] File does not exist.");
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
            printf("\n[+] Burst length: %d", burstLength);
            fflush(stdout);
            bi = (BurstItem *)malloc(sizeof(BurstItem));
            bi->pid = nextPid++;
            bi->burstLength = burstLength;
            bi->arrivalTime = get_time_diff(&props->start);
            bi->remainingTime = burstLength;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            bi->processorId = queueIdx;
            pthread_mutex_lock(&queues[queueIdx]->mutex); // Change pos
            if (props->outmode == 3)
            {
                printf("\n[+] Enqueuing process #%d\n", nextPid - 1);
                fflush(stdout);
            }

            // Enqueue method
            if (props->qs == QS_RM)
            {
                printf("Queued process %d into queue %d\n", bi->pid, queueIdx);
                enqueue(queues[queueIdx], bi);
                curIdx = queueIdx;
                queueIdx = (queueIdx + 1) % props->queuesSize;
            }
            else if (props->qs == QS_LM)
            {
                // Get the shortest queue
                int shortestQueueIdx = 0;
                int shortestQueueLoad = queues[0]->queueLoad;
                printf("Props queues size: %d\n", props->queuesSize);
                fflush(stdout);
                for (int i = 0; i < props->queuesSize; i++)
                {
                    printf("Queue %d load: %lld\n", i, queues[i]->queueLoad);
                    fflush(stdout);
                    if (queues[i]->queueLoad < shortestQueueLoad)
                    {
                        shortestQueueIdx = i;
                        shortestQueueLoad = queues[i]->queueLoad;
                    }
                }

                // Enqueue
                queueIdx = shortestQueueIdx;
                printf("LM Queued process %d into queue %d\n", bi->pid, queueIdx);
                fflush(stdout);
                enqueue(queues[shortestQueueIdx], bi);
            }
            else if (props->qs == QS_NA)
            {
                queueIdx = 0;
                enqueue(queues[0], bi);
            }
            pthread_mutex_unlock(&queues[curIdx]->mutex);
        }
        else if (strcmp(word, "IAT") == 0) // Interarrival Time
        {
            tkn = strtok(NULL, exceptions);
            int sleepTime = atoi(tkn);
            usleep(sleepTime * 1000);
        }
        else // Should not happen
        {
            printf("\n[-] Unknown token.");
        }

        tkn = strtok(NULL, exceptions); // Get the next token
    }

    // Add a dummy item to the end of every queue
    for (int i = 0; i < props->queuesSize; i++)
    {
        BurstItem *bi = (BurstItem *)malloc(sizeof(BurstItem));
        bi->pid = -1;
        bi->burstLength = 0;
        bi->arrivalTime = 0;
        bi->remainingTime = 0;
        bi->finishTime = 0;
        bi->turnaroundTime = 0;
        bi->processorId = 0;
        enqueue(queues[i], bi);
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
        printf("\n[-] No input file specified. (sched_file)");
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
    Queue **queues = props->queues;
    int nextPid = 1;
    int queueIdx = 0; // for RR, Load Balancing needs something else

    for (int i = 0; i < 2 * props->PC - 1; i++)
    {
        if (i % 2 == 0)
        {
            int bl = generateRandom(props->T, props->T1, props->T2);
            pthread_mutex_lock(&queues[queueIdx]->mutex);
            BurstItem *bi = (BurstItem *)malloc(sizeof(BurstItem));
            bi->pid = nextPid++;
            bi->burstLength = bl;
            bi->arrivalTime = get_time_diff(&props->start);
            bi->remainingTime = bl;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            bi->processorId = queueIdx;

            // Enqueue method
            if (props->qs == QS_RM)
            {
                enqueue(queues[queueIdx], bi);
                queueIdx = (queueIdx + 1) % props->queuesSize;
            }
            else if (props->qs == QS_LM)
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
                enqueue(queues[shortestQueueIdx], bi);
            }
            else if (props->qs == QS_NA)
            {
                enqueue(queues[0], bi);
            }
            pthread_mutex_unlock(&queues[queueIdx]->mutex);
        }
        else
        {
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
