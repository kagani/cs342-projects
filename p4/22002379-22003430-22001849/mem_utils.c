#include "mem_utils.h"

void free_fc(unsigned long pfnBegin, unsigned long pfnEnd) {
    // Open /proc/kpagecount
    int kpc = open("/proc/kpagecount", O_RDONLY);

    if (kpc < 0) {
        printf("Error opening /proc/kpagecount\n");
        printf("You probably forgot sudo\n");
        close(kpc);
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
            close(kpc);
            return;
        }

        // Check if the value is 0
        // FYI: -1 is also free
        // but haven't reclaimed by the kernel
        if (value == 0) {
            count++;
        }
    }

    close(kpc);

    printf("Free frame count between %lu-%lu=%d\n", pfnBegin, pfnEnd, count);
}

void frame_info(unsigned long pfn) {
    int kpf = open("/proc/kpageflags", O_RDONLY);

    if (kpf < 0) {
        printf("Error opening /proc/kpageflags\n");
        printf("You probably forgot sudo\n");
        close(kpf);
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
        close(kpf);
        return;
    }

    // These are specified in
    // https://www.kernel.org/doc/html/latest/admin-guide/mm/pagemap.html?highlight=kpageflags
    // However, seem to differ from the code ???
    // https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/linux/page-flags.h
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
    printf("23. OFFLINE = %lu\n", (value >> 23) & 1);
    printf("24. ZERO_PAGE = %lu\n", (value >> 24) & 1);
    printf("25. IDLE = %lu\n", (value >> 25) & 1);
    printf("26. PGTABLE = %lu\n", (value >> 26) & 1);
}

void mem_used(int pid) {
    char mapFileName[256];
    snprintf(mapFileName, 256, "/proc/%d/maps", pid);
    FILE *mapsFd = fopen(mapFileName, "r");

    if (mapsFd < 0) {
        printf("Error opening %s\n", mapFileName);
        printf("You probably forgot sudo\n");
        fclose(mapsFd);
        return;
    }

    // Calculate virtual memory usage
    unsigned long long sum = 0;
    while (!feof(mapsFd)) {
        unsigned long long addrStart = 0, addrEnd = 0;
        char mode[8];
        long offset;
        char dev[10];
        long inode;
        char path[256];  // 256 is PATH_MAX

        char buf[364];
        if (!fgets(buf, 364, mapsFd)) {
            break;
        }

        sscanf(buf, "%lx-%lx %s %ld %s %ld %s", &addrStart, &addrEnd, mode,
               &offset, dev, &inode, path);

        sum += (addrEnd - addrStart) >> 10;

        if (strcmp(path, "[vsyscall]") == 0) {
            break;
        }
    }

    printf("Total virtual memory usage: %llukb\n", sum);
    fclose(mapsFd);

    // Calculate physical memory usage (exclusive and inclusive)
    char pageMapFileName[256];
    snprintf(pageMapFileName, 256, "/proc/%d/pagemap", pid);
    int pmFd = open(pageMapFileName, O_RDONLY);

    unsigned long long totalMemorySum = 0;
    unsigned long long exclusiveMemorySum = 0;
    mapsFd = fopen(mapFileName, "r");
    int kpcFd = open("/proc/kpagecount", O_RDONLY);

    if (mapsFd < 0 || kpcFd < 0) {
        printf("Error opening %s or %s\n", mapFileName, "/proc/kpagecount");
        printf("You probably forgot sudo\n");
        fclose(mapsFd);
        close(kpcFd);
        return;
    }

    while (!feof(mapsFd)) {
        unsigned long long addrStart = 0, addrEnd = 0;
        char mode[8];
        long offset;
        char dev[10];
        long inode;
        char path[256];  // 256 is PATH_MAX

        char buf[364];
        if (!fgets(buf, 364, mapsFd)) {
            break;
        }

        // printf("buf = \n%s\n", buf);
        sscanf(buf, "%lx-%lx %s %ld %s %ld %s", &addrStart, &addrEnd, mode,
               &offset, dev, &inode, path);
        unsigned long pageIdx = addrStart >> 12;
        unsigned long pageEndIdx = addrEnd >> 12;

        for (unsigned long VPN = addrStart >> 12; VPN < pageEndIdx + 1; VPN++) {
            // Seek to correct location
            lseek(pmFd, VPN << 3, SEEK_SET);

            unsigned long data = 0;
            int readBytes = read(pmFd, &data, sizeof(unsigned long));

            if (readBytes != sizeof(unsigned long)) {
                printf("Failed to read %s\n", pageMapFileName);
                close(pmFd);
                close(kpcFd);
                fclose(mapsFd);
                return;
            }

            unsigned long isMapped = (data >> 63) & 1;    // Top bit
            unsigned long pfn = data & 0x7FFFFFFFFFFFFF;  // Top 56 bits

            if (!isMapped) {
                continue;
            }

            lseek(kpcFd, pfn << 3, SEEK_SET);

            // Read value
            long mapCount = 0;
            readBytes = read(kpcFd, &mapCount, sizeof(unsigned long));

            if (readBytes != sizeof(unsigned long)) {
                printf("Failed to read %s\n", "/proc/kpagecount");
                close(pmFd);
                close(kpcFd);
                fclose(mapsFd);
                return;
            }

            unsigned long memCount = 4096;  // page size

            if (VPN == pageEndIdx) {
                memCount = addrEnd - (VPN << 12);
            }

            // Private_Clean + Private_Dirty
            if (mapCount == 1) {
                exclusiveMemorySum += memCount;
            }

            // RSS
            if (mapCount >= 1) {
                totalMemorySum += memCount;
            } else {
                printf("mapCount %ld\n", mapCount);
            }
        }
    }

    printf("Total physical memory: %llukb\n", totalMemorySum >> 10);
    printf("Exclusive physical memory: %llukb\n", exclusiveMemorySum >> 10);
}

