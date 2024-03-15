#include "MemoryTracker.hpp"
#include <iostream>
#include <unordered_map>
#include <new>

size_t MemoryTracker::curr_mem_usage = 0;
bool recording = false;

void MemoryTracker::start_mem_record() {
    curr_mem_usage = 0;
    recording = true;
}

void MemoryTracker::stop_mem_record() {
    recording = false;
}

void log_mem_alloc(size_t p_size) {
    if(!recording) return;
    MemoryTracker::curr_mem_usage += p_size;
}

void* operator new(std::size_t p_size) {

    if (void *ptr = std::malloc(p_size)) {
        log_mem_alloc(p_size);
        return ptr;
    }
    throw std::bad_alloc{};
}
 
void* operator new[](std::size_t p_size) {
    if (void *ptr = std::malloc(p_size)) {
        log_mem_alloc(p_size);
        return ptr;
    } 
    throw std::bad_alloc{};
}

/*
void operator delete(void* ptr) noexcept {std::free(ptr); }
 
void operator delete(void* ptr, std::size_t size) noexcept {std::free(ptr); }
 
void operator delete[](void* ptr) noexcept {std::free(ptr); }
 
void operator delete[](void* ptr, std::size_t size) noexcept {std::free(ptr); }
*/