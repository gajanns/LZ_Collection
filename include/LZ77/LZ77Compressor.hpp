#pragma once

#include "Compressor.hpp"
#include "Definition.hpp"

/**
 * @brief Compressor for "windowed" LZ77-Algorithm.(ToDo: exact/windowless LZ77-Algorithm)
 * 
 */
class LZ77Compressor : public Compression::Compressor<LZ77_Wndw::factor_id> {
    private:
        void compress_impl(StreamView &p_in, Coder::Encoder<LZ77_Wndw::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<LZ77_Wndw::factor_id> &p_in, StreamView &p_out) override;
};