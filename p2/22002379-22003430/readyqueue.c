#include "readyqueue.h"
#include <stdio.h>
#include <stdlib.h>

void enqueue(Queue *list, BurstItem *value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = value;
    newNode->next = NULL;
    newNode->prev = NULL;

    list->size++;
    list->queueLoad += value->burstLength;

    // Check if list is empty
    if (!list->head)
    {
        list->head = newNode;
        list->tail = newNode;
        return;
    }

    // Marker check size = 1
    if (list->head->data->pid == -1)
    {
        list->head = newNode;
        list->head->next = list->tail;
        list->tail->prev = list->head;
        return;
    }

    // Marker check
    if (list->tail->data->pid == -1)
    {
        list->tail->prev->next = newNode;
        newNode->prev = list->tail->prev;
        newNode->next = list->tail;
        list->tail->prev = newNode;
        return;
    }

    // Add to end of list
    list->tail->next = newNode;
    newNode->prev = list->tail;
    list->tail = newNode;
}

BurstItem *dequeue(Queue *list)
{
    if (!list->head)
        return NULL;
    if (!list->head->data->pid == -1)
    {
        printf("Error: dequeueing marker\n");
        fflush(stdout);
        return NULL;
    }

    Node *cur = list->head;
    BurstItem *res = cur->data;

    list->head = list->head->next;

    if (list->head)
        list->head->prev = NULL;
    else // size = 1, tail == cur
        list->tail = NULL;

    cur->next = NULL;
    cur->prev = NULL;
    free(cur); // Doesn't free the BurstItem

    list->size--;
    list->queueLoad -= res->burstLength;

    return res;
}

BurstItem *dequeue_at(Queue *list, int pid)
{
    if (!list->head)
        return NULL;

    Node *cur = list->head;

    while (cur && cur->data->pid != pid)
    {
        cur = cur->next;
    }

    if (!cur)
        return NULL;

    if (cur == list->head)
    {
        return dequeue(list);
    }

    cur->prev->next = cur->next;
    if (cur->next)
        cur->next->prev = cur->prev;
    else
        list->tail = cur->prev;

    cur->next = NULL;
    cur->prev = NULL;

    list->size--;
    list->queueLoad -= cur->data->burstLength;

    BurstItem *res = cur->data;
    free(cur); // Doesn't free the BurstItem
    return res;
}

void sort(Queue *q)
{
    if (!q->head)
        return;

    for (Node *i = q->head; i->next != NULL; i = i->next)
    {
        for (Node *j = i->next; j != NULL; j = j->next)
        {
            if (i->data->pid > j->data->pid)
            {
                BurstItem *temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }
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
