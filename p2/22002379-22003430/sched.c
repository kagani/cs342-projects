#include "sched.h"

void *cpu(void *arg)
{
    // Get the arguments
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    int cpuIdx = threadArgs->id;
    SchedProps *props = threadArgs->schedProps;
    if(props->outmode == RICH)
        printf("CPU #%d started\n", cpuIdx);
    struct timeval *start = &props->start;
    Queue *queue;
    long long execTime = 0;

    if (props->sap == SAP_SINGLE)
    {
        queue = props->queues[0];
        if(props->outmode == RICH)
            printf("CPU #%d is using the single queue\n", cpuIdx);
    }
    else
    {
        queue = props->queues[cpuIdx];
        if(props->outmode == RICH)
            printf("CPU #%d is using multi queue #%d\n", cpuIdx, cpuIdx);
    }

    while (1) // Place a flag here to stop the thread when the queue is empty and parsing is done
    {
        if(props->outmode == RICH)
            printf("Beginning of cpu loop\n");
        fflush(stdout);
        if (queue->head && queue->head->data->pid == -1)
        {
            break;
        }

        if(props->outmode == RICH)
            printf("Waiting for a job to arrive\n");
        fflush(stdout);
        // Wait for a job to arrive
        if(props->outmode == RICH)
            printf("Queue size: %d\n", queue->size);
        fflush(stdout);
        while (1) // Wait for job
        {
            pthread_mutex_lock(&queue->mutex);
            if (queue->size > 0)
            {
                break;
            }
            pthread_mutex_unlock(&queue->mutex);
            usleep(1000);
        }

        // Reached the end of the queue and exit the cpu loop
        if (queue->head && queue->head->data->pid == -1)
        {
            pthread_mutex_unlock(&queue->mutex);
            break;
        }

        // Do the job
        if (props->alg == ALG_FCFS)
        {
            BurstItem *burst = dequeue(queue);
            pthread_mutex_unlock(&queue->mutex);

            if (props->outmode == INFO)
                printf("\ntime=%lld, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d", get_time_diff(start), burst->processorId, burst->pid, burst->burstLength, burst->remainingTime);
            else if (props->outmode == RICH)
            {
                printf("\n[+] BURST PICKED BY CPU #%d: Burst pid = %d, Burst Length = %d, Arrival Time = %d, Remaining Time = %d", cpuIdx, burst->pid, burst->burstLength, burst->arrivalTime, burst->remainingTime);
            }

            if (props->outmode == RICH)
            {
                printf("\n[+] CPU #%d is executing process #%d", cpuIdx, burst->pid);
                fflush(stdout);
            }
            execTime = burst->burstLength;
            burst->remainingTime = 0;
            burst->processorId = cpuIdx;

            usleep(execTime * 1000); // Sleep for the execution time
            burst->finishTime = get_time_diff(start);
            burst->turnaroundTime = burst->finishTime - burst->arrivalTime;

            if (props->outmode == RICH)
            {
                printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, burst->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has finished", burst->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has a turnaround time of %d ms", burst->pid, burst->turnaroundTime);
                fflush(stdout);
            }

            pthread_mutex_lock(&props->finishedQueue->mutex);
            enqueue(props->finishedQueue, burst);
            pthread_mutex_unlock(&props->finishedQueue->mutex);
        }
        else if (props->alg == ALG_SJF)
        {
            BurstItem *burst;
            Node *cur = queue->head;
            Node *shortest = cur;
            while (cur != NULL)
            {
                if (cur->data->pid != -1 && cur->data->remainingTime < shortest->data->remainingTime)
                {
                    shortest = cur;
                }
                cur = cur->next;
            }

            burst = dequeue_at(queue, shortest->data->pid);
            pthread_mutex_unlock(&queue->mutex);

            if (props->outmode == INFO)
                printf("\ntime=%lld, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d", get_time_diff(start), burst->processorId, burst->pid, burst->burstLength, burst->remainingTime);
            else if (props->outmode == RICH)
            {
                printf("\n[+] BURST PICKED BY CPU #%d: Burst pid = %d, Burst Length = %d, Arrival Time = %d, Remaining Time = %d", cpuIdx, burst->pid, burst->burstLength, burst->arrivalTime, burst->remainingTime);
            }

            if (props->outmode == RICH)
            {
                printf("\n[+] CPU #%d is executing process #%d", cpuIdx, burst->pid);
                fflush(stdout);
            }

            execTime = burst->remainingTime;
            burst->remainingTime = 0;
            burst->processorId = cpuIdx;

            usleep(execTime * 1000); // Sleep for the execution time
            burst->finishTime = get_time_diff(start);
            burst->turnaroundTime = burst->finishTime - burst->arrivalTime;

            if (props->outmode == RICH)
            {
                printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, burst->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has finished", burst->pid);
                fflush(stdout);
                printf("\n[+] Process #%d has a turnaround time of %d ms", burst->pid, burst->turnaroundTime);
                fflush(stdout);
            }

            pthread_mutex_lock(&props->finishedQueue->mutex);
            enqueue(props->finishedQueue, burst);
            pthread_mutex_unlock(&props->finishedQueue->mutex);

            if(props->outmode == RICH)
                printf("Dequeued process #%d", burst->pid);
            fflush(stdout);
        }
        else if (props->alg == ALG_RR)
        {
            BurstItem *burst = dequeue(queue);
            pthread_mutex_unlock(&queue->mutex);

            if (props->outmode == INFO)
                printf("\ntime=%lld, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d", get_time_diff(start), burst->processorId, burst->pid, burst->burstLength, burst->remainingTime);
            else if (props->outmode == RICH)
            {
                printf("\n[+] BURST PICKED BY CPU #%d: Burst pid = %d, Burst Length = %d, Arrival Time = %d, Remaining Time = %d", cpuIdx, burst->pid, burst->burstLength, burst->arrivalTime, burst->remainingTime);
            }

            if (props->outmode == RICH)
            {
                printf("\n[+] CPU #%d is executing process #%d", cpuIdx, burst->pid);
                fflush(stdout);
            }

            if (burst->remainingTime > props->Q)
            {
                execTime = props->Q;
                burst->remainingTime -= props->Q;

                usleep(execTime * 1000); // Sleep for the execution time

                pthread_mutex_lock(&queue->mutex);
                if(props->outmode == RICH)
                    printf("Requeueing process #%d\n", burst->pid);
                enqueue(queue, burst);
                if(props->outmode == RICH)
                    printf("Requeued process #%d\n", burst->pid);
                pthread_mutex_unlock(&queue->mutex);
            }
            else
            {
                execTime = burst->remainingTime;
                burst->remainingTime = 0;

                usleep(execTime * 1000); // Sleep for the execution time
                burst->finishTime = get_time_diff(start);

                burst->turnaroundTime = burst->finishTime - burst->arrivalTime;
                burst->processorId = cpuIdx;
                if (props->outmode == RICH)
                {
                    printf("\n[+] CPU #%d has finished executing process #%d", cpuIdx, burst->pid);
                    fflush(stdout);
                    printf("\n[+] Process #%d has finished", burst->pid);
                    fflush(stdout);
                    printf("\n[+] Process #%d has a turnaround time of %d ms", burst->pid, burst->turnaroundTime);
                    fflush(stdout);
                }

                pthread_mutex_lock(&props->finishedQueue->mutex);
                enqueue(props->finishedQueue, burst);
                pthread_mutex_unlock(&props->finishedQueue->mutex);
            }
        }
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
        if(schedProps->outmode == RICH) {
            printf("\nFreeing queue %d", i);
            fflush(stdout);
        }

        free(queues[i]);
    }

    sort(schedProps->finishedQueue);

    if(schedProps->outmode == OFILE) {
        FILE* f = fopen(schedProps->outfile, "w");
        fprintf(f, "%3s  %3s  %8s  %3s  %7s  %11s  %10s", "pid", "cpu", "burstlen", "arv", "finish", "waitingtime", "turnaround");        Node *cur = schedProps->finishedQueue->head;
        int sum = 0;
        while (cur != NULL)
        {
            fprintf(f, "\n%3d  %3d  %8d  %3d  %7d  %11d  %10d", cur->data->pid, cur->data->processorId, cur->data->burstLength, cur->data->arrivalTime, cur->data->finishTime, cur->data->turnaroundTime - cur->data->burstLength, cur->data->turnaroundTime);
            sum += cur->data->turnaroundTime;
            cur = cur->next;
        }
        fprintf(f, "\naverage turnaround time: %d ms", sum/schedProps->finishedQueue->size);
    }
    else {
        printf("\n\n%3s  %3s  %8s  %3s  %7s  %11s  %10s", "pid", "cpu", "burstlen", "arv", "finish", "waitingtime", "turnaround");
        Node *cur = schedProps->finishedQueue->head;
        int sum = 0;
        while (cur != NULL)
        {
            printf("\n%3d  %3d  %8d  %3d  %7d  %11d  %10d", cur->data->pid, cur->data->processorId, cur->data->burstLength, cur->data->arrivalTime, cur->data->finishTime, cur->data->turnaroundTime - cur->data->burstLength, cur->data->turnaroundTime);
            sum += cur->data->turnaroundTime;
            cur = cur->next;
        }
        printf("\naverage turnaround time: %d ms", sum/schedProps->finishedQueue->size);
        printf("\n");
    }
}

