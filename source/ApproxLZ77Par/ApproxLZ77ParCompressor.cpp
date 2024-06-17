#include "ApproxLZ77ParCompressor.hpp"
#include "RabinKarp.hpp"
#include <numeric>
#include <bit>
#include <list>
#include "unordered_dense.h"
#include <omp.h>
#include <execution>
#include "sharded_map.hpp"

using namespace ApproxLZ77;
using namespace ApproxLZ77Par;
using namespace sharded_map;


void ApproxLZ77ParCompressor::compress_impl(InStreamView &p_in, Coder::Encoder<ApproxLZ77::factor_id> &p_out) {
    enum {Init_Time, Round_Time, Factor_Time};
    m_stats.m_specialized_stats.m_field_names = {"init_time", "round_time", "factor_time"};
    m_stats.m_specialized_stats.m_field_values.resize(3, 0);

    const std::span<const char8_t> input_span = p_in.slice_ref(0, p_in.size());
    if(input_span.size() == 0) return;

    BlockTablePar block_table(input_span);
    std::vector<CherryNode> chain_ids;
    std::vector<BlockRef> marked_refs;
    std::vector<BlockNode> unmarked_nodes;

    const size_t in_size = std::bit_ceil(input_span.size()), in_log_size = std::bit_width(in_size) - 1;
    const size_t min_round = std::min(in_log_size, ApproxLZ77::min_round), max_round = in_log_size - std::bit_width(ApproxLZ77::min_block_size) + 1;
    const int min_block_log_size = std::bit_width(ApproxLZ77::min_block_size) - 1;
    size_t max_block_log_size;
    size_t round = min_round;

    auto match_nodes = [&](size_t p_round, bool p_capture_refs = true) {
        const size_t block_size = in_size >> p_round;        
        const size_t data_per_chunk = (input_span.size() - block_size + num_threads - 1) / num_threads;
        const size_t num_chunks = (input_span.size() - block_size + data_per_chunk - 1) / data_per_chunk;
        
        //ShardedMap<size_t, u_int32_t, ankerl::unordered_dense::map, LeftMostOccurence> fp_table(num_threads, 128);
        std::unique_ptr<ankerl::unordered_dense::map<size_t, u_int32_t>> fp_table[num_threads];
        auto ref_table = block_table.create_fp_table(fp_table, unmarked_nodes, p_round);
        unmarked_nodes[0].fp.precompute_pop_values();

        #pragma omp parallel for
        for(size_t chunk_id = 0; chunk_id < num_chunks; chunk_id++) {
            const size_t start_pos = chunk_id * data_per_chunk;
            const size_t end_pos = std::min(start_pos + data_per_chunk, input_span.size() - block_size);

            RabinKarpFingerprint test_fp = [&]() {
                if(chunk_id == 0) return unmarked_nodes[0].fp;
                else if(start_pos < end_pos) {
                    return RabinKarpFingerprint(std::span<const char8_t>(input_span.data() + start_pos, block_size));
                }
                else return RabinKarpFingerprint();
            }();

            for(size_t pos = start_pos; pos < end_pos; pos++) {
                block_table.preprocess_matches(pos, test_fp.val, fp_table, ref_table);
                test_fp.shift_right(input_span[pos], input_span[pos + block_size]);
            }
        }

        if(p_capture_refs) block_table.postprocess_matches(unmarked_nodes, ref_table, p_round, marked_refs);
        else block_table.postprocess_matches(unmarked_nodes, ref_table, p_round);
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
        const size_t chains_per_chunk = (chain_ids.size() + ApproxLZ77Par::num_threads - 1) / ApproxLZ77Par::num_threads;

        auto [chunk_size, chunked_ids] = chunks(chain_ids, ApproxLZ77Par::num_threads);

        #pragma omp parallel for ordered
        for(size_t chunk_id = 0; chunk_id < chunked_ids.size(); chunk_id++) {
            auto & chunk = chunked_ids[chunk_id];
            std::vector<u_int8_t> chain_sizes;
            
            bool is_chain_up = (chunk_size * chunk_id) % 2;
            for(auto cherry_node : chunk) {
                int bit_pos = is_chain_up ? min_block_log_size : max_block_log_size;
                const int bit_dir = is_chain_up ? 1 : -1;
                const int bit_end = is_chain_up ? max_block_log_size + 1 : min_block_log_size - 1;

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
    
    auto process_round = [&]() {
        match_nodes(round);
        if(round == max_round) return 0;
        unmarked_nodes = block_table.next_nodes(unmarked_nodes, chain_ids, round);
        return 1;
    };   

    auto push_factors = [&]() {

        std::sort(std::execution::par, marked_refs.begin(), marked_refs.end());
        std::sort(std::execution::par, chain_ids.begin(), chain_ids.end());

        const auto factor_log_sizes = extract_factor_log_sizes();

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
        return 0;
    };    

    
    // Execute Algorithm
    if (max_round - min_round > 6) block_table.precompute_fingerprint(max_round - 3);
    auto start = std::chrono::high_resolution_clock::now();
    init_nodes(ApproxLZ77::dynamic_init);
    auto end = std::chrono::high_resolution_clock::now();
    m_stats.m_specialized_stats.m_field_values[Init_Time] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    while(round <= max_round) {
        if(process_round()) round++;
        else break;
    }
    block_table.populate_unmarked_chain(unmarked_nodes, chain_ids, round);
    end = std::chrono::high_resolution_clock::now();
    m_stats.m_specialized_stats.m_field_values[Round_Time] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    push_factors();
    end = std::chrono::high_resolution_clock::now();
    m_stats.m_specialized_stats.m_field_values[Factor_Time] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
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