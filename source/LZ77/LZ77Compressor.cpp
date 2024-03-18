#include "LZ77Compressor.hpp"

void LZ77Compressor::compress_impl(StreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) {
    std::string window;
    size_t window_offset = 0;
    int32_t pos = 0;
    LZ77::factor_id id;
    while(size_t x = p_in.extractSlice(window, std::max(pos - LZ77::search_buffer_size, 0), LZ77::window_size+std::min(pos - LZ77::search_buffer_size, 0))){
        auto search_size = LZ77::search_buffer_size - std::max(LZ77::search_buffer_size - pos, 0);
        if(search_size >= x) break;
        auto preview_size = x - search_size;
        LZ77::factor_id id = {.offset = 0, .length = 0, .next_char = window[search_size]};

        // Search for the longest match in the search buffer
        for (int offset = 1; offset <= search_size; offset++)
        {
            int len = 0;
            while (len < preview_size && window[search_size - offset + len] == window[search_size + len])
            {
                len++;
            }
          
            // Update the token if a longer match is found
            if (len > id.length)
            {
                id.offset = offset;
                id.length = len;
                id.next_char = window[search_size+len];
            }
        }
        p_out.encode(id);
        pos += id.length + 1;
    }
}

void LZ77Compressor::decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, StreamView &p_out){}