void parse_and_enqueue(SchedProps *props)
{
    // Parse the input file and create jobs
    FILE *file = fopen(props->infile, "r");
    Queue **queues = props->queues;
    int nextPid = 1;
    int rr_queueIdx = 0; // for RR, Load Balancing needs something else

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
        if(props->outmode == RICH) {
            printf("Begin parsing word: %s\n", word);
            fflush(stdout);
        }

        if (strcmp(word, "PL") == 0) // Process Length
        {
            tkn = strtok(NULL, exceptions);
            int burstLength = atoi(tkn);
            if(props->outmode == RICH) {
                printf("\n[+] Burst length: %d", burstLength);
                fflush(stdout);
            }

            bi = (BurstItem *)malloc(sizeof(BurstItem));
            bi->pid = nextPid++;
            bi->burstLength = burstLength;
            bi->arrivalTime = get_time_diff(&props->start);
            bi->remainingTime = burstLength;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            if (props->outmode == RICH)
            {
                printf("\n[+] Enqueuing process #%d\n", nextPid - 1);
                fflush(stdout);
            }

            // Enqueue method
            if (props->qs == QS_RM)
            {
                if (props->outmode == RICH) {
                    printf("Queued process %d into queue %d\n", bi->pid, rr_queueIdx);
                    fflush(stdout);
                }
                bi->processorId = rr_queueIdx;
                pthread_mutex_lock(&queues[rr_queueIdx]->mutex);
                enqueue(queues[rr_queueIdx], bi);
                pthread_mutex_unlock(&queues[rr_queueIdx]->mutex);
                rr_queueIdx = (rr_queueIdx + 1) % props->queuesSize;
            }
            else if (props->qs == QS_LM)
            {
                // Get the shortest queue
                int shortestQueueIdx = 0;
                int shortestQueueLoad = queues[0]->queueLoad;
                for (int i = 0; i < props->queuesSize; i++)
                {
                    if (queues[i]->queueLoad < shortestQueueLoad)
                    {
                        shortestQueueIdx = i;
                        shortestQueueLoad = queues[i]->queueLoad;
                    }
                }

                // Enqueue
                bi->processorId = shortestQueueIdx;
                pthread_mutex_lock(&queues[shortestQueueIdx]->mutex);
                enqueue(queues[shortestQueueIdx], bi);
                pthread_mutex_unlock(&queues[shortestQueueIdx]->mutex);
            }
            else if (props->qs == QS_NA)
            {
                bi->processorId = 0;
                pthread_mutex_lock(&queues[0]->mutex);
                enqueue(queues[0], bi);
                pthread_mutex_unlock(&queues[0]->mutex);
            }
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
        pthread_mutex_lock(&queues[i]->mutex);
        enqueue(queues[i], bi);
        pthread_mutex_unlock(&queues[i]->mutex);
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
            BurstItem *bi = (BurstItem *)malloc(sizeof(BurstItem));
            bi->pid = nextPid++;
            bi->burstLength = bl;
            bi->arrivalTime = get_time_diff(&props->start);
            bi->remainingTime = bl;
            bi->finishTime = -1;
            bi->turnaroundTime = -1;
            if (props->outmode == RICH)
            {
                printf("\n[+] Enqueuing process #%d\n", nextPid - 1);
                fflush(stdout);
            }

            // Enqueue method
            if (props->qs == QS_RM)
            {
                bi->processorId = queueIdx;
                pthread_mutex_lock(&queues[queueIdx]->mutex);
                enqueue(queues[queueIdx], bi);
                pthread_mutex_unlock(&queues[queueIdx]->mutex);
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
                bi->processorId = shortestQueueIdx;
                pthread_mutex_lock(&queues[shortestQueueIdx]->mutex);
                enqueue(queues[shortestQueueIdx], bi);
                pthread_mutex_unlock(&queues[shortestQueueIdx]->mutex);
            }
            else if (props->qs == QS_NA)
            {
                pthread_mutex_lock(&queues[0]->mutex);
                enqueue(queues[0], bi);
                pthread_mutex_unlock(&queues[0]->mutex);
            }
        }
        else
        {
            usleep(generateRandom(props->L, props->L1, props->L2) * 1000);
        }
    }
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
        pthread_mutex_lock(&queues[i]->mutex);
        enqueue(queues[i], bi);
        pthread_mutex_unlock(&queues[i]->mutex);
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
