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
    Sequence input_data;
    BlockTableBasic<Sequence> block_table_basic;

    auto split_block_node(const BlockNode *p_block_node, size_t block_size) {
        return block_table_basic.split_block_node(p_block_node, block_size);
    }

public:

    BlockTablePar(const Sequence &p_input_data) : input_data(p_input_data), block_table_basic(p_input_data) {}

    /**
     * @brief Initializes Sequence of unmarked Blocknodes for the initial round
     * 
     * @param p_init_round The initial round of the algorithm
    */
    auto init_nodes(size_t p_init_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_init_round;
        size_t num_blocks = (input_data.size() + block_size - 1) / block_size;
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
    void create_fp_table(ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table, std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round, std::vector<BlockRef> *p_marked_refs=nullptr) {
        block_table_basic.create_fp_table(p_fp_table, p_unmarked_nodes, p_cur_round, p_marked_refs);
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

            size_t new_block_id = i / num_nodes_pack;
            #pragma omp ordered
            {
                p_cur_nodes[new_block_id].block_id = new_block_id;
                p_cur_nodes[new_block_id].chain_info = 0;
                p_cur_nodes[new_block_id].fp = fp;
            }
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
        
        #pragma omp parallel for num_threads(ApproxLZ77Par::num_threads) ordered
        for(size_t chunk_id = 0; chunk_id < num_chunks; chunk_id++)
        {
            std::vector<CherryNode> chain;            
            size_t start_pos = chunk_id * nodes_per_chunk;
            size_t end_pos = std::min(start_pos + nodes_per_chunk, p_prev_nodes.size());
            if(end_pos <= start_pos) continue;

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
    void preprocess_matches(u_int32_t p_pos, size_t p_fp, ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table) {
        block_table_basic.preprocess_matches(p_pos, p_fp, p_fp_table);
    }

    /**
     * @brief Translate previuosly matched references into marked BlockNodes
     * 
     * @param p_unmarked_nodes Sequence of BlockNodes(partly marked)
     * @param p_fp_table Fingerprint Table of unmarked blocks
     * @param p_round Current Round
     */
    void postprocess_matches(std::vector<BlockNode> &p_unmarked_nodes, ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table, size_t p_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_round;

        #pragma omp parallel for
        for(auto &node : p_unmarked_nodes | std::views::drop(1)) {
            if(node.block_id * block_size + block_size > input_data.size()) [[unlikely]] continue;
            if(node.chain_info & block_size) continue;
            auto ref_pos = p_fp_table[node.fp.val];
            if(ref_pos < node.block_id * block_size) {
                node.chain_info |= block_size;
            }
        }
    }

    /**
     * @brief Translate previuosly matched references into marked BlockNodes
     * 
     * @param p_unmarked_nodes Sequence of BlockNodes(partly marked)
     * @param p_fp_table Fingerprint Table of unmarked blocks
     * @param p_round Current Round
     * @param p_marked_refs Sequence of raw reference factors
     */
    void postprocess_matches(std::vector<BlockNode> &p_unmarked_nodes, ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table, size_t p_round, std::vector<BlockRef> &p_marked_refs) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_round;

        #pragma omp declare reduction (merge : std::vector<BlockRef> : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
        #pragma omp parallel for reduction(merge: p_marked_refs)
        for(auto &node : p_unmarked_nodes | std::views::drop(1)) {
            if(node.block_id * block_size + block_size > input_data.size()) [[unlikely]] continue;
            if(node.chain_info & block_size) continue;
            auto ref_pos = p_fp_table[node.fp.val];
            if(ref_pos < node.block_id * block_size) {
                node.chain_info |= block_size;
                p_marked_refs.emplace_back(node.block_id*block_size, ref_pos);
            }
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
        size_t block_size = std::bit_ceil(input_data.size()) >> p_round;
        size_t init_size = p_chain_ids.size();
        p_chain_ids.resize(init_size + p_unmarked_nodes.size());
        
        #pragma omp parallel for
        for(size_t i = 0; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
            size_t tmp_block_size = std::min(block_size, input_data.size() - node.block_id * block_size + 1);
            p_chain_ids[i + init_size].block_position = node.block_id * block_size;
            p_chain_ids[i + init_size].chain_info = node.chain_info | tmp_block_size;
        }
    }
};