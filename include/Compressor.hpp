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
        size_t m_factor_count;
        
        struct SpecializedStats {
            std::vector<std::string> m_field_names;
            std::vector<size_t> m_field_values;
        } m_specialized_stats;

        void dump() {
            std::cout << "Input size: " << m_input_size << std::endl;
            std::cout << "Output size: " << m_output_size << std::endl;
            std::cout << "Compression ratio: " << (double) m_output_size / m_input_size << std::endl;
            std::cout << "Compression time: " << m_run_time_milliseconds << "ms" << std::endl;
            std::cout << "Memory Usage: " << m_mem_usage << " Bytes" << std::endl;
            std::cout << "nFactors: " << m_factor_count << std::endl;
        }
    };

    template <typename Factor>
    class Compressor
    { 
        private:
            virtual void compress_impl(InStreamView &p_in, Coder::Encoder<Factor> &p_out) = 0;
            virtual void decompress_impl(Coder::Decoder<Factor> &p_in, OutStreamView &p_out) = 0;

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
            void compress(InStreamView &p_in, Coder::Encoder<Factor> &p_out) {
                #ifdef PERF
                MemoryTracker::start_mem_record();
                #endif
                auto start = std::chrono::high_resolution_clock::now();

                compress_impl(p_in, p_out);

                auto end = std::chrono::high_resolution_clock::now();
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                
                p_out.flush();
                m_stats.m_input_size = p_in.size();
                m_stats.m_output_size = p_out.bytes_written();
                m_stats.m_factor_count = p_out.factors_written();
                p_in.reset();
                
                
                #ifdef PERF
                MemoryTracker::stop_mem_record();
                m_stats.m_mem_usage = MemoryTracker::peak_mem_usage;
                #endif
            }
            
            /**
             * @brief Perform Decompression
             * 
             * @param p_in Encoded Input-Stream
             * @param p_out Raw Output-Stream
             */
            void decompress(Coder::Decoder<Factor> &p_in, OutStreamView &p_out) {
                #ifdef PERF
                MemoryTracker::start_mem_record();
                auto start = std::chrono::high_resolution_clock::now();
                #endif

                decompress_impl(p_in, p_out);
                
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.size();
                p_out.reset();

                #ifdef PERF
                MemoryTracker::stop_mem_record();
                m_stats.m_mem_usage = MemoryTracker::peak_mem_usage;
                auto end = std::chrono::high_resolution_clock::now();
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                #endif
            }
    };
}