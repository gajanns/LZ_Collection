#pragma once
#include <cstdlib>


namespace MemoryTracker
{
    /**
     * @brief Peak Memory Allocation on Heap between calls to start_mem_record and stop_mem_record.
     * 
     */
    extern size_t peak_mem_usage;

    /**
     * @brief (Re-)Start tracking memory allocations on Heap
     * 
     */
    void start_mem_record();

    /**
     * @brief Stop tracking memory allocations on Heap and set peak_mem_usage to the peak memory allocation.
     * 
     */
    void stop_mem_record();
}


