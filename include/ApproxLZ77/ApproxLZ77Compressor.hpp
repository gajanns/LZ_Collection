#pragma once

#include "Definition.hpp"
#include "Compressor.hpp"


class ApproxLZ77Compressor : public Compression::Compressor<LZ77::factor_id> {
    private:
        void compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, OutStreamView &p_out) override;
};
