#pragma once

#include <span>
#include <numeric>
#include <bit>
#include "Definition.hpp"
#include "RabinKarp.hpp"
#include "unordered_dense.h"
#include "BlockTableBasic.hpp"
#include "sharded_map.hpp"
#include <omp.h>

using namespace ApproxLZ77;


namespace ApproxLZ77Par {

    struct LeftMostOccurence {
        using InputValue = u_int32_t*;

        static void update(const size_t &, u_int32_t &metric, InputValue &&position) {
            if(metric > *position) metric = *position;
            else *position = metric;
        }

        static u_int32_t init(const size_t &, InputValue &&position) {
            return *position;
        }
    };
}

/**
 * @brief Class controls Data-Storage and Manipulation for ApproxLZ77-Algorithm
 * 
 * @tparam Sequence The range-type of the input data

*/
template<typename Sequence> requires NumRange<Sequence>
class BlockTablePar {
    using Item = typename Sequence::value_type;
    using ShardedMap = sharded_map::ShardedMap<size_t, u_int32_t, ankerl::unordered_dense::map, ApproxLZ77Par::LeftMostOccurence>;

private:
    const Sequence input_data;
    const u_int32_t in_size;
    const u_int32_t in_ceil_size;
    size_t precomputed_round = 0;
    std::vector<RabinKarpFingerprint> precomputed_fps;

    RabinKarpFingerprint extract_precomputed_fp(size_t p_round, size_t p_block_id) {
        if(p_round == precomputed_round) return precomputed_fps[p_block_id];

        size_t start_block_id = p_block_id << (precomputed_round - p_round);
        size_t num_chunk = 1 << (precomputed_round - p_round);
        size_t end_block_id = std::min(start_block_id + num_chunk, precomputed_fps.size());

        return std::accumulate(precomputed_fps.begin() + start_block_id, precomputed_fps.begin() + end_block_id, RabinKarpFingerprint(), 
        [](RabinKarpFingerprint p_fp, const RabinKarpFingerprint &p_fp_right) {
            return p_fp + p_fp_right;
        });
    }

    auto split_block_node(const BlockNode *p_block_node, size_t p_round) {
        const size_t block_size = in_ceil_size >> p_round;
        const auto block_start_it = input_data.begin() + p_block_node->block_id * 2 * block_size;
        const auto block_end_it = (p_block_node->block_id + 1) * 2 * block_size <= in_size ? input_data.begin() + (p_block_node->block_id + 1) * 2 * block_size 
                            : input_data.end();
        
        if(p_block_node->block_id * 2 * block_size + block_size < in_size) {
            auto right_fp = p_round <= precomputed_round ? extract_precomputed_fp(p_round, p_block_node->block_id*2+1) : 
                            RabinKarpFingerprint(std::span<const Item>(block_start_it + block_size, block_end_it));
            auto left_fp = p_block_node->fp.split_off(right_fp, block_end_it - block_start_it - block_size);    
            return std::pair<BlockNode, BlockNode>{BlockNode(p_block_node->block_id*2, 0, left_fp), BlockNode(p_block_node->block_id*2+1, 0, right_fp)};
        }
        else {
            auto left_fp =  p_round <= precomputed_round ? extract_precomputed_fp(p_round, p_block_node->block_id*2) : 
                            RabinKarpFingerprint(std::span<const Item>(block_start_it, block_end_it));
            return std::pair<BlockNode, BlockNode>{BlockNode(p_block_node->block_id*2, 0, left_fp), BlockNode()};
        }
    }

    auto next_nodes_seq(const BlockNodeRange auto &p_prev_nodes, std::vector<CherryNode> &p_chain_ids, size_t p_prev_round) {
        const size_t prev_block_size = in_ceil_size >> p_prev_round;
        const size_t cur_block_size = prev_block_size >> 1;
        std::vector<BlockNode> next_unmarked_nodes;

        for(size_t i = 0; i < p_prev_nodes.size(); i += 2) {
            const BlockNode* block_node = &p_prev_nodes[i], *sibling_node = (i < p_prev_nodes.size()-1) ? &p_prev_nodes[i+1] : nullptr;

            const bool is_marked = block_node->chain_info & prev_block_size;
            const bool is_sibling_marked = sibling_node && sibling_node->chain_info & prev_block_size;
            
            if(is_marked && (!sibling_node || is_sibling_marked)) {
                p_chain_ids.emplace_back(block_node->block_id * prev_block_size + prev_block_size - 1, block_node->chain_info);
                if(sibling_node) [[likely]] p_chain_ids.emplace_back(sibling_node->block_id * prev_block_size, sibling_node->chain_info);
                continue;
            }

            if(!is_marked) {
                auto [left_node, right_node] = split_block_node(block_node, p_prev_round + 1);
                left_node.chain_info = block_node->chain_info;
                if(sibling_node && is_sibling_marked) right_node.chain_info = sibling_node->chain_info;
                next_unmarked_nodes.push_back(left_node);
                if(right_node.is_valid()) [[likely]] next_unmarked_nodes.push_back(right_node);
            }

            if(sibling_node && !is_sibling_marked) {
                auto [left_node, right_node] = split_block_node(sibling_node, p_prev_round + 1);
                if(is_marked) left_node.chain_info = block_node->chain_info;
                next_unmarked_nodes.push_back(left_node);
                if(right_node.is_valid()) [[likely]] {
                    right_node.chain_info = sibling_node->chain_info;
                    next_unmarked_nodes.push_back(right_node);
                }
            }
        }
        return next_unmarked_nodes;        
    }

public:

