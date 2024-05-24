#include "ApproxLZ77ParCompressor.hpp"
#include "RabinKarp.hpp"
#include "Definition.hpp"
#include <numeric>
#include <bit>
#include <list>
#include <omp.h>


void ApproxLZ77ParCompressor::compress_impl(InStreamView &p_in, Coder::Encoder<ApproxLZ77::factor_id> &p_out) {

    std::span<const char8_t> input_span = p_in.slice_ref(0, p_in.size());
    if(input_span.size() == 0) return;

    BlockTablePar block_table(input_span);
    std::set<CherryNode> chain_ids;
    std::set<BlockRef> marked_refs;
    std::vector<BlockNode> unmarked_nodes;
    std::unordered_map<u_int32_t, std::list<UniqueBlockGroup>> fp_table;

    size_t in_size = std::bit_ceil(input_span.size()), in_log_size = std::bit_width(in_size) - 1;
    size_t min_round = std::min(in_log_size, ApproxLZ77::min_round), max_round = in_log_size - std::bit_width(ApproxLZ77::min_block_size) + 1;

    size_t round = min_round;

    auto match_nodes = [&](size_t p_round, bool p_capture_refs = true) {
        size_t block_size = in_size >> p_round;
        size_t data_per_thread = (input_span.size() - block_size + num_threads -1) / num_threads;
        block_table.create_fp_table(fp_table, unmarked_nodes, p_round);

        #pragma omp parallel
        {
            size_t thread_id = omp_get_thread_num();
            size_t start_pos = thread_id * data_per_thread;
            size_t end_pos = std::min(start_pos + data_per_thread, input_span.size() - block_size);
            
            RabinKarpFingerprint test_fp;
            if(start_pos < end_pos) {
                test_fp = RabinKarpFingerprint(std::span<const char8_t>(input_span.data() + start_pos, block_size));
            }

            for(size_t pos = start_pos; pos < end_pos; pos++) {
                block_table.preprocess_matches(pos, test_fp.val, fp_table, p_round);
                test_fp.shift_right(input_span[pos], input_span[pos + block_size]);
            }           
        }
        block_table.postprocess_matches(fp_table, round, p_capture_refs ? &marked_refs : nullptr);
        
        return 1;
    };

    auto init_nodes = [&](bool dynamic_init) {
        unmarked_nodes = block_table.init_nodes(round);
        if(ApproxLZ77::dynamic_init) {
            size_t probe_round = (min_round + max_round) / 2;
            size_t probe_block_size = in_size >> probe_round;
            match_nodes(probe_round, false);

            size_t max_consecutive = 0, cur_consecutive = 0;
            for(auto it = unmarked_nodes.begin(); it != unmarked_nodes.end(); it++) {
                if(it->chain_info & probe_block_size) {
                    cur_consecutive++;
                }
                else {
                    max_consecutive = std::max(max_consecutive, cur_consecutive);
                    cur_consecutive = 0;
                }
            }
            max_consecutive = std::max(max_consecutive, cur_consecutive);
            int diff_round = std::bit_width(max_consecutive) - 1;
            if(diff_round >= 0) {
                block_table.previous_nodes(unmarked_nodes, diff_round);
                round = probe_round - diff_round;
            }
            else {
                round = probe_round + 1;
                unmarked_nodes = block_table.init_nodes(round);
            } 
        }
        return 0;
    };
    
    auto process_round = [&]() {
        match_nodes(round);
        if(round == max_round) return 0;
        unmarked_nodes = block_table.next_nodes(unmarked_nodes, chain_ids, round);
        return 1;
    };   

    auto push_factors = [&]() {
        auto it_ref = marked_refs.begin();
        size_t cur_pos = 0;
        bool is_chain_up = false;

        for(auto it_chain = chain_ids.begin(); it_chain != chain_ids.end(); it_chain++) {
            size_t chain = it_chain -> chain_info;
            int bit_pos = is_chain_up ? 0 : in_log_size;
            int bit_dir = is_chain_up ? 1 : -1;
            int bit_end = is_chain_up ? in_log_size + 1 : -1;

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
        return 0;
    };    


    // Execute Algorithm
    init_nodes(ApproxLZ77::dynamic_init);
    while(round <= max_round) {
        if(process_round()) round++;
        else break;
    }
    block_table.populate_unmarked_chain(unmarked_nodes, chain_ids, round);
    push_factors();
}

void ApproxLZ77ParCompressor::decompress_impl(Coder::Decoder<ApproxLZ77::factor_id> &p_in, OutStreamView &p_out){
    
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