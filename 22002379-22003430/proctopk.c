#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"
#include <sys/mman.h>

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
    void* SHARED_MEM = create_shared_memory(4096);

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
        char **res = topKFrequent(head, K, &size);
        memcpy(SHARED_MEM, res, sizeof(res));

        printf("\nTop %d words:\n", size);
        for (int i = 0; i < size; i++)
        {
            printf("%s\n", res[i]);
        }
        fclose(ptr);
        return 0;
    }

    for (int i = 0; i < N; i++) {
        wait(processes[i]);
    }

    return 0;
}
