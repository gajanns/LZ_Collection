#pragma once

#include <span>
#include <numeric>
#include <bit>
#include "Definition.hpp"
#include "RabinKarp.hpp"
#include "unordered_dense.h"
#include "BlockTableBasic.hpp"
#include <omp.h>

using namespace ApproxLZ77;

/**
 * @brief Class controls Data-Storage and Manipulation for ApproxLZ77-Algorithm
 * 
 * @tparam Sequence The range-type of the input data

*/
template<typename Sequence> requires NumRange<Sequence>
class BlockTablePar {
    using Item = typename Sequence::value_type;
private:
    const Sequence input_data;
    const u_int32_t in_size;
    const u_int32_t in_ceil_size;

    BlockTableBasic<Sequence> block_table_basic;

public:

    BlockTablePar(const Sequence &p_input_data) : input_data(p_input_data), in_size(input_data.size()), in_ceil_size(std::bit_ceil(in_size)), block_table_basic(p_input_data) {}

    /**
     * @brief Initializes Sequence of unmarked Blocknodes for the initial round
     * 
     * @param p_init_round The initial round of the algorithm
    */
    auto init_nodes(size_t p_init_round) {
        size_t block_size = in_ceil_size >> p_init_round;
        size_t num_blocks = (in_size + block_size - 1) / block_size;
        auto unmarked_nodes = std::vector<BlockNode>(num_blocks);

        #pragma omp parallel for
        for(size_t i = 0; i < num_blocks; i++) {            
            unmarked_nodes[i].block_id = i;
            unmarked_nodes[i].chain_info = 0;
            if(i == num_blocks - 1) [[unlikely]] {
                unmarked_nodes[i].fp = RabinKarpFingerprint(std::span<const Item>(input_data.begin() + i*block_size, input_data.end()));
            }
            else [[likely]] {
                unmarked_nodes[i].fp = RabinKarpFingerprint(std::span<const Item>(input_data.data() + i*block_size, block_size));
            }
        }
        return unmarked_nodes;
    }

    /**
     * @brief Fill Fingerprint-Table from current unmarked nodes
     * 
     * @param p_fp_table The Fingerprint-Table to populate
     * @param p_unmarked_nodes The current unmarked nodes
     * @param p_cur_round The current round of the algorithm
    */
    std::vector<u_int32_t> create_fp_table(ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table, std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round) {
        p_fp_table.clear();
        std::vector<u_int32_t> ref_table(p_unmarked_nodes.size());

        size_t block_size = in_ceil_size >> p_cur_round;
        
        for(size_t i = 1; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
            ref_table[i] = node.block_id * block_size;
            if(node.block_id * block_size + block_size > in_size) [[unlikely]] break;

            auto match_it = p_fp_table.find(node.fp.val);
            if(match_it == p_fp_table.end()) {
                p_fp_table[node.fp.val] = i;
            }
            else {
                ref_table[i] = ref_table[match_it->second];
            }
        }
        return ref_table;
    }

    /**
     * @brief Create previous instance of unmarked nodes from current unmarked nodes
     * 
     * @param p_cur_nodes The current unmarked nodes(Out)
     * @param p_diff_round Numer of rounds to move back
    */
    void previous_nodes(std::vector<BlockNode> &p_cur_nodes, size_t p_diff_round) {
        size_t num_nodes_pack = 1 << p_diff_round;
        
        #pragma omp parallel for ordered
        for(size_t i = 0; i < p_cur_nodes.size(); i += num_nodes_pack) {
            auto fp = std::accumulate(p_cur_nodes.begin() + i, p_cur_nodes.begin() + std::min(i + num_nodes_pack, p_cur_nodes.size()), RabinKarpFingerprint(), 
            [](RabinKarpFingerprint p_fp, const BlockNode &p_node_right) {
                return p_fp + p_node_right.fp;
            });

            int32_t new_block_id = i / num_nodes_pack;
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
        std::vector<BlockNode> next_unmarked_nodes;
        size_t nodes_per_chunk = [&](){
            size_t tmp = (p_prev_nodes.size() + ApproxLZ77Par::num_threads - 1) / ApproxLZ77Par::num_threads;
            return tmp + tmp % 2;
        }();
        size_t num_chunks = (p_prev_nodes.size() + nodes_per_chunk - 1) / nodes_per_chunk;
        
        #pragma omp parallel for ordered
        for(size_t chunk_id = 0; chunk_id < num_chunks; chunk_id++)
        {
            std::vector<CherryNode> chain;            
            size_t start_pos = chunk_id * nodes_per_chunk;
            size_t end_pos = std::min(start_pos + nodes_per_chunk, p_prev_nodes.size());

            auto tmp = block_table_basic.next_nodes(std::span<const BlockNode>(p_prev_nodes.begin() + start_pos, p_prev_nodes.begin() + end_pos), chain, p_prev_round);
            #pragma omp ordered
            {
                next_unmarked_nodes.insert(next_unmarked_nodes.end(), tmp.begin(), tmp.end());
                p_chain_ids.insert(p_chain_ids.end(), chain.begin(), chain.end());
            }
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
    void preprocess_matches(u_int32_t p_pos, size_t p_fp, ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table, std::vector<u_int32_t> &p_ref_table) {
        auto match_it = p_fp_table.find(p_fp);
        if(match_it != p_fp_table.end() && p_ref_table[match_it->second] > p_pos) p_ref_table[match_it->second] = p_pos;
    }

    /**
     * @brief Translate previuosly matched references into marked BlockNodes
     * 
     * @param p_unmarked_nodes Sequence of BlockNodes(partly marked)
     * @param p_fp_table Fingerprint Table of unmarked blocks
     * @param p_round Current Round
     */
    void postprocess_matches(std::vector<BlockNode> &p_unmarked_nodes, std::vector<u_int32_t> &p_ref_table, size_t p_round) {
        size_t block_size = in_ceil_size >> p_round;

        #pragma omp parallel for
        for(size_t i = 1; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
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
        size_t block_size = in_ceil_size >> p_round;
 
        size_t nodes_per_chunk = (p_unmarked_nodes.size() + ApproxLZ77Par::num_threads - 1) / ApproxLZ77Par::num_threads;
        size_t num_chunk = (p_unmarked_nodes.size() + nodes_per_chunk - 1) / nodes_per_chunk;

        #pragma omp parallel for
        for(size_t chunk_id = 0; chunk_id < num_chunk; chunk_id++) {
            size_t start_pos = chunk_id * nodes_per_chunk;
            size_t end_pos = std::min(start_pos + nodes_per_chunk, p_unmarked_nodes.size());

            std::vector<BlockRef> tmp_refs;
            for(size_t i = start_pos; i < end_pos; i++) {
                auto &node = p_unmarked_nodes[i];
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
        u_int32_t block_size = in_ceil_size >> p_round;
        u_int32_t init_size = p_chain_ids.size();
        p_chain_ids.resize(init_size + p_unmarked_nodes.size());
        
        #pragma omp parallel for
        for(size_t i = 0; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
            u_int32_t tmp_block_size = std::min(block_size, in_size - node.block_id * block_size + 1);
            p_chain_ids[i + init_size] = { node.block_id * block_size, node.chain_info | tmp_block_size };
        }
    }
};