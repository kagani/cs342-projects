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
} Node;

typedef struct Queue
{
    Node *head;
    Node *tail;
    int size;
    long long queueLoad;
    pthread_mutex_t mutex;
} Queue;

void enqueue(Queue *q, BurstItem *b);
/**
 * @brief Dequeue the first item in the queue
 *
 * @param q
 */

void dequeue(Queue *rq, Queue *fq);
/**
 * @brief Dequeue the item at the given index
 *
 * @param list
 * @param idx
 */
void dequeue_at(Queue *list, Queue *fq, int idx);

/**
 * @brief Requeue the first item in the queue to the end of the queue
 *
 * @param list
 */
void requeue(Queue *list);
void printQueue(Queue *q);

#endif