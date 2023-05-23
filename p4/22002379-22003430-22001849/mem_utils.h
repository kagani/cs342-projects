#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

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
void free_fc(unsigned long pfn_begin, unsigned long pfn_end);
void frame_info(int pfn);
void mem_used(int pid);
void map_va(unsigned long PID, unsigned long VA);
void pte(unsigned long PID, unsigned long VA);
void map_range(unsigned long PID, unsigned long VA1, unsigned long VA1);
void map_all(int pid);
void map_all_in(int pid);
void all_table_size(int pid);
