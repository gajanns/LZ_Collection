#pragma once
#include <cstdlib>
#include <unordered_map>


namespace MemoryTracker
{
    /**
     * @brief Allocated Memory on Heap since call to start_mem_record.
     * 
     */
    extern size_t curr_mem_usage;

    /**
     * @brief (Re-)Start tracking memory allocations on Heap
     * 
     */
    void start_mem_record();

    /**
     * @brief Stop tracking memory allocations on Heap
     * 
     */
    void stop_mem_record();
}


