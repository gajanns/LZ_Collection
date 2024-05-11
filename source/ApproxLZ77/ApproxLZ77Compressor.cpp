#include "ApproxLZ77Compressor.hpp"
#include "RabinKarp.hpp"
#include <numeric>
#include <bit>
#include <list>



void ApproxLZ77Compressor::compress_impl(InStreamView &p_in, Coder::Encoder<ApproxLZ77::factor_id> &p_out) {
    std::span<const char8_t> input_span = p_in.slice_ref(0, p_in.size());
    if(input_span.size() == 0) return;

    size_t round = ApproxLZ77::min_round, block_size = std::bit_ceil(input_span.size()) >> round, log_block_size = std::bit_width(block_size)-1;
    BlockTableBasic block_table(input_span);
    std::set<CherryNode> chain_ids;
    std::set<BlockRef> marked_refs;

    auto unmarked_nodes = block_table.init_nodes(round);
    std::unordered_map<size_t, std::list<BlockNode*>> fp_table;

    while(block_size >= ApproxLZ77::min_block_size) {
        block_table.create_fp_table(fp_table, unmarked_nodes, round);
        RabinKarpFingerprint test_fp = unmarked_nodes[0].fp;

        for(size_t pos = 0; pos < input_span.size() - block_size; pos++) {
            block_table.match_blocks(pos, test_fp.val, fp_table, marked_refs, round);
            test_fp = input_span[pos] << test_fp << input_span[pos+block_size];
        }

        if(block_size == ApproxLZ77::min_block_size) break;

        unmarked_nodes = block_table.next_nodes(unmarked_nodes, chain_ids, round);
        round++;
        block_size >>= 1;
    }

    block_table.populate_unmarked_chain(unmarked_nodes, chain_ids, round);

    // Combine chain_ids and marked_refs into Factors to be encoded
    auto it_ref = marked_refs.begin();
    size_t cur_pos = 0;
    bool is_chain_up = false;

    for(auto it_chain = chain_ids.begin(); it_chain != chain_ids.end(); it_chain++) {
        size_t chain = it_chain -> chain_info;
        int bit_pos = is_chain_up ? 0 : log_block_size;
        int bit_dir = is_chain_up ? 1 : -1;
        int bit_end = is_chain_up ? log_block_size + 1 : -1;

        while(bit_pos != bit_end) {
            while(!(chain & (1 << bit_pos)) && bit_pos != bit_end) {
                bit_pos += bit_dir;
            }
            if(bit_pos == bit_end) break;

            size_t next_length = 1 << bit_pos;

            if(it_ref != marked_refs.end() && it_ref->block_position == cur_pos) {
                p_out.encode(ApproxLZ77::factor_id{.value = it_ref->ref_position, .log_length = static_cast<size_t>(bit_pos+1)});
                cur_pos += next_length;
                it_ref++;
            }
            else {
                for(size_t i = 0; i < next_length; i++) {
                    p_out.encode(ApproxLZ77::factor_id{.value = input_span[cur_pos++], .log_length = 0});
                }
            }
            bit_pos += bit_dir;
        }
        is_chain_up = !is_chain_up;
    }
}

void ApproxLZ77Compressor::decompress_impl(Coder::Decoder<ApproxLZ77::factor_id> &p_in, OutStreamView &p_out){
    
    ApproxLZ77::factor_id id;
    while(p_in.decode(id)) {

        if(id.log_length == 0) {
            if(std::get<char8_t>(id.value) != 0) {
                p_out.put(std::get<char8_t>(id.value));
            }
            continue;
        }
        else {
            size_t length = 1<<(id.log_length-1);
            auto data = p_out.slice_val(std::get<size_t>(id.value), length);            
            for(int i = length; i>0;) {
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