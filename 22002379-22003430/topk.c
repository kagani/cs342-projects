#include "topk.h"

char **topKFrequent(node *words, int k, int *returnSize)
{
    if (!words)
    {
        return NULL;
    }

    sort(words);

    char **res = (char **)malloc(sizeof(char *) * k);
    node *cur = words->next;

    cur = words->next;
    int i = 0;
    for (; i < k && cur; i++)
    {
        int len = strlen(cur->word);
        res[i] = (char *)malloc((len + 1) * sizeof(char));
        strcpy(res[i], cur->word);
        cur = cur->next;
    }

    *returnSize = i;
    return res;
}
