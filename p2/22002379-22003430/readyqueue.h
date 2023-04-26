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
 * @param q The queue to dequeue from
 * @param fq The queue to enqueue the dequeued item to
 */
void dequeue(Queue *rq, Queue *fq);

/**
 * @brief Dequeue the item at the given pid
 *
 * @param list The queue to dequeue from
 * @param fq The queue to enqueue the dequeued item to
 * @param pid The pid of the item to dequeue
 */
void dequeue_at(Queue *list, Queue *fq, int pid);

/**
 * @brief Requeue the first item in the queue to the end of the queue
 *
 * @param list
 */
void requeue(Queue *list);

/**
 * @brief Print the queue
 *
 * @param q
 */
void printQueue(Queue *q);

#endif
