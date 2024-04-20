#pragma once

#include <stddef.h>
#include <map>
#include <stdexcept>
#include <variant>
#include <cstdint>


enum Algorithm {lzw, lz77, appr77seq, appr77par, all};
const std::map<std::string, Algorithm> algorithm_from_name{{"LZW", lzw}, {"LZ77", lz77}, {"Approx.LZ77", appr77seq}, {"Approx.LZ77Par", appr77par}};
const std::map<Algorithm, std::string> algorithm_to_name{{lzw, "LZW"}, {lz77, "LZ77"}, {appr77seq, "Approx.LZ77"}, {appr77par, "Approx.LZ77Par"}}; 

template<typename T>
concept NumRange = std::ranges::range<T> && std::is_integral_v<std::ranges::range_value_t<T>>;

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
            std::basic_stringstream<char8_t> ss;
            ss << value;
            p_out += ss.str();
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

            std::basic_stringstream<char8_t> ss;
            ss << length << u8",";

            if(length == 0) {
                ss << std::get<char8_t>(value);
            }
            else {
                ss << std::get<size_t>(value);
            }
            p_out += ss.str();
            return true;
        }

        bool operator<<(std::u8string &p_in) {
            
            auto pos = p_in.find(u8',');
            if(pos == std::string::npos) {
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