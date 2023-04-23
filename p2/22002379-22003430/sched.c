#include <pthread.h>
#include "readyqueue.h"

void schedule(int N)
{
    // Create N queues
    struct readyQueue *queues[N];
    for (int i = 0; i < N; i++)
    {
        queues[i] = (struct readyQueue *)malloc(sizeof(struct readyQueue));
    }

    // Create N threads
    pthread_t threads[N];
    int threadArgs[N][2];
    for (int i = 0; i < N; i++)
    {
        threadArgs[i][0] = i;
        threadArgs[i][1] = 0;
        pthread_create(&threads[i], NULL, worker, threadArgs[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Free queues
    for (int i = 0; i < N; i++)
    {
        free(queues[i]);
    }
}
