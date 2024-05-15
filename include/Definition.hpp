#pragma once

#include <stddef.h>
#include <map>
#include <bit>
#include <sstream>
#include "Compressor.hpp"
#include <stdexcept>
#include <variant>
#include <cstdint>


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
    /**
     * @brief Representation of Factor in LZ77-Algorithm(LZSS-Style)
     * 
     */
    struct factor_id{
        std::variant<char8_t, size_t> value;
        size_t length;

        bool operator>>(std::u8string &p_out) {

            std::stringstream ss;
            ss << length << ",";
        
            if(length == 0) {
                ss << reinterpret_cast<char&>(std::get<char8_t>(value));
            }
            else {
                ss << std::get<size_t>(value);
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
                    value = std::stoul(std::string{p_in.begin()+pos+1, p_in.end()});
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

    const size_t min_round = 1;
    const size_t min_block_size = 1;
    const bool dynamic_init = true;

    /**
     * @brief Representation of Factor in Approximate LZ77-Algorithm
     * 
     */
    struct factor_id{
        std::variant<char8_t, size_t> value;
        size_t log_length; // if(0) => length = 0, else => 2^(log_length-1)

        bool operator>>(std::u8string &p_out) {

            size_t length = log_length ? 1<<(log_length-1) : 0;    
            std::stringstream ss;
            ss << length << ",";
        
            if(length == 0) {
                ss << reinterpret_cast<char&>(std::get<char8_t>(value));
            }
            else {
                ss << std::get<size_t>(value);
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

            size_t length = std::stoul(std::string{p_in.begin(), p_in.begin()+pos});
            log_length = length ? std::bit_width(length) : 0;
            
            if(log_length == 0) {
                value = p_in[pos+1];
            }
            else {
                try {
                    value = std::stoul(std::string{p_in.begin()+pos+1, p_in.end()});
                }
                catch(std::invalid_argument &e) {
                    return false;
                }
            }
            return true;
        }
    };
}