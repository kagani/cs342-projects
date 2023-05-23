#include "mem_utils.h"

void free_fc(unsigned long pfn_begin, unsigned long pfn_end) {
    // Open /proc/kpagecount
    int kpc = open("/proc/kpagecount", O_RDONLY);

    if (kpc < 0) {
        printf("Error opening /proc/kpagecount\n");
        return;
    }

    printf("kpc = %d\n", kpc);

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
            return;
        }

        // Check if the value is 0
        if (value == 0) {
            count++;
        }
    }

    printf("%d\n", count);
}
