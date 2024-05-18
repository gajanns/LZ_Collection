#pragma once

#include "Definition.hpp"
#include "Compressor.hpp"
#include "RabinKarp.hpp"
#include "BlockTablePar.hpp"


class ApproxLZ77ParCompressor : public Compression::Compressor<ApproxLZ77::factor_id> {
    private:
        void compress_impl(InStreamView &p_in, Coder::Encoder<ApproxLZ77::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<ApproxLZ77::factor_id> &p_in, OutStreamView &p_out) override;

        ApproxLZ77ParCompressor() = default;
        ApproxLZ77ParCompressor(const ApproxLZ77ParCompressor&) = delete;
        ApproxLZ77ParCompressor& operator=(const ApproxLZ77ParCompressor&) = delete;
        
    public:
        static ApproxLZ77ParCompressor& Instance() {
            static ApproxLZ77ParCompressor instance;
            return instance;
        }
};
