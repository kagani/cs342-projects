#include "mem_utils.h"
#include <string.h>

int main(int argc, char *argv[])
{
    char* cur = argv[1];
    if(strcmp(cur, "-freefc")==0) {
        long long PFN1, PFN2;
        PFN1 = atoll(argv[2]);
        PFN2 = atoll(argv[3]);
    }
    else if(strcmp(cur, "-frameinfo")==0) {
        long long PFN;
        PFN = atoll(argv[2]);
    }
    else if(strcmp(cur, "-memused")==0) {
        long long PID;
        PID = atoll(argv[2]);
    }
    else if(strcmp(cur, " -mapva")==0) {
        long long PID, VA;
        PID = atoll(argv[2]);
        VA = atoll(argv[3]);
    }
    else if(strcmp(cur, "-pte")==0) {
        long long PID, VA;
        PID = atoll(argv[2]);
        VA = atoll(argv[3]);
    }
    else if(strcmp(cur, "-maprange")==0) {
        long long PID, VA1, VA2;
        PID = atoll(argv[2]);
        VA = atoll(argv[3]);
    }
    else if(strcmp(cur, "-mapall")==0) {
        long long PID;
        PID = atoll(argv[2]);
    }
    else if(strcmp(cur, "-mapallin")==0) {
        long long PID;
        PID = atoll(argv[2]);
    }
    else if(strcmp(cur, "-alltablesize")==0) {
        long long PID;
        PID = atoll(argv[2]);
    }
    return 0;
}
