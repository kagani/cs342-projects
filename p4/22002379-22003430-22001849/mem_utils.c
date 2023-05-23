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

/**
 * @brief
pvm -mapva PID VA: When invoked with this option, your program
will find and print out the physical address corresponding to the virtual
address VA for the process PID. The physical address will be printed in
4 hexadecimal in the form 0x.... The printed value will be 16 digits long.
The first 4 digits will be 0x0000. 
*/
void mapva(unsigned long PID, unsigned long VA) {

}


/**
 * @brief
pvm -pte PID VA: When invoked with this option, your program will
find and print out detailed information for the page corresponding to the
virtual address VA of the process PID. The information will be obtained
from /proc/PID/pagemap file. Numbers (physical frame number or
swap offset) must be printed in hexadecimal form. 
*/
void pte(unsigned long PID, unsigned long VA) {

}

/**
 * @brief
pvm -maprange PID VA1 VA2: When invoked with this option, your
program will find and print out (page number, frame number) mappings
for the virtual address range [VA1, VA2). For each page in the range, your
program will print a line of information that contains the page number
and the corresponding frame number (if any). If a page in the range is not
a used page (i.e., is not in one of the virtual memory areas of the process),
you will print unused instead of a frame number. If a page is a used page
(i.e., in one of the virtual memory areas of the process), but is not in
memory, you will print not-in-memory instead of a frame number. 
*/
void maprange(unsigned long PID, unsigned long VA1, unsigned long VA1) {

}