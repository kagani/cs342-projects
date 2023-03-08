#include "topk.h"

// Insert with dummy head
void insert(node *head, char *word)
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

char **topKFrequent(char **words, int wordsSize, int k, int *returnSize)
{
    if (wordsSize < 1)
    {
        return NULL;
    }

    // Dummy head
    node *head = (node *)malloc(sizeof(node));
    head->word = "";
    head->freq = INT_MAX;
    head->next = NULL;
    head->prev = NULL;
    for (int i = 0; i < wordsSize; i++)
    {
        insert(head, words[i]);
    }
    node *cur = head->next;
    sort(head);

    char **res = (char **)malloc(sizeof(char *) * k);
    cur = head->next;

    cur = head->next;
    for (int i = 0; i < k; i++)
    {
        int len = strlen(cur->word);
        res[i] = (char *)malloc((len + 1) * sizeof(char));
        strcpy(res[i], cur->word);
        cur = cur->next;
    }

    *returnSize = k;
    return res;
}
