#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "rm.h"
#include <stdbool.h>


// global variables

int DA;  // indicates if deadlocks will be avoided or not
int N;   // number of processes
int M;   // number of resource types
int ExistingRes[MAXR]; // Existing resources vector

//..... other definitions/variables .....
//.....
//.....

// free those after use!!!
int** maxDemand;
int** allocation;
int** need;
int* available;

pthread_mutex_t mutex;
pthread_cond_t* cvs;

// end of global variables
void printMat(int** mat, int n, int m) {
    for(int i = 0; i < n; i++) {
        printf("\n");
        for(int j = 0; j < m; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

// Return true if arr1 <= arr2, false otherwise
bool arrLessThan(int* arr1, int* arr2, int N) {
    for(int i = 0; i < N; i++) {
        if(arr1[i] > arr2[i]) {
            return false;
        }
    }
    return true;
}

// Check if the current state is safe
bool checkSafe(int* available, int** need, int** allocation) {
    int work[M];
    bool finish[N];
    for(int i = 0; i < M; i++)
        work[i] = available[i];
    for(int i = 0; i < N; i++)
        finish[i] = 0;

    while(1) {
        int x = -1;
        for(int i = 0; i < N; i++) {
            if(!finish[i] && arrLessThan(need[i], work, M)) {x=i; break;}
        }
        if(x==-1) break;
        else {
            for(int i = 0; i < M; i++) {
                work[i] += allocation[x][i];
                finish[x] = true;
            }
        }
    }
    for(int i = 0; i < N; i++) {
        if(!finish[i]) return false;
    }
    return true;
}

bool checkAvailability(int request[]) {
    for(int i = 0; i < M; i++) {
        if(request[i] > available[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief This function will be called by a thread immediately after it starts
 * execution. That means this function will be called at the beginning of a thread
 * start function. With this function, the thread will indicate to the library that it
 * has become alive and its identifier is tid. 
 * @param tid: The tid parameter value is determined by the main thread (i.e., the application) 
 * and passed to the thread, i.e., to the thread start function, by specifying it as the last argument to the
 * pthread_create() function. In this way each created thread will have a
 * unique integer id selected by the main thread (application) from the range [0,
 * N-1], where N is the number threads to create.
*/
int rm_thread_started(int tid)
{
    int ret = 0;
    return (ret);
}

/**
 * @brief This function will be called by a thread just before termination. With
 * this function, the thread indicates to the library that it is terminating. 
 * @return 0 upon success, -1 upon failure. 
*/
int rm_thread_ended()
{
    int ret = 0;
    return (ret);
}

/**
 * @brief If deadlock avoidance is desired (indicated with rm_init()), then a
 * thread will use this function to indicate to the library its maximum resource
 * demand. This function should be called just after calling
 * rm_thread_started(), before any request is issued. 
 * @param claim: The claim array is used to specify how many resource instances 
 * of each type a thread may need at most. The library populates a MaxDemand 
 * matrix with the claim information it receives from all threads. Deadlock 
 * avoidance algorithm, implemented inside the rm_request() function, will use the claim
 * information to avoid deadlocks. 
 * @return 0 if successful. It will return -1 in case of an error, such as trying to claim 
 * more instances than existing. 
*/
int rm_claim (int claim[])
{
    pthread_t tid = pthread_self();
    for(int i = 0; i < MAXR; i++) {
        if(claim[i] > ExistingRes[i])
            return -1;
        maxDemand[tid][i] = claim[i];
    }
    int ret = 0;
    return(ret);
}

/**
 * @brief This function will initialize the necessary structures and variables in
 * the library to do resource management. N is the number of threads and M is
 * the number of resource types. The parameter existing is an array of size M
 * @param p_count: Number of threads.
 * @param r_count: Number of resource types.
 * @param r_exist: Array of M (r_count) integers indicating the existing resource 
 * instance of each type initially all available.
 * @param avoid: Used to specify if the library will do deadlock avoidance or not
 * If avoid is 1, then deadlock avoidance will be used while allocating resources. 
 * If avoid is 0, then no deadlock avoidance will be applied, and therefore deadlocks 
 * may occur. 
 * @return 0 if initialization is successfully done, -1 in case there is an
 * error encountered; for example, when the value of N or M exceeds the 
 * maximum number of threads or resources types supported by the library; or 
 * if any specified value is negative, etc.
*/
int rm_init(int p_count, int r_count, int r_exist[], int avoid)
{
    if(p_count>MAXP | r_count>MAXR)
        return -1;
    
    int i;
    int ret = 0;
    
    DA = avoid;
    N = p_count;
    M = r_count;
    available = (int*)malloc(sizeof(int)*M);

    // initialize mutex lock
    pthread_mutex_init(&mutex, NULL);

    // initialize condition variables
    for(int i = 0; i < N; i++)
        pthread_cond_init(&cvs[i], NULL);
    
    // initialize (create) resources
    for (i = 0; i < M; ++i) {
        available[i] = r_exist[i];
        ExistingRes[i] = r_exist[i];
    }   

    // initialize maxDemand, allocation and need matrices
    maxDemand = (int**)malloc(sizeof(int*)*N);
    allocation = (int**)malloc(sizeof(int*)*N);
    need = (int**)malloc(sizeof(int*)*N);
    for(int i = 0; i < N; i++) {
        maxDemand[i] = (int*)malloc(sizeof(int)*M);
        allocation[i] = (int*)malloc(sizeof(int)*M);
        need[i] = (int*)malloc(sizeof(int)*M);
        for(int j = 0; j < M; j++) {
            maxDemand[i][j] = 0;
            allocation[i][j] = 0;
            need[i][j] = 0;
        }
    }
    return (ret);
}

/**
 * @brief This function is called by a thread to request resources from the library.
 * The function will allocate the requested resources if resources are available. If
 * deadlock avoidance is used, resources may not be allocated even when they
 * are available, because it may not be safe to do so. If the requested resources are 
 * available, then they will be allocated and the function will return without getting 
 * blocked. If the requested resources are not available, the thread will be blocked 
 * inside the function (by use of a condition variable), until the requested resources 
 * can be allocated, at which time resources will be allocated and the function will return.
 * If deadlock avoidance is used, resources will be allocated only if it is safe to do so. 
 * If the new state would not be safe, the requested resources are not allocated and the 
 * calling thread is blocked, even though there are resources available to satisfy the request.
 * @param request: An integer array of size M, and indicates the number of 
 * instances of each resource type that the calling thread is requesting.
 * @return 0 upon success (resources allocated). -1 if there is an error condition, for 
 * example, the number of requested instances for a resources type is greater than the 
 * number of existing instances. 
*/
int rm_request (int request[])
{   
    pthread_t tid = pthread_self();

    // ERROR IF REQUEST > MAX NEED
    for(int i = 0; i < M; i++) {
        if(request[i] > need[tid][i])
            return -1;
    }

    bool avail = true;

    pthread_mutex_lock(&mutex);

    // Wait if requested resources are not available
    while(!checkAvailability(request)) {
        pthread_cond_wait(&cvs[tid], &mutex);
    }

    // Check if the new state is safe
    while(1) {
        // Compute the new state
        int* newAvailable = (int*)malloc(sizeof(int)*M); 
        int** newAllocation = (int**)malloc(sizeof(int*)*N);
        int** newNeed = (int**)malloc(sizeof(int*)*N);
        for(int i = 0; i < M; i++) {
            newAvailable[i] = available[i] - request[i];
        }
        for(int i = 0; i < N; i++) {
            newAllocation[i] = (int*)malloc(sizeof(int)*M);
            newNeed[i] = (int*)malloc(sizeof(int)*M);
            for(int j = 0; j < M; j++) {
                if(i == tid) {
                    newAllocation[i][j] = allocation[i][j] + request[i];
                    newNeed[i][j] = need[i][j] - request[i];
                }
                else {
                    newAllocation[i][j] = allocation[i][j];
                    newNeed[i][j] = need[i][j];
                }
            }
        }
        if(checkSafe(newAvailable, newNeed, newAllocation)) {
            free(available);
            for(int i = 0; i < N; i++) { 
                free(allocation[i]);
                free(need[i]);
            } 
            available = newAvailable;
            need = newNeed;
            allocation = newAllocation;
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        else {
            pthread_cond_wait(&cvs[tid], &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

/**
 * @brief This function is called by a thread to release resources. 
 * The function will deallocate the indicated resource instances. It 
 * will also wake up the blocked threads (that were blocked at rm_request() 
 * function) so that each thread can check if it can continue now. Note that 
 * with a release call, it is possible that a thread may not release all its 
 * allocated resources, but only some of them. Therefore, in an application, 
 * the number of request calls and release calls do not have to be equal.
 * @param release: The number of instances of each resource type that 
 * the thread wants to release is indicated with the array release of 
 * size M.  
 * @return 0 upon success. -1 in case of an error condition, for example, 
 * when trying to release more instances than allocated.
*/
int rm_release (int release[])
{
    pthread_t tid = pthread_self();

    // Check error condition
    for(int i = 0; i < M; i++) {
        if(release[i] > allocation[tid][i])
            return -1;
    }

    // Set the new matrices & vectors
    int* newAvailable = (int*)malloc(sizeof(int)*M); 
    int** newAllocation = (int**)malloc(sizeof(int*)*N);
    int** newNeed = (int**)malloc(sizeof(int*)*N);

    pthread_mutex_lock(&mutex);
    for(int i = 0; i < M; i++) {
        newAvailable[i] = available[i] + release[i];
    }
    for(int i = 0; i < N; i++) {
        newAllocation[i] = (int*)malloc(sizeof(int)*M);
        newNeed[i] = (int*)malloc(sizeof(int)*M);
        for(int j = 0; j < M; j++) {
            if(i == tid) {
                newAllocation[i][j] = allocation[i][j] - release[i];
                newNeed[i][j] = need[i][j] + release[i];
            } else {
                newAllocation[i][j] = allocation[i][j];
                newNeed[i][j] = need[i][j];
            }
        }
    }
    free(available);
    for(int i = 0; i < N; i++) { 
        free(allocation[i]);
        free(need[i]);
    } 
    available = newAvailable;
    need = newNeed;
    allocation = newAllocation;
    pthread_mutex_unlock(&mutex);
    return 0;
}

/**
 * @brief This function will check if there are deadlocked processes at the moment. 
 * @return The function will return the number of deadlocked processes, if any. If there 
 * is no deadlocked process, 0 will be returned. In case of any error, -1 will be returned. 
*/
int rm_detection()
{
    int ret = 0;
    
    return (ret);
}

/**
 * @brief When called, this function will print out information about the current
 * state in the library. The information will include the content of the Existing
 * vector, Available vector, Allocation matrix, Request matrix, MaxDemand
 * matrix, and the Need matrix. If deadlock avoidance is not specified, then
 * MaxDemand matrix and Need matrix will have all zeros. An application that
 * is using the library may call this function whenever it wants to learn about the
 * state. This function will be the only function in your library (in the submitted
 * version) that will print out something to the screen. The headermsg
 * parameter is a string that will be printed out at the beginning of the state
 * information.
*/
void rm_print_state (char hmsg[])
{
    return;
}