void map_va(int pid, unsigned long va) {
    char path[256];
    snprintf(path, 256, "/proc/%d/pagemap", pid);
    int fd = open(path, O_RDONLY);

    if(fd == -1) perror("[-] Cannot open pagemap on map_va");

    off_t size = lseek(fd, 0, SEEK_END);
    off_t pageCnt = size / sizeof(uint64_t);

    lseek(fd, 0, SEEK_SET);

    uint64_t entry;
    for (off_t i = 0; i < pageCnt; i++) {
        read(fd, &entry, sizeof(uint64_t));
        if (entry & (1ULL << 63)) {
            if(va == i) {
                uint64_t frameNumber = entry & ((1ULL << 55) - 1);
                printf("0x%016lx\n", (unsigned long long)frameNumber);
                break;
            }
        }
    }
    close(fd);
}

void pte(int pid, unsigned long va) {}

void map_range(int pid, unsigned long vaBegin, unsigned long vaEnd) {}

void map_all(int id){
    map(id, 0);
}

void map_all_in(int id) {
    map(id, 1);
}

void map(int id, int inMem){
    int pid = id;

    //get pagesize
    const int pageSize = getpagesize();

    //open maps & pagemap
    char pagemapPath[100];
    sprintf(pagemapPath, "/proc/%d/pagemap", pid);

    char mapsPath[100];
    sprintf(mapsPath, "/proc/%d/maps", pid);

    FILE* maps = fopen(mapsPath, "r");
    if (maps == NULL) {
        printf("Error opening the file.\n");
        return;
    }

    int pagemap = open(pagemapPath, O_RDONLY);
    if (pagemap < 0) {
        printf("Error opening %s", pagemapPath);
        return;
    }

    // get mapped intervals from proc/pid/maps interval start: va1, interval end: va2
    char line[1000];
    while (fgets(line, sizeof(line), maps)) {
        unsigned long long va1, va2, pagemapEntry, readBytes;
        off_t offset;
        if (sscanf(line, "%llx-%llx", &va1, &va2) == 2) {
            // iterate whole interval by incrementing va1 by pagesize
            for(;va1 < va2; va1 += pageSize) {
                // pagemap index -> virtual page number = va / pagesize
                offset = va1 / pageSize * sizeof(unsigned long long);
                if(lseek(pagemap, offset, SEEK_SET) < 0) {
                    return;
                }
                readBytes = read(pagemap, &pagemapEntry, sizeof(unsigned long long));
                if (readBytes == -1) {
                    printf("Error reading from pagemap");
                    break;
                }
                if (readBytes == 0) {
                    printf("End of pagemap");
                    break;
                }
                // if entry = 0, not used so skip
                if(pagemapEntry == 0) {
                    continue;
                }
                // if 63rd bit is 1, page is present in memory
                if (pagemapEntry & (1ULL << 63)) {
                    // get 0-54th bits for pfn
                    unsigned long long bitmask = 0x3FFFFFFFFFFFFF;
                    unsigned long long result = pagemapEntry & bitmask;
                    printf("Page %llx: Frame %llx\n", va1 / pageSize, result);
                } else {
                    if(inMem == 0) {
                        printf("Page %llx: not-in-memory\n", va1 / pageSize);
                    }
                    else {
                        continue;
                    }

                }
            }
        }
    }
    fclose(maps);
    close(pagemap);
}

