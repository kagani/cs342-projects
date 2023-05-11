#include <pthread.h>
#include <stdio.h>

void* thread_func(void* arg) {
    pthread_t tid = pthread_self();
    printf("Thread ID is %lu\n", tid);
    return NULL;
}
int arrLessThan(int *arr1, int *arr2, int N)
{
    for (int i = 0; i < N; i++)
    {
        if (arr1[i] > arr2[i])
        {
            return 0;
        }
    }
    return 1;
}

int checkAllZero(int* arr, int N) {
    for(int i = 0; i < N; i++) {
        if(arr[i] != 0)
            return 0;
    }
    return 1;
}

int main() {
    int arr1[5] = {0,0,0,1,0};
    int arr2[5] = {3, 5, 6, 7, 8};
    printf("res: %d", checkAllZero(arr1, 5));
    return 0;
}
