#ifdef PERF
#include "MemoryTracker.hpp"
#include "malloc_count.h"

size_t MemoryTracker::peak_mem_usage = 0, cur_mem_usage = 0;


void MemoryTracker::start_mem_record() {
    peak_mem_usage = 0;
    cur_mem_usage = malloc_count_current();
    malloc_count_reset_peak();
}

void MemoryTracker::stop_mem_record() {
    peak_mem_usage = malloc_count_peak() - cur_mem_usage;
}
#endif