    BlockTablePar(const Sequence &p_input_data) : input_data(p_input_data), in_size(input_data.size()), in_ceil_size(std::bit_ceil(in_size)) {}

    /**
     * @brief Initializes Sequence of unmarked Blocknodes for the initial round
     * 
     * @param p_init_round The initial round of the algorithm
    */
    auto init_nodes(size_t p_init_round) {
        const size_t block_size = in_ceil_size >> p_init_round;
        const size_t num_blocks = (in_size + block_size - 1) / block_size;
        auto unmarked_nodes = std::vector<BlockNode>(num_blocks);

        #pragma omp parallel for
        for(size_t i = 0; i < num_blocks; i++) {            
            unmarked_nodes[i].block_id = i;
            unmarked_nodes[i].chain_info = 0;
            unmarked_nodes[i].fp = [&](){
                if(precomputed_round >= p_init_round) return extract_precomputed_fp(p_init_round, i);
                if(i == num_blocks - 1) [[unlikely]] return RabinKarpFingerprint(std::span<const Item>(input_data.begin() + i*block_size, input_data.end()));
                return RabinKarpFingerprint(std::span<const Item>(input_data.data() + i*block_size, block_size));
            }();
        }
        return unmarked_nodes;
    }

    /**
     * @brief Precompute Fingerprint of all blocks for a given round
     * 
     * @param p_round The round to precompute the fingerprints for
    */
    void precompute_fingerprint(size_t p_round) {
        precomputed_round = p_round;
        const size_t block_size = in_ceil_size >> p_round;
        const size_t num_blocks = (in_size + block_size - 1) / block_size;

        precomputed_fps.resize(num_blocks);

        #pragma omp parallel for
        for(size_t i = 0; i < num_blocks; i++) {
            const size_t start = i * block_size;
            const size_t end = std::min(start + block_size, input_data.size());
            precomputed_fps[i] = RabinKarpFingerprint(std::span(input_data.begin() + start, input_data.begin() + end));
        }
    }

    /**
     * @brief Fill Fingerprint-Table from current unmarked nodes
     * 
     * @param p_fp_table The Fingerprint-Table to populate
     * @param p_unmarked_nodes The current unmarked nodes
     * @param p_cur_round The current round of the algorithm
    */
    auto create_fp_table(std::unique_ptr<ankerl::unordered_dense::map<size_t, u_int32_t>> p_fp_table[], const BlockNodeRange auto& p_unmarked_nodes, size_t p_cur_round) {
        const size_t block_size = in_ceil_size >> p_cur_round;
        const size_t nodes_size = p_unmarked_nodes.back().block_id * block_size + block_size > in_size ? p_unmarked_nodes.size() - 1 : p_unmarked_nodes.size();

        std::vector<u_int32_t> ref_table(p_unmarked_nodes.size());
        std::vector<u_int8_t> flag_table(p_unmarked_nodes.size());
        
        #pragma omp parallel
        {
            #pragma omp for
            for(size_t i = 1; i < nodes_size; i++) {
                flag_table[i] = p_unmarked_nodes[i].fp.val & ApproxLZ77Par::num_thread_mask;
            }

            ankerl::unordered_dense::map<size_t, u_int32_t> t_fp_table;
            u_int32_t chunk_id = omp_get_thread_num();
            for(size_t i = 1; i < nodes_size; i++) {
                if(flag_table[i] != chunk_id) continue;
                auto [match_it, insert_success] = t_fp_table.insert(std::pair(p_unmarked_nodes[i].fp.val, i));
                ref_table[i] = p_unmarked_nodes[match_it->second].block_id * block_size;
            }
            p_fp_table[chunk_id] = std::make_unique<ankerl::unordered_dense::map<size_t, u_int32_t>>(std::move(t_fp_table));
        }

        return ref_table;
    }

    std::vector<u_int32_t> create_fp_table(ShardedMap &p_fp_table, const std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round);

