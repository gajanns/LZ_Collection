#pragma once

#include <cstdint>


namespace LZW
{
    /**
     * @brief Representation of Factor in LZW-Algorithm
     * 
     */
    using factor_id = int32_t;
}

namespace LZ77
{
    /**
     * @brief Representation of Factor in LZ77-Algorithm
     * 
     */
    struct factor_id{
        int32_t offset;
        int32_t length;
        unsigned char next_char;
    };
}