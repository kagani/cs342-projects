#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "rm.h"


// global variables

int DA;  // indicates if deadlocks will be avoided or not
int N;   // number of processes
int M;   // number of resource types
int ExistingRes[MAXR]; // Existing resources vector

//..... other definitions/variables .....
//.....
//.....

// end of global variables

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
    int ret = 0;
    return(ret);
}

/**
 * @brief This function will initialize the necessary structures and variables in
 * the library to do resource management.  is the number of threads and M is
 * the number of resource types. The parameter existing is an array of M
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
int rm_init(int p_count, int r_count, int r_exist[],  int avoid)
{
    int i;
    int ret = 0;
    
    DA = avoid;
    N = p_count;
    M = r_count;
    // initialize (create) resources
    for (i = 0; i < M; ++i)
        ExistingRes[i] = r_exist[i];
    // resources initialized (created)
    
    //....
    // ...
    return  (ret);
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
    int ret = 0;
    
    return(ret);
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
    int ret = 0;

    return (ret);
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
