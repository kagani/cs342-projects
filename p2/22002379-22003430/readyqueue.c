#include "readyqueue.h"
#include <stdio.h>
#include <stdlib.h>

void enqueue(Queue *list, BurstItem *value)
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
        if (newNode->data->pid == -1)
            return;
        list->tail = newNode;
    }

    list->size++;
    list->queueLoad += value->burstLength;
}

void requeue(Queue *list)
{
    if (!list->head)
        return;
    if (list->head->data->pid == -1 || (list->head->next && list->head->next->data->pid == -1))
        return;
    Node *temp = list->head;
    list->head = list->head->next;
    temp->next = list->tail->next;
    list->tail = temp;
}

void dequeue(Queue *list, Queue *fq)
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
    list->queueLoad -= temp->data->burstLength;
    enqueue(fq, temp->data);
}

void dequeue_at(Queue *list, Queue *fq, int idx)
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
    list->queueLoad -= temp->data->burstLength;
    enqueue(fq, temp->data);
}

void printQueue(Queue *list)
{
    printf("\nLIST: ");
    Node *curr = list->head;
    while (curr != NULL)
    {
        printf("\n[pid: %d, bl: %d, at: %d, rt: %d, ft: %d, tt: %d, proc: %d]", curr->data->pid, curr->data->burstLength, curr->data->arrivalTime, curr->data->remainingTime, curr->data->finishTime, curr->data->turnaroundTime, curr->data->processorId);
        fflush(stdout);
        curr = curr->next;
    }
    printf("\n");
}
