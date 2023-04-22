#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct Node {   
    int data;
    struct Node* next;
};

struct LinkedList {
    struct Node* head;
};

void insertNode(struct LinkedList* list, int value);
void removeNode(struct LinkedList* list, int value);
void printList(struct LinkedList* list);

#endif