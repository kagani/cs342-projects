#include "readyqueue.h"
#include <stdio.h>
#include <stdlib.h>

void enqueue(ReadyQueue *list, BurstItem value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = value;
    newNode->next = NULL;

    if (list->head == NULL)
    {
        list->head = newNode;
        list->tail = newNode;
    }
    else
    {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    list->size++;
    list->queueLoad += value.burstLength;
}

void requeue(ReadyQueue *list)
{
    if (!list->head)
        return;
    Node *temp = list->head;
    list->head = list->head->next;
    if (!list->head)
    {
        list->tail = NULL;
    }
    list->size--;
    list->queueLoad -= temp->data.burstLength;
    temp->next = NULL;
    list->tail->next = temp;
    list->tail = temp;
}

void dequeue(ReadyQueue *list)
{
    if (!list->head)
        return;
    Node *temp = list->head;
    list->head = list->head->next;
    if (!list->head)
    {
        list->tail = NULL;
    }
    list->size--;
    list->queueLoad -= temp->data.burstLength;
    free(temp);
}

void dequeue_at(ReadyQueue *list, int idx)
{
    if (!list->head)
        return;
    Node *temp = list->head;
    Node *prev = NULL;
    while (idx--)
    {
        prev = temp;
        temp = temp->next;
    }

    if (prev)
    {
        prev->next = temp->next;
    }
    else
    {
        list->head = temp->next;
    }

    if (!temp->next)
    {
        list->tail = prev;
    }

    list->size--;
    list->queueLoad -= temp->data.burstLength;
    free(temp);
}

void printQueue(ReadyQueue *list)
{
    printf("\nLIST: ");
    Node *curr = list->head;
    while (curr != NULL)
    {
        printf("\n[pid: %d, bl: %d, at: %d, rt: %d, ft: %d, tt: %d, proc: %d]", curr->data.pid, curr->data.burstLength, curr->data.arrivalTime, curr->data.remainingTime, curr->data.finishTime, curr->data.turnaroundTime, curr->data.processorId);
        curr = curr->next;
    }
    printf("\n");
}