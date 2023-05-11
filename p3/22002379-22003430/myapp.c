#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rm.h"

#define NUMR 6  // number of resource types
#define NUMP 5  // number of threads

int exist[6] = {1, 1, 1, 1, 1, 1};  // resources existing in the system

int AVOID = 1;

void pr(int tid, char astr[], int m, int r[]) {
    int i;
    printf("thread %d, %s, [", tid, astr);
    fflush(0);
    for (i = 0; i < m; ++i) {
        if (i == (m - 1)) {
            printf("%d", r[i]);
            fflush(0);
        } else {
            printf("%d,", r[i]);
            fflush(0);
        }
    }
    printf("]\n");
    fflush(0);
}

void setarray(int r[MAXR], int m, ...) {
    va_list valist;
    int i;

    va_start(valist, m);
    for (i = 0; i < m; i++) {
        r[i] = va_arg(valist, int);
    }
    va_end(valist);
    return;
}

void *worker(void *a) {
    int tid;
    int request1[MAXR];
    int request2[MAXR];
    int claim[MAXR];

    tid = *((int *)a);
    rm_thread_started(tid);

    setarray(claim, NUMR, 0, 0, 0, 0, 0, 0);  // init to 0

    for (int i = 0; i < NUMP - 1; i++) {  // claim own resource
        if (i == tid) {
            claim[i] = 1;
            break;
        }
    }

    if (tid == NUMP - 1) {  // claim all resources for last thread
        setarray(claim, NUMR, 1, 1, 1, 1, 1, 1);
    }

    rm_claim(claim);

    pr(tid, "REQ", NUMR, claim);
    fflush(0);
    rm_request(claim);

    sleep(4);

    rm_release(claim);

    rm_thread_ended();
    printf("thread %d ended\n", tid);
    pthread_exit(NULL);
}

/**
 * @brief Deadlock condition demonstration
 *
 */
void deadlock() {
    int tids[NUMP];
    pthread_t threadArray[NUMP];

    rm_init(NUMP, NUMR, exist, 0);

    for (int i = 0; i < NUMP; i++) {
        tids[i] = i;
        pthread_create(&threadArray[i], NULL, worker, (void *)&tids[i]);
    }

    for (int i = 0; i < 10; i++) {
        sleep(1);
        rm_print_state("The current state");
        int ret = rm_detection();
        if (ret > 0) {
            printf("deadlock detected, count=%d\n", ret);
            rm_print_state("state after deadlock");
        }
    }

    for (int i = 0; i < NUMP; i++) {
        pthread_join(threadArray[i], NULL);
    }
}

/**
 * @brief Deadlock avoidance demonstration
 *
 */
void deadlock_avoid() {
    int tids[NUMP];
    pthread_t threadArray[NUMP];

    rm_init(NUMP, NUMR, exist, 1);

    for (int i = 0; i < NUMP; i++) {
        tids[i] = i;
        pthread_create(&threadArray[i], NULL, worker, (void *)&tids[i]);
    }

    for (int i = 0; i < 10; i++) {
        sleep(1);
        rm_print_state("The current state");
        int ret = rm_detection();
        if (ret > 0) {
            printf("deadlock detected, count=%d\n", ret);  // Should not happen
            rm_print_state("state after deadlock");
        }
    }

    for (int i = 0; i < NUMP; i++) {
        pthread_join(threadArray[i], NULL);
        printf("thread %d joined\n", i);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: ./app avoidflag\n");
        exit(1);
    }

    AVOID = atoi(argv[1]);
    if (AVOID == 1)
        deadlock_avoid();
    else
        deadlock();
    return 0;
}