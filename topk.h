#include <stdio.h>
#include <limits.h>

struct node
{
    char *word;
    int freq;
    struct node *next;
    struct node *prev;
};

void insert(struct node *head, char *word);
void swap(struct node *a, struct node *b);
void sort(struct node *head);
char **topKFrequent(char **words, int wordsSize, int k, int *returnSize);
