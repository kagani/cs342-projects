#include <string.h>

#include "mem_utils.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "--help") == 0) {
        printf(
            "The program has the following options. Use one option at a "
            "time.\n\n");
        printf("   -mapva PID VA\n");
        printf(
            "       print frame number for virtual address VA of process "
            "PID\n\n");
        printf("   -frameinfo PFN\n");
        printf("       print frame flags for physical frame PFN\n\n");
        printf("   -pte PID VA\n");
        printf(
            "       print pagemap flags for the page corresponding to virtual "
            "address VA of process PID\n\n");
        printf("   -maprange PID VA1 VA2\n");
        printf(
            "       print (page number, frame number) mappings for the pages "
            "in range [VA1, VA2) for process PID\n\n");
        printf("   -mapall PID\n");
        printf(
            "       print (page number, frame number) mappings for all pages "
            "of process PID\n\n");
        printf("   -mapallin PID\n");
        printf(
            "       print (page number, frame number) mappings for all "
            "in-memory pages of process PID\n\n");
        printf("   -memused PID\n");
        printf(
            "       calculate the used virtual memory and used physical memory "
            "of process PID\n\n");
        printf("   -alltablesize PID\n");
        printf("       print total page table size for process PID\n");
        return 0;
    }

    char* arg = argv[1];
    if (strcmp(arg, "-freefc") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: sudo ./pvm -freefc <pfnBegin> <pfnEnd>\n");
            return -1;
        }

        unsigned long pfnBegin = strtoul(argv[2], 0, 0);
        unsigned long pfnEnd = strtoul(argv[3], 0, 0);
        free_fc(pfnBegin, pfnEnd);
    } else if (strcmp(arg, "-frameinfo") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -frameinfo <pfn>\n");
            return -1;
        }

        unsigned long pfn = strtoul(argv[2], 0, 0);
        frame_info(pfn);
    } else if (strcmp(arg, "-memused") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -memused <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        mem_used(pid);
    } else if (strcmp(arg, "-mapva") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: sudo ./pvm -mapva <pid> <VA>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        unsigned long VA = strtoul(argv[3], 0, 0);
        map_va(pid, VA);
    } else if (strcmp(arg, "-pte") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: sudo ./pvm -pte <pid> <VA>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        unsigned long VA = strtoul(argv[3], 0, 0);
        pte(pid, VA);
    } else if (strcmp(arg, "-maprange") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: sudo ./pvm -maprange <pid> <VA1> <VA2>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        unsigned long VA1 = strtoul(argv[3], 0, 0);
        unsigned long VA2 = strtoul(argv[4], 0, 0);
        map_range(pid, VA1, VA2);
    } else if (strcmp(arg, "-mapall") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -mapall <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        map_all(pid);
    } else if (strcmp(arg, "-mapallin") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -mapallin <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        map_all_in(pid);
    } else if (strcmp(arg, "-alltablesize") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -alltablesize <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        all_table_size(pid);
    }

    return 0;
}
