#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"
#include <pthread.h>
#include <sys/time.h>

/*
 * Usage: threadtopk <K> <outfile> <N> <infile1> .... <infileN>
 * N threads will be created to process N input files.
 */

char **files; // file names global array files[i] is ith thread
pair **results; // 2d array to gather results from multiple threads res[i] is the res of ith thread
int *resultsSize; // Size of each result from each thread

void *worker(int *arg) {
    int workerIdx = arg[0];
    int K = arg[1];
    char* fileName = files[workerIdx];
    FILE *ptr;

    ptr = fopen(fileName, "r");
    if (!ptr)
    {
        printf("[-] File does not exist.\n");
        return -1;
    }

    node *head = (node *)malloc(sizeof(node)); // Dummy head
    head->freq = INT_MAX;
    head->word = "";

    char *line = NULL;
    int len = 0;
    int read;
    int count = 0;
    read = getline(&line, &len, ptr);
    while (read != -1)
    {
        int l = 0;
        int r = 0;
        while (r <= read) {
            if (l < r && line[r] == ' ' || line[r] == '\t' || line[r] == '\0' || line[r] == '\r' || line[r] == '\n') {
                int strLen = r - l;
                char *str = (char *)malloc(sizeof(char) * (strLen + 1));

                for (int i = 0; i < strLen; i++) {
                    str[i] = line[l++];

                    if ('a' <= str[i] && str[i] <= 'z') str[i] -= 32;
                }

                str[strLen] = '\0';

                insert(head, str, strLen, 1);
                count++;
                l = r;
                while (line[l] == ' ' || line[l] == '\t' || line[l] == '\r' || line[l] == '\n') l++;
                while (line[r] == '\r' || line[r] == '\n') r++;
            }
            
            r++;
        }
        
        read = getline(&line, &len, ptr);
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

    // Add argument check here
    struct timeval tv;
    gettimeofday(&tv, 0);

    int K = atoi(argv[1]);
    char *outfile = argv[2];
    int N = atoi(argv[3]);

    files = (char **) malloc(sizeof(char*) * N);
    results = (pair**) malloc(sizeof(pair*) * N);
    resultsSize = (int *) malloc(sizeof(int) * N);

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
        int *args = (int*) malloc(sizeof(int) * 2);
        args[0] = i;
        args[1] = K;
        pthread_create(&threads[i], NULL, worker, args);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    // Process top k again
    node *head = (node *)malloc(sizeof(node)); // Dummy head
    head->word = "";
    head->freq = INT_MAX;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < resultsSize[i]; j++) {
            insert(head, results[i][j].first, strlen(results[i][j].first), results[i][j].second);
        }
    }

    int size = 0;
    pair *res = topKFrequent(head, K, &size);

    FILE *fptr;
    fptr = fopen(outfile,"w");
    printf("\nTop %d words:\n", size);
    for (int i = 0; i < size; i++)
    {
        fprintf(fptr,"%s %d\n", res[i].first, res[i].second);
        printf("%s %d\n", res[i].first, res[i].second);
    }

    long start = (tv.tv_sec) * 1000000 + tv.tv_usec;
    gettimeofday(&tv, 0);
    long end = (tv.tv_sec) * 1000000 + tv.tv_usec;

    printf("Time to insert: %ldμs\n", end - start);

    return 0;
}
