#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"
#include <pthread.h>

/*
 * Usage: threadtopk <K> <outfile> <N> <infile1> .... <infileN>
 * N threads will be created to process N input files.
 */

char **files; // file names global array files[i] is ith thread
pair **res; // 2d array to gather results from multiple threads res[i] is the res of ith thread

void *worker(void *arg) {
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
            if (l < r && line[r] == ' ' || line[r] == '\t' || line[r] == '\0') {
                int strLen = r - l;
                char *str = (char *)malloc(sizeof(char) * (strLen + 1));

                for (int i = 0; i < strLen; i++) {
                    str[i] = line[l++];

                    if ('a' <= str[i] && str[i] <= 'z') str[i] -= 32;
                }

                str[strLen] = '\0';

                insert(head, str, strLen);
                count++;
                l = r;
                while (line[l] == ' ' || line[l] == '\t') l++;
            }
            
            r++;
        }
        
        read = getline(&line, &len, ptr);
    }

    int size = 0;
    
    pair *res = topKFrequent(head, K, &size);

    printf("\nTop %d words:\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%s %d\n", res[i].first, res[i].second);
    }
    fclose(ptr);
    return 0;
}

int main(int argc, char *argv[])
{

    // Add argument check here

    int K = atoi(argv[1]);
    char *outfile = argv[2];
    int N = atoi(argv[3]);

    files = (char *) malloc(sizeof(char*) * N);

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
        void *arg = {i, K};
        pthread_create(&threads[i], NULL, worker, arg);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
