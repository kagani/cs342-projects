#include "mem_utils.h"

void free_fc(unsigned long pfnBegin, unsigned long pfnEnd) {
    // Open /proc/kpagecount
    int kpc = open("/proc/kpagecount", O_RDONLY);

    if (kpc < 0) {
        printf("Error opening /proc/kpagecount\n");
        printf("You probably forgot sudo\n");
        return;
    }

    int count = 0;
    for (unsigned long i = pfnBegin; i < pfnEnd; i++) {
        // Seek to the correct position
        unsigned long long offset = i * sizeof(unsigned long);
        lseek(kpc, offset, SEEK_SET);

        // Read the value
        unsigned long value;
        // read returns the number of bytes read
        int readBytes = read(kpc, &value, sizeof(unsigned long));

        if (readBytes != sizeof(unsigned long)) {
            printf("Error reading from /proc/kpagecount\n");
            printf("PFN %lu is invalid\n", i);
            printf("Free frame count between %lu-%lu=%d\n", pfnBegin, i, count);
        }

        // Check if the value is 0
        // FYI: -1 is also free
        // but haven't reclaimed by the kernel
        if (value == 0) {
            count++;
        }
    }

    printf("Free frame count between %lu-%lu=%d\n", pfnBegin, pfnEnd, count);
}

void map_va(int pid, unsigned long va) {}

void pte(int pid, unsigned long va) {}

void map_range(int pid, unsigned long vaBegin, unsigned long vaEnd) {}