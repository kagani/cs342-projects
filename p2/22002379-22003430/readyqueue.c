#include "readyqueue.h"
#include <stdio.h>
#include <stdlib.h>

void enqueue(struct ReadyQueue* list, struct BurstItem value) {
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    newNode->data = value;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

void dequeue(struct ReadyQueue* list) {
    if(list->head == NULL)
        return;
    if(list->head->next == NULL) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        return;
    }
    struct Node* temp = list->head->next;
    free(list->head);
    list->head = temp;
}

void printQueue(struct ReadyQueue* list) {
    printf("\nLIST: ");
    struct Node* curr = list->head;
    while (curr != NULL) {
        printf("\n[pid: %d, bl: %d, at: %d, rt: %d, ft: %d, tt: %d, proc: %d]", curr->data.pid, curr->data.burstLength, curr->data.arrivalTime, curr->data.remainingTime, curr->data.finishTime, curr->data.turnaroundTime, curr->data.processorId);
        curr = curr->next;
    }
    printf("\n");
}