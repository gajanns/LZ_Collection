#pragma once

#include <stddef.h>
#include <map>
#include <variant>
#include <cstdint>

enum Algorithm {lzw, lz77, appr77seq, appr77par, all};
const std::map<std::string, Algorithm> algorithm_from_name{{"LZW", lzw}, {"LZ77", lz77}, {"Approx.LZ77", appr77seq}, {"Approx.LZ77Par", appr77par}};
const std::map<Algorithm, std::string> algorithm_to_name{{lzw, "LZW"}, {lz77, "LZ77"}, {appr77seq, "Approx.LZ77"}, {appr77par, "Approx.LZ77Par"}}; 

template<typename T>
concept NumRange = requires { std::ranges::bidirectional_range<T> && std::is_integral_v<typename T::value_type>; };

namespace LZW
{
    /**
     * @brief Representation of Factor in LZW-Algorithm
     * 
     */
    using factor_id = int32_t;

    const size_t initial_dict_size = 256;
}

namespace LZ77_Wndw
{
    /**
     * @brief Representation of Factor in LZ77-Algorithm
     * 
     */
    struct factor_id{
        int32_t offset;
        int32_t length;
        char next_char;
    };

    const int32_t search_buffer_size = 63;
    const int32_t preview_buffer_size = 63;
    const int32_t window_size = search_buffer_size + preview_buffer_size;
}

namespace LZ77
{
    /**
     * @brief Distinction between data(literal) and reference factor
     * 
    */
    enum class FactorType:bool {data, ref};

    /**
     * @brief Representation of Factor in LZ77-Algorithm(LZSS-Style)
     * 
     */
    struct factor_id{
        FactorType type;
        std::variant<char, size_t> value;
        size_t length;
    };
}

/**
 * @brief Extract number of bit needed to express value
 * 
 * @param p_in Numeric value
 * @return size_t Number of bits
 */
inline size_t log2(auto p_in){
    size_t res = 1, tmp = p_in;
    while(tmp >>= 1) res++;
    return res;
}