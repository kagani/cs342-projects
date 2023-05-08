#include <pthread.h>
#include <stdio.h>

void* thread_func(void* arg) {
    pthread_t tid = pthread_self();
    printf("Thread ID is %lu\n", tid);
    return NULL;
}

int main() {
    pthread_t tid = 0;
    pthread_create(&tid, NULL, thread_func, NULL);
    pthread_join(tid, NULL);
    printf("aaaThread ID is %lu\n", tid);
    return 0;
}
