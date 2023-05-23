#include "mem_utils.h"

void free_fc(unsigned long pfn_begin, unsigned long pfn_end) {
    // Open /proc/kpagecount
    int kpc = open("/proc/kpagecount", O_RDONLY);

    if (kpc < 0) {
        printf("Error opening /proc/kpagecount\n");
        printf("You probably forgot sudo\n");
        return;
    }

    int count = 0;
    for (unsigned long i = pfn_begin; i < pfn_end; i++) {
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
            printf("Free frame count between %lu-%lu=%d\n", pfn_begin, i,
                   count);
        }

        // Check if the value is 0
        // FYI: -1 is also free
        // but haven't reclaimed by the kernel
        if (value == 0) {
            count++;
        }
    }

    printf("Free frame count between %lu-%lu=%d\n", pfn_begin, pfn_end, count);
}
