#pragma once

#include <chrono>
#include <concepts>

/**
 * @brief Measures the time taken by a function to execute
 * 
 * @tparam Func The function type
 * @tparam Args The arguments types
 * @param p_func The function to execute
 * @param p_args The arguments to pass to the function
 * @return The result of the function and the time taken to execute it
*/
template<typename Func, typename... Args>
requires std::invocable<Func, Args...>
auto timed_execute_ms(Func *p_func, Args... p_args) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*p_func)(p_args...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::make_pair(result, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

/**
 * @brief Measures the time taken by a function to execute and outputs the time
 * 
 * @tparam Func The function type
 * @tparam Args The arguments types
 * @param p_func The function to execute
 * @param p_args The arguments to pass to the function
 * @param p_name Tag to identify time measurement
 * @return The result of the function
*/
template<typename Func, typename... Args>
requires std::invocable<Func, Args...>
auto log_timed_execute_ms(const std::string &p_name, Func *p_func, Args... p_args) {
    auto [result, time] = timed_execute_ms(p_func, p_args...);
    std::cout << p_name << " time: " << time << "ms" << std::endl;
    return result;
}