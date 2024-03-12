#pragma once

#include <iostream>
#include <chrono>
#include "MemoryTracker.hpp"
#include "Coder.hpp"

namespace Compression {
    struct CompressionStatistics {
        size_t m_input_size;
        size_t m_output_size;
        size_t m_run_time_milliseconds;
        size_t m_mem_usage;

        void dump() {
            std::cout << "Input size: " << m_input_size << std::endl;
            std::cout << "Output size: " << m_output_size << std::endl;
            std::cout << "Compression ratio: " << (double) m_output_size / m_input_size << std::endl;
            std::cout << "Compression time: " << m_run_time_milliseconds << "ms" << std::endl;
            std::cout << "Memory Usage: " << m_input_size << " Bytes" << std::endl;
        }
    };

    template <typename IN, typename OUT>
    class Compressor
    { 
        private:
            virtual void compress_impl(Coder::Decoder<IN> &p_in, Coder::Encoder<OUT> &p_out) = 0;
            virtual void decompress_impl(Coder::Decoder<OUT> &p_in, Coder::Encoder<IN> &p_out) = 0;

        public:
            CompressionStatistics m_stats;

            inline void compress(Coder::Decoder<IN> &p_in, Coder::Encoder<OUT> &p_out) {
                #ifdef PERF
                MemoryTracker::start_mem_record();
                auto start = std::chrono::high_resolution_clock::now();
                #endif
                compress_impl(p_in, p_out);
                p_out.flush();
                #ifdef PERF
                auto end = std::chrono::high_resolution_clock::now();
                MemoryTracker::stop_mem_record();
                m_stats.m_mem_usage = MemoryTracker::curr_mem_usage;
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.bytes_written();
                m_stats.dump();
                #endif
            }
            
            inline void decompress(Coder::Decoder<OUT> &p_in, Coder::Encoder<IN> &p_out) {
                #ifdef PERF
                MemoryTracker::start_mem_record();
                auto start = std::chrono::high_resolution_clock::now();
                #endif
                decompress_impl(p_in, p_out);
                p_out.flush();
                #ifdef PERF
                auto end = std::chrono::high_resolution_clock::now();
                MemoryTracker::stop_mem_record();
                m_stats.m_mem_usage = MemoryTracker::curr_mem_usage;
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.bytes_written();
                #endif
            }
    };
}