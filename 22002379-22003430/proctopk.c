#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "topk.h"
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include<sys/wait.h>
#include<ctype.h>


#define MAX_STR 64

/*
 * Usage: proctopk <K> <outfile> <N> <infile1> .... <infileN>
 * N child processes will be created to process N input files.
 */

void* shmem_alloc(size_t size, int shm_fd) {
    ftruncate(shm_fd, size);
    return mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
}

void shmem_dealloc(void* shptr, int shm_fd, size_t size) {
    munmap(shptr, size);
    close(shm_fd);
    shm_unlink("/shm.dir");
}

char* uppercase(char* str) {
    char* temp = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(temp, str);
    char cur;
    int i = 0;
    while(temp[i]) {
        cur = temp[i];
        putchar(toupper(cur));
        i++;
    }
    return temp;
}

int main(int argc, char *argv[])
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    // Add argument check here

    int K = atoi(argv[1]);
    char *outfile = argv[2];
    int N = atoi(argv[3]);

    // The names of input files stored in a global variable
    char *files[N];

    // The name of the shared memory stored in a global variable
    int shm_fd = shm_open("/shm.dir", O_CREAT | O_RDWR, 0666);
    if(!shm_fd) {
        printf("[-] Error occured during shared memory creation.");
    }
    size_t shmem_size = K * N * (MAX_STR*sizeof(char)+sizeof(int)) * (sizeof(char) * 64) + (N * sizeof(int));
    void* mptr = shmem_alloc(shmem_size, shm_fd);
    int index = 0;
    memcpy(mptr, &index, sizeof(int));

    for (int i = 0; i < N; i++)
    {
        int len = strlen(argv[i + 4]);
        files[i] = (char *)malloc((len + 1) * sizeof(char));
        strcpy(files[i], argv[i + 4]);
    }

    int* processes[N];
    intptr_t rc = -1;
    int i;
    for (i = 0; i < N; i++)
    {
        rc = fork();

        if (rc == 0)
            break;
        processes[i] = (int*) rc;
    }

    if (rc == 0)
    {
        FILE *ptr;
        long length;
        char *data;
        char *token;
        const char delimiters[] = " \t\r\n\v\f";
        ptr = fopen(files[i], "r");
        if (!ptr)
        {
            printf("[-] File does not exist.\n");
            return -1;
        }

        node *head = (node *)malloc(sizeof(node)); // Dummy head
        head->freq = INT_MAX;
        head->word = "";

        fseek(ptr, 0, SEEK_END);
        length = ftell(ptr);
        fseek(ptr, 0, SEEK_SET);

        data = (char *)malloc(length + 1);

        fread(data, 1, length, ptr);
        data[length] = '\0';

        fclose(ptr);

        token = strtok(data, delimiters);
        while (token != NULL) {
            insert(head, token, strlen(token), 1);
            token = strtok(NULL, delimiters);
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

        insert(head, word, strlen(word) ,freq);

        printf("%d) %s : %d\n", i, word, freq);
        curmem += sz+sizeof(int);
    }

    FILE *fptr;
    fptr = fopen(outfile,"w");
    int size = 0;
    pair *res = topKFrequent(head, K, &size);
    printf("\nTop %d words:\n", size);
    for (int i = 0; i < size; i++)
    {
        fprintf(fptr,"%s %d\n", res[i].first, res[i].second);
        printf("%s %d\n", res[i].first, res[i].second);
    }

    long start = (tv.tv_sec) * 1000000 + tv.tv_usec;
    gettimeofday(&tv, 0);
    long end = (tv.tv_sec) * 1000000 + tv.tv_usec;

    printf("\n\nTime to complete: %ldμs\n", end - start);

    shmem_dealloc(mptr, shm_fd, shmem_size);

    return 0;
}
