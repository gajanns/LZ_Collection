#pragma once

#include <stddef.h>
#include <map>
#include <bit>
#include <sstream>
#include "Compressor.hpp"
#include <stdexcept>
#include <variant>
#include <cstdint>
#include <omp.h>



#define DYNAMIC_GRANULARITY true


/**
 * @brief Concept for Integral-Range
 * 
 */
template<typename T>
concept NumRange = std::ranges::range<T> && std::is_integral_v<std::ranges::range_value_t<T>>;

template<typename T>
concept ByteRange = std::ranges::range<T> && std::is_unsigned_v<std::ranges::range_value_t<T>> && sizeof(std::ranges::range_value_t<T>) == 1;

namespace LZW
{
    /**
     * @brief Representation of Factor in LZW-Algorithm
     * 
     */
    struct factor_id{
        int32_t value;

        bool operator==(const factor_id &p_rhs) const {
            return value == p_rhs.value;
        }

        bool operator==(const int32_t &p_rhs) const {
            return value == p_rhs;
        }

        bool operator>>(std::u8string &p_out) {
            
            std::string tmp = std::to_string(value);
            p_out += std::u8string{tmp.begin(), tmp.end()};
            return true;
        }

        bool operator<<(std::u8string &p_in) {
            try {
                value = std::stoi(std::string{p_in.begin(), p_in.end()});
                return true;
            }
            catch(std::invalid_argument &e) {
                return false;
            }
        }
    };

    const size_t alphabet_size = 256;
}

namespace LZ77
{
    #if DYNAMIC_GRANULARITY == true
        inline size_t min_ref_size = 1;
    #else
        const size_t min_ref_size = 1;
    #endif
    /**
     * @brief Representation of Factor in LZ77-Algorithm(LZSS-Style)
     * 
     */
    struct factor_id{
        std::variant<char8_t, u_int32_t> value;
        u_int32_t length;

        bool operator>>(std::u8string &p_out) {

            std::stringstream ss;
            ss << length << ",";
        
            if(length == 0) {
                ss << reinterpret_cast<char&>(std::get<char8_t>(value));
            }
            else {
                ss << std::get<u_int32_t>(value);
            }
            std::string tmp = ss.str();
            p_out += std::u8string{tmp.begin(), tmp.end()};
            return true;
        }

        bool operator<<(std::u8string &p_in) {
            
            auto pos = p_in.find(',');
            if(pos == std::u8string::npos) {
                return false;
            }

            length = std::stoul(std::string{p_in.begin(), p_in.begin()+pos});
            
            if(length == 0) {
                value = p_in[pos+1];
            }
            else {
                try {
                    value = static_cast<u_int32_t>(std::stoul(std::string{p_in.begin()+pos+1, p_in.end()}));
                }
                catch(std::invalid_argument &e) {
                    return false;
                }
            }
            return true;
        }
    };
}

namespace ApproxLZ77 {
    const bool dynamic_init = true;
    const size_t min_round = 1;

    #if DYNAMIC_GRANULARITY == true
       inline size_t min_block_size = 1;
    #else
        const size_t min_block_size = 1;
    #endif

    const bool precompute = true;

    /**
     * @brief Representation of Factor in Approximate LZ77-Algorithm
     * 
     */
    struct factor_id{
        std::variant<char8_t, u_int32_t> value;
        u_int8_t log_length; // if(0) => length = 0, else => 2^(log_length-1)

        bool operator>>(std::u8string &p_out) {

            u_int32_t length = log_length ? 1<<(log_length-1) : 0;    
            std::stringstream ss;
            ss << length << ",";
        
            if(length == 0) {
                ss << reinterpret_cast<char&>(std::get<char8_t>(value));
            }
            else {
                ss << std::get<u_int32_t>(value);
            }
            std::string tmp = ss.str();
            p_out += std::u8string{tmp.begin(), tmp.end()};
            return true;
        }

        bool operator<<(std::u8string &p_in) {
            
            auto pos = p_in.find(',');
            if(pos == std::u8string::npos) {
                return false;
            }

            u_int32_t length = std::stoul(std::string{p_in.begin(), p_in.begin()+pos});
            log_length = length ? std::bit_width(length) : 0;
            
            if(log_length == 0) {
                value = p_in[pos+1];
            }
            else {
                try {
                    value = static_cast<u_int32_t>(std::stoul(std::string{p_in.begin()+pos+1, p_in.end()}));
                }
                catch(std::invalid_argument &e) {
                    return false;
                }
            }
            return true;
        }
    };
}

namespace ApproxLZ77Par {
    inline size_t num_threads = omp_get_max_threads();
    inline size_t num_thread_mask = std::bit_floor(num_threads) - 1;

    template<typename Sequence, typename Item = typename Sequence::value_type>
    std::pair<size_t, std::vector<std::span<const Item>>> chunks(const Sequence &p_data, size_t p_num_chunks, size_t p_overlap = 0) {
        size_t data_per_chunk = (p_data.size() + p_num_chunks - 1) / p_num_chunks;
        size_t num_chunks_trimmed = (p_data.size() + data_per_chunk - 1) / data_per_chunk;

        std::vector<std::span<const Item>> tmp_chunks(num_chunks_trimmed);
        for(size_t i = 0; i < num_chunks_trimmed; i++) {
            size_t start_pos = i * data_per_chunk;
            size_t end_pos = std::min(start_pos + data_per_chunk + p_overlap, p_data.size());
            tmp_chunks[i] = std::span<const Item>(p_data.begin() + start_pos, p_data.begin() + end_pos);
        }
        return {data_per_chunk, tmp_chunks};
    }
}