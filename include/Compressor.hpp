#pragma once

#include <iostream>
#include <chrono>
#include "MemoryTracker.hpp"
#include "Coder.hpp"
#include "StreamView.hpp"

namespace Compression {
    /**
     * @brief Structure for collecting Performance-Stats. To be populated by Compressor.
     * 
     */
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
            std::cout << "Memory Usage: " << m_mem_usage << " Bytes" << std::endl;
        }
    };

    template <typename Factor>
    class Compressor
    { 
        private:
            virtual void compress_impl(StreamView &p_in, Coder::Encoder<Factor> &p_out) = 0;
            virtual void decompress_impl(Coder::Decoder<Factor> &p_in, StreamView &p_out) = 0;

        public:
            /**
             * @brief Collection of Performance-Stats
             * 
             */
            CompressionStatistics m_stats;

            /**
             * @brief Perform Compression
             * 
             * @param p_in Raw Input-Stream
             * @param p_out Encoded Output-Stream
             */
            void compress(StreamView &p_in, Coder::Encoder<Factor> &p_out) {
                #ifdef PERF
                MemoryTracker::start_mem_record();
                auto start = std::chrono::high_resolution_clock::now();
                #endif
                compress_impl(p_in, p_out);
                p_out.flush();
                #ifdef PERF
                auto end = std::chrono::high_resolution_clock::now();
                MemoryTracker::stop_mem_record();
                m_stats.m_mem_usage = MemoryTracker::max_mem_usage;
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.bytes_written();
                #endif
            }
            
            /**
             * @brief Perform Decompression
             * 
             * @param p_in Encoded Input-Stream
             * @param p_out Raw Output-Stream
             */
            void decompress(Coder::Decoder<Factor> &p_in, StreamView &p_out) {
                #ifdef PERF
                MemoryTracker::start_mem_record();
                auto start = std::chrono::high_resolution_clock::now();
                #endif
                decompress_impl(p_in, p_out);
                #ifdef PERF
                auto end = std::chrono::high_resolution_clock::now();
                MemoryTracker::stop_mem_record();
                m_stats.m_mem_usage = MemoryTracker::max_mem_usage;
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.bytes_written();
                #endif
            }
    };
}