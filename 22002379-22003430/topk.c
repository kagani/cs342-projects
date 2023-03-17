#include "topk.h"

pair *topKFrequent(node *words, int k, int *returnSize)
{
    if (!words)
    {
        return NULL;
    }

    sort(words);

    pair *res = (pair *)malloc(sizeof(pair *) * k);
    node *cur = words->next;

    cur = words->next;
    int i = 0;
    for (; i < k && cur; i++)
    {
        res[i].first = (char *)malloc((cur->len + 1) * sizeof(char));
        strcpy(res[i].first, cur->word);
        res[i].second = cur->freq;
        cur = cur->next;
    }

    *returnSize = i;
    return res;
}
