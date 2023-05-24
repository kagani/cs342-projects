#include "mem_utils.h"

void free_fc(unsigned long pfnBegin, unsigned long pfnEnd) {
    // Open /proc/kpagecount
    int kpc = open("/proc/kpagecount", O_RDONLY);

    if (kpc < 0) {
        printf("Error opening /proc/kpagecount\n");
        printf("You probably forgot sudo\n");
        return;
    }

    printf("kpc = %d\n", kpc);

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

void frame_info(unsigned long pfn) {
    int kpf = open("/proc/kpageflags", O_RDONLY);

    if (kpf < 0) {
        printf("Error opening /proc/kpagecount\n");
        printf("You probably forgot sudo\n");
        return;
    }

    // Seek to correct offset
    unsigned long long offset = pfn * sizeof(unsigned long);
    lseek(kpf, offset, SEEK_SET);

    // Read the value (total of 26 bits)
    unsigned long value;
    // read returns the number of bytes read
    int readBytes = read(kpf, &value, sizeof(unsigned long));

    if (readBytes != sizeof(unsigned long)) {
        printf("Error reading from /proc/kpageflags\n");
        printf("PFN %lu is invalid\n", pfn);
        return;
    }

    // These are specified in
    // https://www.kernel.org/doc/html/latest/admin-guide/mm/pagemap.html?highlight=kpageflags
    printf("Info for PFN %lu\n", pfn);
    printf("0. LOCKED = %lu\n", value & 1);
    printf("1. ERROR = %lu\n", (value >> 1) & 1);
    printf("2. REFERENCED = %lu\n", (value >> 2) & 1);
    printf("3. UPTODATE = %lu\n", (value >> 3) & 1);
    printf("4. DIRTY = %lu\n", (value >> 4) & 1);
    printf("5. LRU = %lu\n", (value >> 5) & 1);
    printf("6. ACTIVE = %lu\n", (value >> 6) & 1);
    printf("7. SLAB = %lu\n", (value >> 7) & 1);
    printf("8. WRITEBACK = %lu\n", (value >> 8) & 1);
    printf("9. RECLAIM = %lu\n", (value >> 9) & 1);
    printf("10. BUDDY = %lu\n", (value >> 10) & 1);
    printf("11. MMAP = %lu\n", (value >> 11) & 1);
    printf("12. ANON = %lu\n", (value >> 12) & 1);
    printf("13. SWAPCACHE = %lu\n", (value >> 13) & 1);
    printf("14. SWAPBACKED = %lu\n", (value >> 14) & 1);
    printf("15. COMPOUND_HEAD = %lu\n", (value >> 15) & 1);
    printf("16. COMPOUND_TAIL = %lu\n", (value >> 16) & 1);
    printf("17. HUGE = %lu\n", (value >> 17) & 1);
    printf("18. UNEVICTABLE = %lu\n", (value >> 18) & 1);
    printf("19. HWPOISON = %lu\n", (value >> 19) & 1);
    printf("20. NOPAGE = %lu\n", (value >> 20) & 1);
    printf("21. KSM = %lu\n", (value >> 21) & 1);
    printf("22. THP = %lu\n", (value >> 22) & 1);
    printf("23. BALLOON = %lu\n", (value >> 23) & 1);
    printf("24. ZERO_PAGE = %lu\n", (value >> 24) & 1);
    printf("25. IDLE = %lu\n", (value >> 25) & 1);
    printf("26. KPF_THP = %lu\n", (value >> 26) & 1);
}

void mem_used(int pid) {}

void map_va(int pid, unsigned long va) {}

void pte(int pid, unsigned long va) {}

void map_range(int pid, unsigned long vaBegin, unsigned long vaEnd) {}