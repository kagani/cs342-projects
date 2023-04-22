#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

void insertNode(struct LinkedList* list, int value) {
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    newNode->data = value;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
    } else {
        newNode->next = list->head;
        list->head = newNode;
    }
}

void removeNode(struct LinkedList* list, int value) {
    struct Node* prev = NULL;
    struct Node* curr = list->head;

    while (curr != NULL && curr->data != value) {
        prev = curr;
        curr = curr->next;
    }

    if (curr != NULL) {
        if (prev != NULL) {
            prev->next = curr->next;
        } else {
            list->head = curr->next;
        }
        free(curr);
    }
}

void printList(struct LinkedList* list) {
    struct Node* curr = list->head;
    while (curr != NULL) {
        printf("%d ", curr->data);
        curr = curr->next;
    }
    printf("\n");
}