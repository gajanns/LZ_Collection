#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <chrono>
#include "Coder.hpp"

namespace Compression {
    struct CompressionStatistics {
        size_t m_input_size;
        size_t m_output_size;
        size_t m_run_time_milliseconds;

        void dump() {
            std::cout << "Input size: " << m_input_size << std::endl;
            std::cout << "Output size: " << m_output_size << std::endl;
            std::cout << "Compression ratio: " << (double) m_output_size / m_input_size << std::endl;
            std::cout << "Compression time: " << m_run_time_milliseconds << "ms" << std::endl;
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
                auto start = std::chrono::high_resolution_clock::now();
                compress_impl(p_in, p_out);
                p_out.flush();
                auto end = std::chrono::high_resolution_clock::now();
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.bytes_written();
            }
            
            inline void decompress(Coder::Decoder<OUT> &p_in, Coder::Encoder<IN> &p_out) {
                auto start = std::chrono::high_resolution_clock::now();
                decompress_impl(p_in, p_out);
                p_out.flush();
                auto end = std::chrono::high_resolution_clock::now();
                m_stats.m_run_time_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_stats.m_input_size = p_in.bytes_read();
                m_stats.m_output_size = p_out.bytes_written();
            }
    };
}

#endif