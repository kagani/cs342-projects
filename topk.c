#include <stdio.h>

struct node
{
    char *word;
    int freq;
    struct node *next;
    struct node *prev;
};

char **topKFrequent(char **words, int wordsSize, int k, int *returnSize)
{
    char **res;
    if (wordsSize < 1)
    {
        return res;
    }

    struct node *head = (struct node *)malloc(sizeof(struct node));
    head->word = words[0];
    head->freq = 1;
    head->next = NULL;
    head->prev = NULL;

    struct node *cur = head;

    // Doubly linked list as a bad hash map
    for (int i = 1; i < wordsSize; i++)
    {
        while (cur->next && strcmp(cur->word, words[i]) < 0)
        {
            cur = cur->next;
        }

        while (cur->prev && strcmp(cur->word, words[i]) > 0)
        {
            cur = cur->prev;
        }

        if (strcmp(cur->word, words[i]) == 0)
        {
            cur->freq++;
            continue;
        }

        struct node *newNode = (struct node *)malloc(sizeof(struct node));
        newNode->freq = 1;

        // Inserting head
        if (!cur->prev && strcmp(cur->word, words[i]) < 0)
        {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
            continue;
        }

        newNode->next = cur->next;
        newNode->prev = cur;
        cur->next = newNode;
    }

    // Bubble sort the list;
    struct node *cur = head;

    while (cur->next)
    {
        struct node *curJ = cur;

        while (curJ->next)
        {
            if (cur->freq > cur->next->freq)
            {
                cur = cur->next;
                continue;
            }

            // Swap cur with next;
        }
    }
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