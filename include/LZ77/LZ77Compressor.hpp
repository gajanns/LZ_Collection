#pragma once

#include "Compressor.hpp"
#include "Definition.hpp"

/**
 * @brief Compressor for windowless LZ77-Algorithm
 * 
 */
class LZ77Compressor : public Compression::Compressor<LZ77::factor_id> {
    private:
        void compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, OutStreamView &p_out) override;

        LZ77Compressor() = default;
        LZ77Compressor(const LZ77Compressor&) = delete;
        LZ77Compressor& operator=(const LZ77Compressor&) = delete;
        
    public:
        static LZ77Compressor& Instance() {
            static LZ77Compressor instance;
            return instance;
        }
};