#include <stdio.h>

void free_fc(int pfn_begin, int pfn_end);
void frame_info(int pfn);
void mem_used(int pid);
void map_va(int pid, int va);
void pte(int pid, int va);
void map_range(int pid, int va_begin, int va_end);
void map_all(int pid);
void map_all_in(int pid);
void all_table_size(int pid);
