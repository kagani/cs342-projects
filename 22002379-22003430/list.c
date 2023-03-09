#include "list.h"

// Insert with dummy head
void insert(node *head, char *word, int len)
{
    node *cur = head;
    while (cur->next && strcmp(cur->next->word, word) != 0)
    {
        cur = cur->next;
    }

    if (cur->next && strcmp(cur->next->word, word) == 0)
    {
        cur->next->freq++;
        return;
    }

    if (strcmp(cur->word, word) == 0)
    {

        cur->freq++;
        return;
    }

    node *newNode = (node *)malloc(sizeof(node));
    newNode->word = word;
    newNode->freq = 1;
    newNode->len = len;
    newNode->prev = cur;
    newNode->next = cur->next;
    cur->next = newNode;
    return;
}

void swap(node *a, node *b)
{
    char *aWord = a->word;
    int aFreq = a->freq;
    a->freq = b->freq;
    a->word = b->word;
    b->freq = aFreq;
    b->word = aWord;
}

void sort(node *head)
{
    node *cur = NULL;
    node *idx = NULL;

    for (cur = head->next; cur->next; cur = cur->next)
    {
        for (idx = cur->next; idx; idx = idx->next)
        {
            if (cur->freq < idx->freq)
            {
                swap(cur, idx);
            }
            else if (cur->freq == idx->freq && strcmp(cur->word, idx->word) > 0)
            {
                swap(cur, idx);
            }
        }
    }
}
