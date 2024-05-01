#include "ApproxLZ77Compressor.hpp"
#include "RabinKarp.hpp"
#include <numeric>
#include <bit>


void ApproxLZ77Compressor::compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) {
    std::span<const char8_t> input_span(p_in.slice_ref(0, p_in.size()));
    if(input_span.size() == 0) return;

    BlockTableSTD block_table(input_span);
    

    while(block_table.block_size > 1) {
        block_table.next_block_table();
        auto first_match_block = block_table.zero_fp();
        RabinKarpFingerprint candidate_fp = RabinKarpFingerprint(first_match_block);

        for(size_t pos = 0; pos < input_span.size() - block_table.block_size; pos++) {
            block_table.match_block(pos, candidate_fp);
            candidate_fp = input_span[pos] << candidate_fp << input_span[pos+block_table.block_size];
        }
    }
    block_table.insert_residuals();

    for(auto it = block_table.factors_begin_iter(); it != block_table.factors_end_iter(); it++) {
        p_out.encode(it->second);
    }
}

void ApproxLZ77Compressor::decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, OutStreamView &p_out){
    
    LZ77::factor_id id;
    while(p_in.decode(id)) {

        if(id.length == 0) {
            if(std::get<char8_t>(id.value) != 0){
                p_out.put(std::get<char8_t>(id.value));
            }
            continue;
        }
        else {

            auto data = p_out.slice_val(std::get<size_t>(id.value), id.length);            
            for(int i = id.length; i>0;){
                if(i >= data.size()){
                    p_out.write(data);
                    i -= data.size();
                }
                else{
                    p_out.write(std::span(data.data(), i));
                    break;
                }
            }
        }
    }
}