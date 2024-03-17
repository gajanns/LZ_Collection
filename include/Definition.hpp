#pragma once

#include <stddef.h>
#include <cstdint>


namespace LZW
{
    /**
     * @brief Representation of Factor in LZW-Algorithm
     * 
     */
    using factor_id = int32_t;

    const size_t initial_dict_size = 256;
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

    const size_t search_buffer_size = 32;
    const size_t preview_buffer_size = 32;
    const size_t window_size = search_buffer_size + preview_buffer_size;
}