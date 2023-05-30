#include <string.h>

#include "mem_utils.h"

int main(int argc, char* argv[]) {
    char* cur = argv[1];
    if (strcmp(cur, "-freefc") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: sudo ./pvm -freefc <pfnBegin> <pfnEnd>\n");
            return -1;
        }

        unsigned long pfnBegin = strtoul(argv[2], 0, 0);
        unsigned long pfnEnd = strtoul(argv[3], 0, 0);
        free_fc(pfnBegin, pfnEnd);
    } else if (strcmp(cur, "-frameinfo") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -frameinfo <pfn>\n");
            return -1;
        }

        unsigned long pfn = strtoul(argv[2], 0, 0);
        frame_info(pfn);
    } else if (strcmp(cur, "-memused") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -memused <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        mem_used(pid);
    } else if (strcmp(cur, "-mapva") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: sudo ./pvm -mapva <pid> <VA>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        unsigned long VA = strtoul(argv[3], 0, 0);
        map_va(pid, VA);
    } else if (strcmp(cur, "-pte") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: sudo ./pvm -pte <pid> <VA>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        unsigned long VA = strtoul(argv[3], 0, 0);
        pte(pid, VA);
    } else if (strcmp(cur, "-maprange") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: sudo ./pvm -maprange <pid> <VA1> <VA2>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        unsigned long VA1 = strtoul(argv[3], 0, 0);
        unsigned long VA2 = strtoul(argv[4], 0, 0);
        map_range(pid, VA1, VA2);
    } else if (strcmp(cur, "-mapall") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -mapall <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        map_all(pid);
    } else if (strcmp(cur, "-mapallin") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -mapallin <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        map_all_in(pid);
    } else if (strcmp(cur, "-alltablesize") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: sudo ./pvm -alltablesize <pid>\n");
            return -1;
        }

        int pid = atoi(argv[2]);
        all_table_size(pid);
    }

    return 0;
}
