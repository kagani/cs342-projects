#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"
#include <pthread.h>
#include <ctype.h>

/*
 * Usage: threadtopk <K> <outfile> <N> <infile1> .... <infileN>
 * N threads will be created to process N input files.
 */

void uppercase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (islower(str[i]))
        {
            str[i] = toupper(str[i]);
        }
    }
}

char **files;     // file names global array files[i] is ith thread
pair **results;   // 2d array to gather results from multiple threads res[i] is the res of ith thread
int *resultsSize; // Size of each result from each thread

void *worker(int *arg)
{
    int workerIdx = arg[0];
    int K = arg[1];
    char *fileName = files[workerIdx];
    FILE *ptr;

    ptr = fopen(fileName, "r");
    if (!ptr)
    {
        printf("[-] File does not exist.\n");
        return (int *)-1;
    }

    char *line = NULL;
    int len = 0;
    int read;
    int count = 0;
    read = getline(&line, (size_t *)&len, ptr);

    long lgth;
    char *text;
    char *tkn;
    const char exceptions[] = " \t\r\n\v\f";
    if (!ptr)
    {
        printf("[-] File does not exist.\n");
    }
    fseek(ptr, 0, SEEK_END);
    node *head = (node *)malloc(sizeof(node)); // Dummy head
    head->freq = INT_MAX;
    head->word = "";
    lgth = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    text = (char *)malloc(lgth + 1);
    fread(text, 1, lgth, ptr);
    text[lgth] = '\0';

    tkn = strtok(text, exceptions);
    while (tkn != NULL)
    {
        int len = strlen(tkn);
        char *word = (char *)malloc(sizeof(char) * (len + 1));
        strcpy(word, tkn);
        uppercase(word);
        insert(head, word, len, 1);
        tkn = strtok(NULL, exceptions);
    }

    int size = 0;

    pair *res = topKFrequent(head, K, &size);
    results[workerIdx] = res;
    resultsSize[workerIdx] = size;
    fclose(ptr);
    return 0;
}

int main(int argc, char *argv[])
{
    int K = atoi(argv[1]);
    char *outfile = argv[2];
    int N = atoi(argv[3]);

    files = (char **)malloc(sizeof(char *) * N);
    results = (pair **)malloc(sizeof(pair *) * N);
    resultsSize = (int *)malloc(sizeof(int) * N);

    for (int i = 0; i < N; i++)
    {
        int len = strlen(argv[i + 4]);
        files[i] = (char *)malloc((len + 1) * sizeof(char));
        strcpy(files[i], argv[i + 4]);
    }

    pthread_t threads[N];
    int i;
    for (i = 0; i < N; i++)
    {
        int *args = (int *)malloc(sizeof(int) * 2);
        args[0] = i;
        args[1] = K;
        pthread_create(&threads[i], NULL, (void *)worker, args);
        pthread_join(threads[i], NULL);
    }

    // Process top k again
    node *head = (node *)malloc(sizeof(node)); // Dummy head
    head->word = "";
    head->freq = INT_MAX;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < resultsSize[i]; j++)
        {
            insert(head, results[i][j].first, strlen(results[i][j].first), results[i][j].second);
        }
    }

    int size = 0;
    pair *res = topKFrequent(head, K, &size);

    FILE *fptr;
    fptr = fopen(outfile, "w");
    for (int i = 0; i < size; i++)
    {
        fprintf(fptr, "%s %d\n", res[i].first, res[i].second);
    }
    return 0;
}
