#include "MemoryTracker.hpp"
#include <unordered_map>
#include <new>

size_t MemoryTracker::max_mem_usage = 0, curr_mem_usage = 0;
bool recording = false;
std::unordered_map<void*, size_t> alloc_size_table;

void MemoryTracker::start_mem_record() {
    curr_mem_usage = 0;
    max_mem_usage = 0;
    recording = true;
}

void MemoryTracker::stop_mem_record() {
    recording = false;
    alloc_size_table.clear();
}

void log_mem_alloc(void* p_alloc_ptr, size_t p_size) {
    if(!recording) return;
    curr_mem_usage += p_size;
    MemoryTracker::max_mem_usage = std::max(MemoryTracker::max_mem_usage, curr_mem_usage);
    recording = false;
    alloc_size_table[p_alloc_ptr] = p_size;
    recording = true;
}

void unlog_mem_alloc(void* p_alloc_ptr) {
    if(!recording) return;
    recording = false;
    curr_mem_usage -= alloc_size_table[p_alloc_ptr];
    alloc_size_table.erase(p_alloc_ptr);
    recording = true;
}

void* operator new(std::size_t p_size) {
    if (void *ptr = std::malloc(p_size)) {
        log_mem_alloc(ptr, p_size);
        return ptr;
    }
    throw std::bad_alloc{};
}
 
void* operator new[](std::size_t p_size) {
    if (void *ptr = std::malloc(p_size)) {
        log_mem_alloc(ptr, p_size);
        return ptr;
    } 
    throw std::bad_alloc{};
}


void operator delete(void* ptr) noexcept {std::free(ptr); unlog_mem_alloc(ptr); }
 
void operator delete(void* ptr, std::size_t size) noexcept {std::free(ptr); unlog_mem_alloc(ptr); }
 
void operator delete[](void* ptr) noexcept {std::free(ptr); unlog_mem_alloc(ptr); }
 
void operator delete[](void* ptr, std::size_t size) noexcept {std::free(ptr); unlog_mem_alloc(ptr); }
