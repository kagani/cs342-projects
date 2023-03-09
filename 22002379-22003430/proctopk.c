#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"

/*
 * Usage: proctopk <K> <outfile> <N> <infile1> .... <infileN>
 * N child processes will be created to process N input files.
 */

int main(int argc, char *argv[])
{

    // Add argument check here

    int K = atoi(argv[1]);
    char *outfile = argv[2];
    int N = atoi(argv[3]);
    char *files[N];

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
        printf("Line: %s\n", line);
        while (read != -1)
        {
            int l = 0;
            int r = 0;
            printf("here\n");

            for (int i = 0; i < len; i++) {
                printf("%d ", line[i]);
            }
            while (line[r] != '\n') {
                if (line[r] == '\0') {
                    int len = r - l;
                    char *str = (char *)malloc(sizeof(char) * (len + 1));

                    for (int i = 0; i < len; i++) {
                        str[i] = line[l++];
                    }

                    insert(head, str, len);
                    count++;
                }
                
                r++;
                while (line[l] == ' ' || line[l] == '\t') l++;
                while (line[r] == ' ' || line[r] == '\t') r++;
            }

            read = getline(&line, &len, ptr);
        }

        int size = 0;
        char **res = topKFrequent(head, K, &size);

        printf("\nTop %d words:\n", size);
        for (int i = 0; i < size; i++)
        {
            printf("%s\n", res[i]);
        }
        fclose(ptr);
        return 0;
    }

    wait(rc);

    return 0;
}
