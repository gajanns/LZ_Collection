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

        bool operator>>(std::string &p_out) {
            p_out += std::to_string(value);
            return true;
        }

        bool operator<<(std::string &p_in) {
            try {
                value = std::stoi(p_in);
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
        std::variant<char, size_t> value;
        size_t length;

        bool operator>>(std::string &p_out) {
            p_out += std::to_string(length) + ",";

            if(length == 0) {
                p_out += std::get<char>(value);
            }
            else {
                p_out += std::to_string(std::get<size_t>(value));
            }
            return true;
        }

        bool operator<<(std::string &p_in) {
            std::string_view tmp{p_in};

            auto pos = tmp.find(',');
            if(pos == std::string::npos) {
                return false;
            }

            length = std::stoul(std::string{tmp.substr(0, pos)});
            tmp.remove_prefix(pos+1);

            if(length == 0) {
                value = tmp[0];
            }
            else {
                try {
                    value = std::stoul(std::string{tmp});
                }
                catch(std::invalid_argument &e) {
                    return false;
                }
            }
            return true;
        }
    };
}