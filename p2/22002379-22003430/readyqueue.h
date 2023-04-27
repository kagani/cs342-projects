#include <pthread.h>
#include "burstitem.h"

#ifndef READYQUEUE_H
#define READYQUEUE_H

typedef struct Node Node;
typedef struct Queue Queue;

typedef struct Node
{
    BurstItem *data;
    Node *next;
    Node *prev;
} Node;

typedef struct Queue
{
    Node *head;
    Node *tail;
    int size;
    long long queueLoad;
    pthread_mutex_t mutex;
} Queue;

/**
 * @brief Enqueue a new item to the end of the queue
 *
 * @param q The queue to enqueue to
 * @param b The burst item to enqueue
 */
void enqueue(Queue *q, BurstItem *b);

/**
 * @brief Dequeue the first item in the queue
 *
 * @param rq
 * @return BurstItem*
 */
BurstItem *dequeue(Queue *rq);

/**
 * @brief Dequeue the item with the given pid
 *
 * @param list
 * @param pid
 * @return BurstItem*
 */
BurstItem *dequeue_at(Queue *list, int pid);

/**
 * @brief Print the queue
 *
 * @param q
 */
void printQueue(Queue *q);

/**
 * @brief Sort the queue based on pid
 *
 * @param q
 */
void sort(Queue *q);

#endif
