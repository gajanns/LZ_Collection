#pragma once
#include <cstdlib>
#include <unordered_map>


namespace MemoryTracker
{
    extern size_t curr_mem_usage;

    void start_mem_record();
    void stop_mem_record();
    void log_mem_alloc(size_t p_size);
    //void log_mem_free(size_t p_size);
}


