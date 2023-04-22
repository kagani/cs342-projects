#include "burstitem.h"

#ifndef READYQUEUE_H
#define READYQUEUE_H

struct Node {   
    struct BurstItem data;
    struct Node* next;
};

struct ReadyQueue {
    struct Node* head;
    struct Node* tail;
};

void enqueue(struct ReadyQueue* q, struct BurstItem b);
void dequeue(struct ReadyQueue* q);
void printQueue(struct ReadyQueue* q);

#endif