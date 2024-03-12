#pragma once

#include "Compressor.hpp"
#include "Factor.hpp"

class LZWCompressor : public Compression::Compressor<std::string, LZW::factor > {
    private:
        void compress_impl(Coder::Decoder<std::string> &p_in, Coder::Encoder<LZW::factor> &p_out) override;
        void decompress_impl(Coder::Decoder<LZW::factor> &p_in, Coder::Encoder<std::string> &p_out) override;
};