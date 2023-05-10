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
int main() {
    int arr1[5] = {4, 5, 6, 7, 8};
    int arr2[5] = {3, 5, 6, 7, 8};
    printf("res: %d", arrLessThan(arr1, arr2, 5));
    return 0;
}