    /**
     * @brief Create previous instance of unmarked nodes from current unmarked nodes
     * 
     * @param p_cur_nodes The current unmarked nodes(Out)
     * @param p_diff_round Numer of rounds to move back
    */
    void previous_nodes(std::vector<BlockNode> &p_cur_nodes, size_t p_diff_round) {
        const size_t num_nodes_pack = 1 << p_diff_round;
        
        #pragma omp parallel for ordered
        for(size_t i = 0; i < p_cur_nodes.size(); i += num_nodes_pack) {
            auto fp = std::accumulate(p_cur_nodes.begin() + i, p_cur_nodes.begin() + std::min(i + num_nodes_pack, p_cur_nodes.size()), RabinKarpFingerprint(), 
            [](RabinKarpFingerprint p_fp, const BlockNode &p_node_right) {
                return p_fp + p_node_right.fp;
            });

            u_int32_t new_block_id = i / num_nodes_pack;
            #pragma omp ordered
            p_cur_nodes[new_block_id] = { new_block_id, 0, fp };
            
        }
        p_cur_nodes.resize((p_cur_nodes.size() + num_nodes_pack - 1) / num_nodes_pack);
    }

    /**
     * @brief Create next round of unmarked nodes from current marked and unmarked nodes
     * 
     * @param p_prev_nodes The marked nodes from previous round
     * @param p_chain_ids Sequence of CherryNodes to be expanded in case of new fully marked blocks
     * @param p_prev_round The previous round of the algorithm
    */
    auto next_nodes(const BlockNodeRange auto &p_prev_nodes, std::vector<CherryNode> &p_chain_ids, size_t p_prev_round) {
        const size_t block_size = in_ceil_size >> p_prev_round;
        std::vector<BlockNode> next_unmarked_nodes;

        const size_t nodes_per_chunk = [&](){
            size_t tmp = (p_prev_nodes.size() + ApproxLZ77Par::num_threads - 1) / ApproxLZ77Par::num_threads;
            return tmp + tmp % 2;
        }();
        const size_t num_chunks = (p_prev_nodes.size() + nodes_per_chunk - 1) / nodes_per_chunk;
        std::vector<size_t> node_sizes(num_chunks + 1, 0), chain_sizes(num_chunks + 1, 0);

        #pragma omp parallel for
        for(size_t chunk_id = 0; chunk_id < num_chunks; chunk_id++) {
            size_t start_pos = chunk_id * nodes_per_chunk;
            size_t end_pos = std::min(start_pos + nodes_per_chunk, p_prev_nodes.size());
            
            for(size_t i = start_pos; i < end_pos; i+=2) {
                auto *node = &p_prev_nodes[i];
                auto *sibling = i + 1 < end_pos ? &p_prev_nodes[i + 1] : nullptr;
                bool is_marked = node->chain_info & block_size;
                bool is_sibling_marked = sibling && sibling->chain_info & block_size;
                
                if(!is_marked) {
                    node_sizes[chunk_id + 1] += 1;
                    if(node->block_id * block_size + block_size/2 < in_size) node_sizes[chunk_id + 1] += 1;
                }                   
                if(sibling && !is_sibling_marked) {
                    node_sizes[chunk_id + 1] += 1;
                    if(sibling->block_id * block_size + block_size/2 < in_size) node_sizes[chunk_id + 1] += 1;
                }

                if(is_marked) {
                    if(!sibling || is_sibling_marked) chain_sizes[chunk_id + 1] += 1;
                    if(sibling && is_sibling_marked) chain_sizes[chunk_id + 1] += 1;
                }
            }
        }
        
        std::partial_sum(node_sizes.begin(), node_sizes.end(), node_sizes.begin());
        next_unmarked_nodes.resize(node_sizes.back());

        const size_t chain_init_size = p_chain_ids.size();
        std::partial_sum(chain_sizes.begin(), chain_sizes.end(), chain_sizes.begin());
        p_chain_ids.resize(chain_init_size + chain_sizes.back());

        #pragma omp parallel for
        for(size_t chunk_id = 0; chunk_id < num_chunks; chunk_id++)
        {
            std::vector<CherryNode> chain;
            chain.reserve(chain_sizes[chunk_id + 1] - chain_sizes[chunk_id]);            
            size_t start_pos = chunk_id * nodes_per_chunk;
            size_t end_pos = std::min(start_pos + nodes_per_chunk, p_prev_nodes.size());

            auto tmp = next_nodes_seq(std::span<const BlockNode>(p_prev_nodes.begin() + start_pos, p_prev_nodes.begin() + end_pos), chain, p_prev_round);
            std::copy(tmp.begin(), tmp.end(), next_unmarked_nodes.begin() + node_sizes[chunk_id]);
            std::copy(chain.begin(), chain.end(), p_chain_ids.begin() + chain_init_size + chain_sizes[chunk_id]);
        }
        return next_unmarked_nodes;
    }

