#include <stdio.h>
#include <limits.h>

struct node
{
    char *word;
    int freq;
    struct node *next;
    struct node *prev;
};

// Insert with dummy head
void insert(struct node *head, char *word)
{
    struct node *cur = head;

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

    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->word = word;
    newNode->freq = 1;
    newNode->prev = cur;
    newNode->next = cur->next;
    cur->next = newNode;
    return;
}

void swap(struct node *a, struct node *b)
{
    char *aWord = a->word;
    int aFreq = a->freq;
    a->freq = b->freq;
    a->word = b->word;
    b->freq = aFreq;
    b->word = aWord;
}

void sort(struct node *head)
{
    struct node *cur = NULL;
    struct node *idx = NULL;

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
    struct node *head = (struct node *)malloc(sizeof(struct node));
    head->word = "";
    head->freq = INT_MAX;
    head->next = NULL;
    head->prev = NULL;
    for (int i = 0; i < wordsSize; i++)
    {
        insert(head, words[i]);
    }
    struct node *cur = head->next;
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

/*
bool comp(pair<string, int>& a, pair<string, int>& b) {
        if (a.second == b.second) {
            return a.first < b.first;
        }

        return a.second > b.second;
    }

class Solution {
public:

    vector<string> topKFrequent(vector<string>& words, int k) {
        map<string, int> freq;

        for (string s : words) {
            if (freq.count(s) == 0) {
                freq[s] = 1;
                continue;
            }

            freq[s]++;
        }

        vector<pair<string, int>> tmp;

        for (auto &i : freq) {
            tmp.push_back(i);
        }
        sort(tmp.begin(), tmp.end(), comp);

        vector<string> res;

        for (int i = 0; i < k; i++) {
            res.push_back(tmp[i].first);
        }

        return res;
    }
};
*/