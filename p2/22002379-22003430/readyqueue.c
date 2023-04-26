#include "readyqueue.h"
#include <stdio.h>
#include <stdlib.h>

void enqueue(Queue *list, BurstItem *value)
{
    printf("Enqueue start\n");
    fflush(stdout);
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = value;
    newNode->next = NULL;

    if (list->head == NULL)
    {
        list->head = newNode;
        newNode->prev = NULL;
    }
    else
    {
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
    }

    list->size++;
    list->queueLoad += value->burstLength;
    printf("Enqueue end\n");
    fflush(stdout);
}

void requeue(Queue *list)
{
    printf("Requeue start\n");
    fflush(stdout);
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

    printf("Requeue end\n");
    fflush(stdout);
}

void dequeue(Queue *list, Queue *fq)
{
    printf("Dequeue start\n");
    fflush(stdout);
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
    printf("dequeue end\n");
    fflush(stdout);
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
    printf("dequeue_at end\n");
    fflush(stdout);
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
