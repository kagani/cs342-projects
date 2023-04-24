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
    pthread_mutex_t mutex;
} ReadyQueue;

void enqueue(ReadyQueue *q, BurstItem b);
void dequeue(ReadyQueue *q);
void printQueue(ReadyQueue *q);

#endif