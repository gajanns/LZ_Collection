#include "ApproxLZ77Compressor.hpp"
#include "RabinKarp.hpp"
#include <numeric>
#include <bit>

void ApproxLZ77Compressor::fill_block_table(std::span<const char8_t> &p_input, std::vector<size_t> &p_blocks, size_t p_round){
    size_t sentinel_count = std::bit_ceil(p_input.size())-p_input.size();
    size_t block_size = (p_input.size() + sentinel_count) >> p_round;

    block_table.clear();
    block_table.reserve(p_blocks.size());

    for(auto p: p_blocks){
        std::span<const char8_t> block;
        if(p*block_size >= p_input.size()){
            block = std::span<const char8_t>();
        }
        else if(p*block_size + block_size > p_input.size()){
            block = p_input.subspan(p*block_size, p_input.size()-p*block_size);
        }
        else {
            block = p_input.subspan(p*block_size, block_size);
        }
        RKF block_hash(block, block_size - block.size());
        block_table[block_hash].push_back(p);
    }
}

void ApproxLZ77Compressor::compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) {
    std::span<const char8_t> input_span(p_in.slice_ref(0, p_in.size()));
    // ToDo: Implement Approx. LZ77 compression
    size_t sentinel_count = input_span.size() ? std::bit_ceil(input_span.size())-input_span.size(): 0;
    size_t block_size = input_span.size() + sentinel_count;
    std::vector<size_t> unmarked_blocks = {0};

    for(size_t round = 1; block_size > 1; ++round){
        block_size >>= 1;
        std::vector<size_t> new_unmarked_blocks;
        new_unmarked_blocks.reserve(unmarked_blocks.size()*2);
        for(auto p: unmarked_blocks){
            new_unmarked_blocks.push_back(2*p);
            new_unmarked_blocks.push_back(2*p+1);
        }
        unmarked_blocks = new_unmarked_blocks;

        fill_block_table(input_span, unmarked_blocks, round);

        auto block_data = input_span.subspan(0, std::min(block_size, input_span.size()));
        RKF block_hash(block_data, block_size - block_data.size());
        
        for(size_t i = 0; i < input_span.size()-block_size; i++) {
            auto candidates = block_table.find(block_hash);
            if(candidates != block_table.end()){
                for(auto p: candidates->second){
                    if(p*block_size <= i || std::find(unmarked_blocks.begin(), unmarked_blocks.end(), p) == unmarked_blocks.end()){
                        continue;
                    }

                    bool is_match = std::equal(input_span.begin()+i, input_span.begin() + i + block_size, input_span.begin()+p*block_size);
                    if(is_match){
                        factor_chain.insert({p*block_size, LZ77::factor_id{.value = i, .length = block_size}});
                        auto it = std::find(unmarked_blocks.begin(), unmarked_blocks.end(), p);
                        if(it != unmarked_blocks.end()){
                            unmarked_blocks.erase(it);
                        }
                    }
                }
            }
            block_hash = input_span[i] << block_hash << input_span[i+block_size];
        }
    }

    for(auto p: unmarked_blocks) {
        if(input_span.size() == 0 || input_span[p*block_size] == 0) break;
        factor_chain.insert({p*block_size, LZ77::factor_id{.value = input_span[p*block_size], .length = 0}});
    }

    for(auto p: factor_chain){
        p_out.encode(p.second);
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