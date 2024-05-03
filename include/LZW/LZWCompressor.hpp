#pragma once

#include "Compressor.hpp"
#include "Definition.hpp"

/**
 * @brief Compressor using LZW-Algorithm
 * 
 */
class LZWCompressor : public Compression::Compressor<LZW::factor_id > {
    private:
        void compress_impl(InStreamView &p_in, Coder::Encoder<LZW::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<LZW::factor_id> &p_in, OutStreamView &p_out) override;

        LZWCompressor() = default;
        LZWCompressor(const LZWCompressor&) = delete;
        LZWCompressor& operator=(const LZWCompressor&) = delete;

    public:
        static LZWCompressor& Instance() {
            static LZWCompressor instance;
            return instance;
        }
};