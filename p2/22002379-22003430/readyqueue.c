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
        newNode->prev = NULL;
    }
    else if (!list->tail)
    {
        list->tail = newNode;
        list->head->next = list->tail;
        list->tail->prev = list->head;
    }
    else
    {
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
    }

    list->size++;
    list->queueLoad += value->burstLength;
}

void requeue(Queue *list)
{
    if (!list->head)
        return;

    Node *cur = list->head;
    if (!cur->next) // size == 1
    {
        return;
    }

    list->head = list->head->next;
    list->head->prev = NULL;
    cur->next = NULL;
    cur->prev = NULL;

    if (list->tail && list->tail->data->pid == -1)
    {
        if (!list->tail->prev)
        {
            list->tail->prev = cur;
            cur->next = list->tail;
            list->head = cur;
            return;
        }
        list->tail->prev->next = cur;
        cur->prev = list->tail->prev;
        cur->next = list->tail;
        list->tail->prev = cur;
    }
    else
    {
        list->tail->next = cur;
        cur->prev = list->tail;
        list->tail = cur;
    }
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
    else
    {
        list->head->prev = NULL;
    }
    temp->next = NULL;
    temp->prev = NULL;

    list->size--;
    list->queueLoad -= temp->data->burstLength;
    enqueue(fq, temp->data);
    free(temp); // Doesn't free the BurstItem
}

void dequeue_at(Queue *list, Queue *fq, int idx)
{
    if (!list->head)
        return;
    Node *temp = list->head;
    while (idx--)
    {
        temp = temp->next;
    }

    if (temp->prev)
    {
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
    }
    else
    {
        list->head = temp->next;
        temp->next->prev = NULL;
    }

    if (!temp->next)
    {
        list->tail = temp->prev;
        temp->prev->next = NULL;
    }

    list->size--;
    list->queueLoad -= temp->data->burstLength;
    enqueue(fq, temp->data);
    free(temp); // Doesn't free the BurstItem
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
