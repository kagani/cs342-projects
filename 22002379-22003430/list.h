#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node
{
    char *word;
    int freq;
    struct node *next;
    struct node *prev;
} node;

void insert(node *head, char *word);
void swap(node *a, node *b);
void sort(node *head);
