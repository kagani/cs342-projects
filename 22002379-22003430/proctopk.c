#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"
#include <sys/mman.h>

#define MAX_STR 64

/*
 * Usage: proctopk <K> <outfile> <N> <infile1> .... <infileN>
 * N child processes will be created to process N input files.
 */

void* create_shared_memory(size_t size) {
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANONYMOUS;
  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(int argc, char *argv[])
{

    // Add argument check here

    int K = atoi(argv[1]);
    char *outfile = argv[2];
    int N = atoi(argv[3]);

    // The names of input files stored in a global variable
    char *files[N];

    // The name of the shared memory stored in a global variable
    void* mptr = create_shared_memory(K * N * (MAX_STR*sizeof(char)+sizeof(int)) * (sizeof(char) * 64) + (N * sizeof(int)));
    int index = 0;
    memcpy(mptr, &index, sizeof(int));

    for (int i = 0; i < N; i++)
    {
        int len = strlen(argv[i + 4]);
        files[i] = (char *)malloc((len + 1) * sizeof(char));
        strcpy(files[i], argv[i + 4]);
    }

    int processes[N];
    int rc = -1;
    int i;
    for (i = 0; i < N; i++)
    {
        rc = fork();

        if (rc == 0)
            break;
        processes[i] = rc;
    }

    if (rc == 0)
    {
        FILE *ptr;

        ptr = fopen(files[i], "r");
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
        size_t szt = 64;
        pair *res = topKFrequent(head, K, &size);
        void* cur = mptr + sizeof(int) + *((int*)mptr) + 1;
        for(int i = 0; i < size; i++) {
            
            memcpy(cur, res[i].first, szt);
            *((int*)mptr) += szt;

            memcpy(cur+(MAX_STR*sizeof(char)), &res[i].second, sizeof(int));
            *((int*)mptr) += sizeof(int);

            cur += MAX_STR*sizeof(char) + sizeof(int);
        }

        printf("\nTop %d words:\n", size);
        for (int i = 0; i < size; i++)
        {
            printf("%s %d\n", res[i].first, res[i].second);
        }
        fclose(ptr);
        return 0;
    }


    for (int i = 0; i < N; i++) {
        wait(processes[i]);
    }

    printf("\n=== FROM SHARED MEMORY ===\n\n");

    void* curmem = mptr + sizeof(int) + 1;
    size_t sz = MAX_STR*sizeof(char);

    node *head = (node *)malloc(sizeof(node)); // Dummy head
    head->freq = INT_MAX;
    head->word = "";

    for(int i = 0; i < N*K; i++) {
        char* word = curmem;
        int freq = *((int*)(curmem+64));

        node* cur = (node*)malloc(sizeof(node));

        insert(head, word, freq);

        printf("%d) %s : %d\n", i, word, freq);
        curmem += sz+sizeof(int);
    }
    
    int size = 0;
    pair *res = topKFrequent(head, K, &size);
    printf("\nTop %d words:\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%s %d\n", res[i].first, res[i].second);
    }

    return 0;
}
