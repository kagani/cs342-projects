#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

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
    char files[N][64];

    for (int i = 0; i < N; i++)
    {
        strcpy(files[i], argv[i + 4]);
    }

    FILE *ptr;
    ptr = fopen("test.txt", "r");
    if (NULL == ptr)
    {
        printf("[-] File does not exist. \n");
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    while ((read = getline(&line, &len, ptr)) != -1)
    {
        char *token = strtok(line, "\n\r ");
        while (token != NULL)
        {
            printf("%s", token);
            token = strtok(NULL, "\n\r ");
            i++;
        }
    }

    fclose(ptr);
    return 0;
}
