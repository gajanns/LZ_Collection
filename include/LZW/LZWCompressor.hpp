#pragma once

#include "Compressor.hpp"
#include "Definition.hpp"

class LZWCompressor : public Compression::Compressor<LZW::factor_id > {
    private:
        void compress_impl(StreamView &p_in, Coder::Encoder<LZW::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<LZW::factor_id> &p_in, StreamView &p_out) override;
};