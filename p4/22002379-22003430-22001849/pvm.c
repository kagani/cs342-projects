#include <string.h>

#include "mem_utils.h"

int main(int argc, char* argv[]) {
    char* cur = argv[1];
    if (strcmp(cur, "-freefc") == 0) {
        unsigned long pfnBegin, pfnEnd;
        pfnBegin = atoll(argv[2]);
        pfnEnd = atoll(argv[3]);
        free_fc(pfnBegin, pfnEnd);
    } else if (strcmp(cur, "-frameinfo") == 0) {
        unsigned long pfn;
        pfn = atoll(argv[2]);
        frame_info(pfn);
    } else if (strcmp(cur, "-memused") == 0) {
        int pid;
        pid = atoi(argv[2]);
        mem_used(pid);
    } else if (strcmp(cur, "-mapva") == 0) {
        int pid;
        unsigned long VA;
        pid = atoi(argv[2]);
        VA = atoll(argv[3]);
        map_va(pid, VA);
    } else if (strcmp(cur, "-pte") == 0) {
        int pid;
        unsigned long VA;
        pid = atoi(argv[2]);
        VA = atoll(argv[3]);
        pte(pid, VA);
    } else if (strcmp(cur, "-maprange") == 0) {
        int pid;
        unsigned long VA1, VA2;
        pid = atoi(argv[2]);
        VA1 = atoll(argv[3]);
        VA2 = atoll(argv[4]);
        map_range(pid, VA1, VA2);
    } else if (strcmp(cur, "-mapall") == 0) {
        int pid;
        pid = atoi(argv[2]);
    } else if (strcmp(cur, "-mapallin") == 0) {
        int pid;
        pid = atoi(argv[2]);
    } else if (strcmp(cur, "-alltablesize") == 0) {
        int pid;
        pid = atoi(argv[2]);
    }

    return 0;
}