    /**
     * @brief Match current data-window to any unmarked block by updated the leftmost reference-position
     * 
     * @param p_pos Startposition of sliding window
     * @param p_fp Fingerprint of Sliding Window to check against
     * @param p_fp_table Fingerprint Table of unmarked blocks
     */
    void preprocess_matches(u_int32_t p_pos, size_t p_fp, std::unique_ptr<ankerl::unordered_dense::map<size_t, u_int32_t>> p_fp_table[], std::vector<u_int32_t> &p_ref_table) {
        auto &t_fp_table = *p_fp_table[p_fp & ApproxLZ77Par::num_thread_mask];
        auto match_it = t_fp_table.find(p_fp);
        if(match_it != t_fp_table.end() && p_ref_table[match_it->second] > p_pos) p_ref_table[match_it->second] = p_pos;
    }

     void preprocess_matches(u_int32_t p_pos, size_t p_fp, ShardedMap &p_fp_table, std::vector<u_int32_t> &p_ref_table);

    /**
     * @brief Translate previuosly matched references into marked BlockNodes
     * 
     * @param p_unmarked_nodes Sequence of BlockNodes(partly marked)
     * @param p_fp_table Fingerprint Table of unmarked blocks
     * @param p_round Current Round
     */
    void postprocess_matches(std::vector<BlockNode> &p_unmarked_nodes, std::vector<u_int32_t> &p_ref_table, size_t p_round) {
        const size_t block_size = in_ceil_size >> p_round;

        #pragma omp parallel for
        for(size_t i = 1; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
            if(node.block_id * block_size + block_size > in_size) [[unlikely]] continue;
            if(p_ref_table[i] < node.block_id * block_size) node.chain_info |= block_size;
        }
    }

    /**
     * @brief Translate previously matched references into marked BlockNodes
     * 
     * @param p_unmarked_nodes Sequence of BlockNodes(partly marked)
     * @param p_fp_table Fingerprint Table of unmarked blocks
     * @param p_round Current Round
     * @param p_marked_refs Sequence of raw reference factors
     */
    void postprocess_matches(std::vector<BlockNode> &p_unmarked_nodes, std::vector<u_int32_t> &p_ref_table, size_t p_round, std::vector<BlockRef> &p_marked_refs) {
        const size_t block_size = in_ceil_size >> p_round;
 
        const size_t nodes_per_chunk = (p_unmarked_nodes.size() + ApproxLZ77Par::num_threads - 1) / ApproxLZ77Par::num_threads;
        const size_t num_chunk = (p_unmarked_nodes.size() + nodes_per_chunk - 1) / nodes_per_chunk;

        #pragma omp parallel for
        for(size_t chunk_id = 0; chunk_id < num_chunk; chunk_id++) {
            const size_t start_pos = chunk_id * nodes_per_chunk;
            const size_t end_pos = std::min(start_pos + nodes_per_chunk, p_unmarked_nodes.size());

            std::vector<BlockRef> tmp_refs;
            for(size_t i = start_pos; i < end_pos; i++) {
                auto &node = p_unmarked_nodes[i];
                if(node.block_id * block_size + block_size > in_size) [[unlikely]] continue;
                if(p_ref_table[i] < node.block_id * block_size) {
                    node.chain_info |= block_size;
                    tmp_refs.emplace_back(node.block_id * block_size, p_ref_table[i]);
                }
            }

            #pragma omp critical
            p_marked_refs.insert(p_marked_refs.end(), tmp_refs.begin(), tmp_refs.end());
        }
    }

    /**
     * @brief Populate CherryNodes from unmarked nodes at the end of the algorithm regardless of matches
     * 
     * @param p_unmarked_nodes The unmarked nodes at the end of the algorithm
     * @param p_chain_ids The set of CherryNodes to populate
     * @param p_round The current/last round of the algorithm
    */
    void populate_unmarked_chain(std::vector<BlockNode> &p_unmarked_nodes, std::vector<CherryNode> &p_chain_ids, size_t p_round) {
        const u_int32_t block_size = in_ceil_size >> p_round;
        const u_int32_t init_size = p_chain_ids.size();
        p_chain_ids.resize(init_size + p_unmarked_nodes.size());
        
        #pragma omp parallel for
        for(size_t i = 0; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
            u_int32_t tmp_block_size = std::min(block_size, in_size - node.block_id * block_size + 1);
            p_chain_ids[i + init_size] = { node.block_id * block_size, node.chain_info | tmp_block_size };
        }
    }
};