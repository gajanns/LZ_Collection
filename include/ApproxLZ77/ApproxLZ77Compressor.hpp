#pragma once

#include "Definition.hpp"
#include "Compressor.hpp"
#include "RabinKarp.hpp"
#include "BlockTableBasic.hpp"


class ApproxLZ77Compressor : public Compression::Compressor<ApproxLZ77::factor_id> {
    private:
        void compress_impl(InStreamView &p_in, Coder::Encoder<ApproxLZ77::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<ApproxLZ77::factor_id> &p_in, OutStreamView &p_out) override;

        ApproxLZ77Compressor() = default;
        ApproxLZ77Compressor(const ApproxLZ77Compressor&) = delete;
        ApproxLZ77Compressor& operator=(const ApproxLZ77Compressor&) = delete;
        
    public:
        static ApproxLZ77Compressor& Instance() {
            static ApproxLZ77Compressor instance;
            return instance;
        }
};
