#include "mem_utils.h"
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

void free_fc(unsigned long pfnBegin, unsigned long pfnEnd) {
    // Open /proc/kpagecount
    int kpc = open("/proc/kpagecount", O_RDONLY);

    if (kpc < 0) {
        fprintf(stderr, "Error opening /proc/kpagecount\n");
        fprintf(stderr, "You probably forgot sudo\n");
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
            fprintf(stderr, "Error reading from /proc/kpagecount\n");
            fprintf(stderr, "PFN %lu is invalid\n", i);
            printf("Free frame count between %lu-%lu=%d\n", pfnBegin, pfnEnd,
                   count);
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
    char* kpageflags[] = {
        "LOCKED",        "ERROR",     "REFERENCED",  "UPTODATE",
        "DIRTY",         "LRU",       "ACTIVE",      "SLAB",
        "WRITEBACK",     "RECLAIM",   "BUDDY",       "MMAP",
        "ANON",          "SWAPCACHE", "SWAPBACKED",  "COMPOUND_HEAD",
        "COMPOUND_TAIL", "HUGE",      "UNEVICTABLE", "HWPOISON",
        "NOPAGE",        "KSM",       "THP",         "BALLOON",
        "ZERO_PAGE",     "IDLE"};

    for (int i = 0; i < 26; i++) {
        printf("%02d. %-15s ", i, kpageflags[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n\n");

    printf("%-12s", "FRAME#");
    for (int i = 0; i < 26; i++) {
        printf("%02d ", i);
    }
    printf("\n");

    printf("%#011lx ", pfn);
    for (int i = 0; i < 26; i++) {
        printf(" %-2lu", (value >> i) & 1);
    }
    printf("\n");
}

void mem_used(int pid) {
    char mapFileName[256];
    snprintf(mapFileName, 256, "/proc/%d/maps", pid);
    FILE* mapsFd = fopen(mapFileName, "r");

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

    printf("(pid=%d) memused: virtual=%llu KB, ", pid, sum);
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
                continue;
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
            }
        }
    }

    printf("pmem_all=%llu KB, ", totalMemorySum >> 10);
    printf("pmem_alone=%llu KB, ", exclusiveMemorySum >> 10);
    printf("mappedonce=%llu KB\n", exclusiveMemorySum >> 10);
}

void map_va(int pid, unsigned long va) {
    char path[256];
    snprintf(path, 256, "/proc/%d/pagemap", pid);
    int fd = open(path, O_RDONLY);

    if (fd == -1) perror("[-] Cannot open pagemap on map_va");

    unsigned long vpn = (va >> 12);

    lseek(fd, vpn, SEEK_SET);

    uint64_t entry;
    read(fd, &entry, sizeof(uint64_t));
    if (entry & (1ULL << 63)) {
        uint64_t frameNumber = entry & ((1ULL << 55) - 1);
        printf("0x%016lx\n", (unsigned long long)frameNumber);
    } else {
        printf("VA not in memory.\n");
    }

    close(fd);
}

void pte(int pid, unsigned long va) {
    char path[256];
    snprintf(path, 256, "/proc/%d/pagemap", pid);
    int fd = open(path, O_RDONLY);
    printf("\n======================================================");
    if (fd == -1) perror("[-] Cannot open pagemap on pte");

    unsigned long vpn = (va >> 12);
    lseek(fd, vpn, SEEK_SET);
    uint64_t entry;
    read(fd, &entry, sizeof(uint64_t));

    if (entry & (1ULL << 63)) {
        printf("\nVirtual Address: 0x%lx is in physical memory.", va);
        uint64_t frameNumber = entry & ((1ULL << 55) - 1);
        if (entry & (1ULL << 62)) {
            printf("\nVirtual Address: 0x%lx is swapped out.", va);
            printf("\nSwap Offset of the VA = 0x%016lx",
                   (unsigned long long)frameNumber);
        } else {
            printf("\nVirtual Address: 0x%lx is not swapped out.", va);
            printf("\nFrame number of the VA = 0x%016lx",
                   (unsigned long long)frameNumber);
        }
    } else {
        printf("\nVirtual Address: 0x%lx is not in physical memory.", va);
        printf("\n======================================================\n");
        close(fd);
    }
    printf("\n======================================================\n");
}

void map_range(int pid, unsigned long vaBegin, unsigned long vaEnd) {
    char path[256];
    snprintf(path, 256, "/proc/%d/pagemap", pid);
    int fd = open(path, O_RDONLY);
    printf("\n======================================================");
    if (fd == -1) perror("[-] Cannot open pagemap on map_range");

    unsigned long startVpn = vaBegin >> 12;
    unsigned long endVpn = vaEnd >> 12;
    lseek(fd, startVpn, SEEK_SET);

    uint64_t entry;
    for (off_t i = startVpn; i <= endVpn; i = i + 16) {
        printf("\n===>%lx", i);
        read(fd, &entry, sizeof(uint64_t));
        if (entry & (1ULL << 63)) {
            uint64_t frameNumber = entry & ((1ULL << 55) - 1);
            printf("\nVirtual Address: %lu ==> Frame Number: 0x%lx", i,
                   frameNumber);
        } else {
            printf("\nVirtual Address: %lu unused.", i);
        }
    }
    printf("\n======================================================\n");
    close(fd);
}

void map_all(int id) { map(id, 0); }

void map_all_in(int id) { map(id, 1); }

void map(int id, int inMem) {
    int pid = id;

    // get pagesize
    const int pageSize = getpagesize();

    // open maps & pagemap
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

    // get mapped intervals from proc/pid/maps interval start: va1, interval
    // end: va2
    char line[1000];
    while (fgets(line, sizeof(line), maps)) {
        unsigned long long va1, va2, pagemapEntry, readBytes;
        off_t offset;
        if (sscanf(line, "%llx-%llx", &va1, &va2) == 2) {
            // iterate whole interval by incrementing va1 by pagesize
            for (; va1 < va2; va1 += pageSize) {
                // pagemap index -> virtual page number = va / pagesize
                offset = va1 / pageSize * sizeof(unsigned long long);
                if (lseek(pagemap, offset, SEEK_SET) < 0) {
                    return;
                }
                readBytes =
                    read(pagemap, &pagemapEntry, sizeof(unsigned long long));
                if (readBytes == -1) {
                    printf("Error reading from pagemap");
                    break;
                }
                if (readBytes == 0) {
                    printf("End of pagemap");
                    break;
                }
                // if 63rd bit is 1, page is present in memory
                if (pagemapEntry & (1ULL << 63)) {
                    // get 0-54th bits for pfn
                    unsigned long long bitmask = 0x7FFFFFFFFFFFFF;
                    unsigned long long result = pagemapEntry & bitmask;
                    printf("Page %llx: Frame %llx\n", va1 / pageSize, result);
                } else {
                    if (inMem == 0) {
                        printf("Page %llx: not-in-memory\n", va1 / pageSize);
                    } else {
                        continue;
                    }
                }
            }
        }
    }
    fclose(maps);
    close(pagemap);
}

void all_table_size(int pid) {
    char mapsPath[256];
    snprintf(mapsPath, 256, "/proc/%d/maps", pid);

    FILE* mapsFd = fopen(mapsPath, "r");

    if (mapsFd <= 0) {
        printf("Error opening %s.\n", mapsPath);
        return;
    }

    char line[356];

    // Each individual table can have 512 entries
    int level1Count = 1;  // Max 1 = 2^0
    int level2Count = 0;  // Max 512 = 2^9
    int level3Count = 0;  // Max 512 * 512 = 2^18
    int level4Count = 0;  // Max 512 * 512 * 512 = 2^27

    // Yes, I know how this looks.
    // But it works ¯\_(ツ)_/¯
    char* level2 = (char*)malloc(512);
    char* level3 = (char*)malloc(512 * 512);
    char* level4 = (char*)malloc(512 * 512 * 512);

    for (int i = 0; i < 1lu << 9; i++) {
        level2[i] = 0;
    }

    for (int i = 0; i < 1LU << 18; i++) {
        level3[i] = 0;
    }

    for (int i = 0; i < 1LU << 27; i++) {
        level4[i] = 0;
    }

    while (!feof(mapsFd)) {
        if (fgets(line, sizeof(line), mapsFd) == NULL) {
            break;
        }

        unsigned long vaBegin = 0, vaEnd = 0;
        sscanf(line, "%lx-%lx", &vaBegin, &vaEnd);

        for (unsigned long i = vaBegin >> 12; i < vaEnd >> 12; i++) {
            unsigned long pml4Index = i & 0x1ff;  // level 4 index (not needed)
            unsigned long pdptIndex = (i >> 9) & 0x1ff;  // level 3 index
            unsigned long pdIndex = (i >> 18) & 0x1ff;   // level 2 index
            unsigned long ptIndex = (i >> 27) & 0x1ff;   // level 1 index

            // Create a mask to check if the entry is present
            if (!level2[ptIndex]) {
                level2[ptIndex] = 1;
                level2Count++;
            }

            if (!level3[(ptIndex << 9) + pdIndex]) {
                level3[(ptIndex << 9) + pdIndex] = 1;
                level3Count++;
            }

            if (!level4[(ptIndex << 9) + (pdIndex << 18) + pdptIndex]) {
                level4[(ptIndex << 9) + (pdIndex << 18) + pdptIndex] = 1;
                level4Count++;
            }
        }
    }

    printf(
        "(pid=%d) total memory occupied by 4-level page table: %lu KB (%lu "
        "frames)\n",
        pid, (level1Count + level2Count + level3Count + level4Count) << 2,
        (level1Count + level2Count + level3Count + level4Count));
    printf(
        "(pid=%d) number of page tables used: level1=%d, level2=%d, "
        "level3=%d, level4=%d\n",
        pid, level1Count, level2Count, level3Count, level4Count);
    free(level2);
    free(level3);
    free(level4);
}
