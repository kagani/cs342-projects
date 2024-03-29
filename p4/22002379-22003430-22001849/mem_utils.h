#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#define PAGE_SIZE 4096

/**
 * @brief find and print out (in decimal) the number of empty (i.e.,
 * free) frames (mapped count = 0, or nopage flag is set) between physical
 * frame numbers PFN1 and PFN2, i.e., in range [PFN1, PFN2). An example
 * invocation is: pvm -freefc 0x00 0x1a. That means we want to count
 * the free frames in range [0, 26) (in decimal).
 *
 * @param pfn_begin
 * @param pfn_end
 */
void free_fc(unsigned long pfnBegin, unsigned long pfnEnd);

/**
 * @brief This function prints detailed
 * information (various flag values and mapping count) for the specified frame.
 * PFN is the frame number. The flags whose value will be printed out are those
 * that can found in the /proc/kpageflags file.
 *
 * @param pfn
 */
void frame_info(unsigned long pfn);

/**
 * @brief This function finds out the total
 * amount of virtual memory and physical memory used by the process PID (in KB).
 *
 * @param pid
 */
void mem_used(int pid);

/**
 * @brief pvm -mapva PID VA: When invoked with this option, program
 * will find and print out the physical address corresponding to the virtual
 * address VA for the process PID. The physical address will be printed in
 * 4 hexadecimal in the form 0x.... The printed value will be 16 digits long.
 * The first 4 digits will be 0x0000.
 *
 * @param pid
 * @param va
 */
void map_va(int pid, unsigned long va);

/**
 * @brief
 * pvm -pte PID VA: When invoked with this option, your program will
 * find and print out detailed information for the page corresponding to the
 * virtual address VA of the process PID. The information will be obtained
 * from /proc/PID/pagemap file. Numbers (physical frame number or
 * swap offset) must be printed in hexadecimal form.
 *
 * @param pid
 * @param va
 */
void pte(int pid, unsigned long va);

/**
 * @brief
 * pvm -maprange PID VA1 VA2: When invoked with this option, your
 * program will find and print out (page number, frame number) mappings
 * for the virtual address range [VA1, VA2). For each page in the range, your
 * program will print a line of information that contains the page number
 * and the corresponding frame number (if any). If a page in the range is not
 * a used page (i.e., is not in one of the virtual memory areas of the process),
 * you will print unused instead of a frame number. If a page is a used page
 * (i.e., in one of the virtual memory areas of the process), but is not in
 * memory, you will print not-in-memory instead of a frame number.
 *
 * @param pid
 * @param vaBegin
 * @param vaEnd
 */
void map_range(int pid, unsigned long vaBegin, unsigned long vaEnd);
void map_all(int pid);
void map_all_in(int pid);
void map(int pid, int inMem);
void all_table_size(int pid);
