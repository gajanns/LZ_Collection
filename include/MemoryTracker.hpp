#pragma once

#include <iostream>

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

/**
     * @brief Measures the memory used by a function to execute
     * 
     * @tparam Func The function type
     * @tparam Args The arguments types
     * @param p_func The function to execute
     * @param p_args The arguments to pass to the function
     * @return The result of the function and the memory used to execute it
    */
    template<typename Func, typename... Args>
    requires std::invocable<Func, Args...>
    auto mem_tracked_execute(Func *p_func, Args... p_args) {
        MemoryTracker::start_mem_record();
        auto result = (*p_func)(p_args...);
        MemoryTracker::stop_mem_record();
        return std::make_pair(result, MemoryTracker::peak_mem_usage);
    }

    /**
     * @brief Measures the memory used by a member function to execute
     * 
     * @tparam Func The member function type
     * @tparam Class The class type
     * @tparam Args The arguments types
     * @param p_func The member function to execute
     * @param p_class Object to bind the member function to
     * @param p_args The arguments to pass to the function
     * @return The result of the function and the memory used to execute it
    */
    template<typename Func, typename Class, typename... Args>
    requires std::is_member_function_pointer_v<Func Class::*> && std::is_class_v<Class>
    auto mem_tracked_execute(Func Class::*p_func, Class &p_class, Args... p_args) {
        MemoryTracker::start_mem_record();
        auto result = (p_class.*p_func)(p_args...);
        MemoryTracker::stop_mem_record();
        return std::make_pair(result, MemoryTracker::peak_mem_usage);
    }

    /**
     * @brief Measures the memory used by a function to execute and outputs the memory
     * 
     * @tparam Func The function type
     * @tparam Args The arguments types
     * @param p_func The function to execute
     * @param p_args The arguments to pass to the function
     * @param p_name Tag to identify memory measurement
     * @return The result of the function
    */
    template<typename Func, typename... Args>
    requires std::invocable<Func, Args...>
    auto log_mem_tracked_execute(Func *p_func, Args... p_args, const std::string &p_name) {
        auto [result, mem] = mem_tracked_execute(p_func, p_args...);
        std::cout << p_name << " memory: " << mem << " Bytes" << std::endl;
        return result;
    }

    /**
     * @brief Measures the memory used by a member function to execute and outputs the memory
     * 
     * @tparam Func The member function type
     * @tparam Class The class type
     * @tparam Args The arguments types
     * @param p_func The member function to execute
     * @param p_class Object to bind the member function to
     * @param p_args The arguments to pass to the function
     * @param p_name Tag to identify memory measurement
     * @return The result of the function
    */
    template<typename Func, typename Class, typename... Args>
    requires std::is_member_function_pointer_v<Func Class::*> && std::is_class_v<Class>
    auto log_mem_tracked_execute(Func Class::*p_func, Class &p_class, Args... p_args, const std::string &p_name) {
        auto [result, mem] = mem_tracked_execute(p_func, p_class, p_args...);
        std::cout << p_name << " memory: " << mem << " Bytes" << std::endl;
        return result;
    }


