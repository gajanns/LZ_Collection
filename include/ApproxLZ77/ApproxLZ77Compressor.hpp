#pragma once

#include "Definition.hpp"
#include "Compressor.hpp"
#include "RabinKarp.hpp"
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <span>




class ApproxLZ77Compressor : public Compression::Compressor<LZ77::factor_id> {

    using RKF = RabinKarpFingerprint;

    private:
        struct cmp {
            bool operator()(const std::pair<size_t, LZ77::factor_id> &a, const std::pair<size_t, LZ77::factor_id> &b) const {
                return a.first < b.first;
            }
        };
        std::set<std::pair<size_t, LZ77::factor_id>, cmp> factor_chain;
        std::unordered_map<RKF, std::vector<size_t>, RKF::HashOp, RKF::EqualOp> block_table;


        void compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) override;
        void decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, OutStreamView &p_out) override;

        void fill_block_table(std::span<const char8_t> &p_input, std::unordered_set<size_t> &p_blocks, size_t p_round);
};
