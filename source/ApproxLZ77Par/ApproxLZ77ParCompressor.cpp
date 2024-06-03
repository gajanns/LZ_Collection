#include "ApproxLZ77ParCompressor.hpp"
#include "RabinKarp.hpp"
#include <numeric>
#include <bit>
#include <list>
#include "unordered_dense.h"
#include <omp.h>
#include <execution>

using namespace ApproxLZ77;
using namespace ApproxLZ77Par;

void ApproxLZ77ParCompressor::compress_impl(InStreamView &p_in, Coder::Encoder<ApproxLZ77::factor_id> &p_out) {

    std::span<const char8_t> input_span = p_in.slice_ref(0, p_in.size());
    if(input_span.size() == 0) return;

    BlockTablePar block_table(input_span);
    std::vector<CherryNode> chain_ids;
    std::vector<BlockRef> marked_refs;
    std::vector<BlockNode> unmarked_nodes;

    ankerl::unordered_dense::map<size_t, u_int32_t> fp_table;

    const size_t in_size = std::bit_ceil(input_span.size()), in_log_size = std::bit_width(in_size) - 1;
    const size_t min_round = std::min(in_log_size, ApproxLZ77::min_round), max_round = in_log_size - std::bit_width(ApproxLZ77::min_block_size) + 1;
    const int min_block_log_size = std::bit_width(ApproxLZ77::min_block_size) - 1;
    size_t max_block_log_size;
    size_t round = min_round;

    auto match_nodes = [&](size_t p_round, bool p_capture_refs = true, bool log_time = false) {
        auto start = std::chrono::high_resolution_clock::now();
        block_table.create_fp_table(fp_table, unmarked_nodes, p_round, p_capture_refs ? &marked_refs : nullptr);
        auto end = std::chrono::high_resolution_clock::now();
        if(log_time) {
            std::cout << "FP Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms ... ";
            start = std::chrono::high_resolution_clock::now();
        }
        
        size_t block_size = in_size >> p_round;        
        auto [chunk_size, chunked_input] = chunks(input_span, num_threads, block_size - 1);

        #pragma omp parallel for
        for(size_t chunk_id = 0; chunk_id < chunked_input.size(); chunk_id++) {
            auto & chunk_data = chunked_input[chunk_id];
            RabinKarpFingerprint test_fp = RabinKarpFingerprint(chunk_data | std::views::take(block_size));

            for(size_t pos = 0; pos <= chunk_data.size() - block_size; pos++) {
                block_table.preprocess_matches(chunk_id * chunk_size + pos, test_fp.val, fp_table);
                test_fp.shift_right(chunk_data[pos], chunk_data[pos + block_size]);
            }
        }

        if(log_time) {
            end = std::chrono::high_resolution_clock::now();
            std::cout << "Pre-Match Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms ... ";
            start = std::chrono::high_resolution_clock::now();
        }

        if(p_capture_refs) block_table.postprocess_matches(unmarked_nodes, fp_table, p_round, marked_refs);
        else block_table.postprocess_matches(unmarked_nodes, fp_table, p_round);

        if(log_time) {
            end = std::chrono::high_resolution_clock::now();
            std::cout << "Post-Match Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
        }
        return 1;
    };

    auto init_nodes = [&](bool dynamic_init) {
       
        if(dynamic_init) {
            const size_t probe_round = (min_round + max_round) / 2;

            const int diff_round = [&]() {                
                const size_t probe_block_size = in_size >> probe_round;

                unmarked_nodes = block_table.init_nodes(probe_round);
                match_nodes(probe_round, false);
                size_t max_consecutive = 0, cur_consecutive = 0;
                for(auto &node : unmarked_nodes | std::views::drop(1)) {
                    if(node.chain_info & probe_block_size) {
                        cur_consecutive++;
                    }
                    else {
                        max_consecutive = std::max(max_consecutive, cur_consecutive);
                        cur_consecutive = 0;
                    }
                }
                max_consecutive = std::max(max_consecutive, cur_consecutive);
                return std::bit_width(max_consecutive) - 1;
            }();
            
            if(diff_round >= 0) {
                block_table.previous_nodes(unmarked_nodes, diff_round);
                round = probe_round - diff_round;
            }
            else {
                round = probe_round + 1;
                unmarked_nodes = block_table.init_nodes(round);
            } 
        }
        else {
            unmarked_nodes = block_table.init_nodes(round);
        }
        max_block_log_size = in_log_size - round;
        return 0;
    };

    auto extract_factor_log_sizes = [&]() {
        std::vector<u_int8_t> factor_log_sizes;
        size_t chains_per_chunk = (chain_ids.size() + ApproxLZ77Par::num_threads - 1) / ApproxLZ77Par::num_threads;

        auto [chunk_size, chunked_ids] = chunks(chain_ids, ApproxLZ77Par::num_threads);

        #pragma omp parallel for ordered
        for(size_t chunk_id = 0; chunk_id < chunked_ids.size(); chunk_id++) {
            auto & chunk = chunked_ids[chunk_id];
            std::vector<u_int8_t> chain_sizes;
            
            bool is_chain_up = (chunk_size * chunk_id) % 2;
            for(auto cherry_node : chunk) {
                int bit_pos = is_chain_up ? min_block_log_size : max_block_log_size;
                int bit_dir = is_chain_up ? 1 : -1;
                int bit_end = is_chain_up ? max_block_log_size + 1 : min_block_log_size - 1;

                while(bit_pos != bit_end) {
                    while(bit_pos != bit_end && !(cherry_node.chain_info & (1 << bit_pos)) ) {
                        bit_pos += bit_dir;
                    }
                    if(bit_pos == bit_end) break;
                    chain_sizes.push_back(static_cast<u_int8_t>(bit_pos));
                    bit_pos += bit_dir;
                }
                is_chain_up = !is_chain_up;
            }

            #pragma omp ordered
            factor_log_sizes.insert(factor_log_sizes.end(), chain_sizes.begin(), chain_sizes.end());
        }
        return factor_log_sizes;
    };
    
    auto process_round = [&](bool log_time = false) {
        auto start = std::chrono::high_resolution_clock::now();
        match_nodes(round);
        auto end = std::chrono::high_resolution_clock::now();
        if(log_time) {
            std::cout << "R" << round << ":Match Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms ... ";
            start = std::chrono::high_resolution_clock::now();
        }
        if(round == max_round) return 0;
        unmarked_nodes = block_table.next_nodes(unmarked_nodes, chain_ids, round);
        if(log_time) {
            end = std::chrono::high_resolution_clock::now();
            std::cout << "Next Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
        }
        return 1;
    };   

    auto push_factors = [&](bool log_time = false) {
        auto start = std::chrono::high_resolution_clock::now();
        std::sort(std::execution::par, marked_refs.begin(), marked_refs.end());
        std::sort(std::execution::par, chain_ids.begin(), chain_ids.end());
        auto end = std::chrono::high_resolution_clock::now();

        if(log_time) {
            std::cout << "Sort Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms ... ";
            start = std::chrono::high_resolution_clock::now();
        }

        auto factor_log_sizes = extract_factor_log_sizes();

        if(log_time) {
            end = std::chrono::high_resolution_clock::now();
            std::cout << "Extract Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms ... ";
            start = std::chrono::high_resolution_clock::now();
        }

        for(auto [it_ref, cur_pos] = std::pair{marked_refs.begin(), size_t{0}}; auto log_size : factor_log_sizes) {
            if(it_ref != marked_refs.end() && it_ref->block_position == cur_pos) {
                p_out.encode(ApproxLZ77::factor_id{.value = it_ref->ref_position, .log_length = static_cast<size_t>(log_size+1)});
                cur_pos += 1 << log_size;
                it_ref++;
            }
            else {
                for(size_t i = 0; i < (1 << log_size); i++) {
                    p_out.encode(ApproxLZ77::factor_id{.value = input_span[cur_pos++], .log_length = 0});
                }
            }
        }

        if(log_time) {
            end = std::chrono::high_resolution_clock::now();
            std::cout << "Internal Push Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms ... ";
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