#include <pthread.h>
#include "burstitem.h"

#ifndef READYQUEUE_H
#define READYQUEUE_H

typedef struct Node Node;
typedef struct ReadyQueue ReadyQueue;

typedef struct Node
{
    BurstItem data;
    Node *next;
} Node;

typedef struct ReadyQueue
{
    Node *head;
    Node *tail;
    int size;
    long long queueLoad;
    pthread_mutex_t mutex;
} ReadyQueue;

void enqueue(ReadyQueue *q, BurstItem b);
/**
 * @brief Dequeue the first item in the queue
 *
 * @param q
 */

void dequeue(ReadyQueue *q);
/**
 * @brief Dequeue the item at the given index
 *
 * @param list
 * @param idx
 */
void dequeue_at(ReadyQueue *list, int idx);

/**
 * @brief Requeue the first item in the queue to the end of the queue
 *
 * @param list
 */
void requeue(ReadyQueue *list);
void printQueue(ReadyQueue *q);

#endif