#pragma once

#include <span>
#include <numeric>
#include <bit>
#include "Definition.hpp"
#include "RabinKarp.hpp"
#include "unordered_dense.h"

using namespace ApproxLZ77;

namespace ApproxLZ77 {
    /**
     * @brief Encapsulates a block of data
     * 
     * @param block_id The id of the block at current level of implicit tree (-1 => invalid block)
     * @param chain_info Stores the structure of marked nodes in previous levels
     * @param fp The fingerprint of the block
    */
    struct BlockNode {
        u_int32_t block_id = std::numeric_limits<u_int32_t>::max();
        u_int32_t chain_info = 0;
        RabinKarpFingerprint fp;
        
        BlockNode() = default;
        BlockNode(u_int32_t p_block_id, u_int32_t p_chain_info, RabinKarpFingerprint p_fp): block_id(p_block_id), chain_info(p_chain_info), fp(p_fp){}

        bool operator==(const BlockNode &p_rhs) const {
            return block_id == p_rhs.block_id && chain_info == p_rhs.chain_info && fp == p_rhs.fp;
        }

        bool operator<(const BlockNode &p_rhs) const {
            return block_id < p_rhs.block_id;
        }

        bool is_valid() const {
            return block_id != std::numeric_limits<u_int32_t>::max();
        }
    };

    /**
     * @brief Concept for Range containing BlockNodes
    */
    template<typename T>
    concept BlockNodeRange = std::ranges::range<T> && std::same_as<typename T::value_type, BlockNode>;

    /**
     * @brief Encapsulates a reference to a block (=> LZ77-Factor)
     * 
     * @param block_position The position of the block in the input data
     * @param ref_position The position of the reference in the input data (ref_position < block_position)
    */
    struct BlockRef {
        u_int32_t block_position;
        u_int32_t ref_position;

        bool operator==(const BlockRef &p_rhs) const {
            return block_position == p_rhs.block_position && ref_position == p_rhs.ref_position;
        }

        bool operator<(const BlockRef &p_rhs) const {
            return block_position < p_rhs.block_position;
        }

        BlockRef(u_int32_t p_block_position, u_int32_t p_ref_position): block_position(p_block_position), ref_position(p_ref_position){}
    };

    /**
     * @brief Encapsulates a cherry node (Parentnode has been completely marked by its descendants)
     * 
     * @param block_position Position within Block ("only" used for ordering)
     * @param chain_info The structure of marked nodes in previous levels
    */
    struct CherryNode {
        u_int32_t block_position = 0;
        u_int32_t chain_info = 0;

        bool operator==(const CherryNode &p_rhs) const {
            return block_position == p_rhs.block_position && chain_info == p_rhs.chain_info;
        }

        bool operator<(const CherryNode &p_rhs) const {
            return block_position < p_rhs.block_position;
        }

        CherryNode() = default;
        CherryNode(u_int32_t p_block_position, u_int32_t p_chain_info): block_position(p_block_position), chain_info(p_chain_info){}
    };
}

/**
 * @brief Class controls Data-Storage and Manipulation for ApproxLZ77-Algorithm
 * 
 * @tparam Sequence The range-type of the input data

*/
template<typename Sequence> requires NumRange<Sequence>
class BlockTableBasic {
    using Item = typename Sequence::value_type;
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
            RabinKarpFingerprint left_fp, right_fp;
            if(p_round <= precomputed_round) {
                left_fp = extract_precomputed_fp(p_round, p_block_node->block_id*2);
                right_fp = extract_precomputed_fp(p_round, p_block_node->block_id*2+1);
            }
            else {
                auto [l, r] = p_block_node->fp.split(std::span<const Item>(block_start_it, block_end_it), block_size);
                left_fp = l;
                right_fp = r;
            }            
            return std::pair<BlockNode, BlockNode>{BlockNode(p_block_node->block_id*2, 0, left_fp), BlockNode(p_block_node->block_id*2+1, 0, right_fp)};
        }
        else {
            auto left_fp = p_round <= precomputed_round ? extract_precomputed_fp(p_round, p_block_node->block_id*2) : RabinKarpFingerprint(std::span<const Item>(block_start_it, block_end_it));
            return std::pair<BlockNode, BlockNode>{BlockNode(p_block_node->block_id*2, 0, left_fp), BlockNode()};
        }
    }

public:

    BlockTableBasic(const Sequence &p_input_data) : input_data(p_input_data), in_size(input_data.size()), in_ceil_size(std::bit_ceil(in_size)) {}

    /**
     * @brief Initializes Sequence of unmarked Blocknodes for the initial round
     * 
     * @param p_init_round The initial round of the algorithm
    */
    auto init_nodes(size_t p_init_round) {
        const size_t block_size = in_ceil_size >> p_init_round;
        const size_t num_blocks = (in_size + block_size - 1) / block_size;
        auto unmarked_nodes = std::vector<BlockNode>(num_blocks);

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

    void precompute_fingerprint(size_t p_round) {
        precomputed_round = p_round;
        const size_t block_size = in_ceil_size >> p_round;
        const size_t num_blocks = (in_size + block_size - 1) / block_size;

        precomputed_fps.clear();
        precomputed_fps.reserve(num_blocks);
        for(size_t i = 0; i < num_blocks; i++) {
            const size_t start = i * block_size;
            const size_t end = std::min(start + block_size, input_data.size());
            precomputed_fps.push_back(RabinKarpFingerprint(std::span(input_data.begin() + start, input_data.begin() + end)));
        }
    }

    /**
     * @brief Fill Fingerprint-Table from current unmarked nodes
     * 
     * @param p_fp_table The Fingerprint-Table to populate
     * @param p_unmarked_nodes The current unmarked nodes
     * @param p_cur_round The current round of the algorithm
    */
    std::vector<u_int32_t> create_fp_table(ankerl::unordered_dense::map<size_t, u_int32_t> &p_fp_table, std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round) {
        std::vector<u_int32_t> ref_table(p_unmarked_nodes.size());

        const size_t block_size = in_ceil_size >> p_cur_round;
        const size_t nodes_size = p_unmarked_nodes.back().block_id * block_size + block_size > in_size ? p_unmarked_nodes.size() - 1 : p_unmarked_nodes.size();
        
        for(size_t i = 1; i < nodes_size; i++) {
            auto [match_it, insert_success] = p_fp_table.insert({p_unmarked_nodes[i].fp.val, i});
            ref_table[i] = p_unmarked_nodes[match_it->second].block_id * block_size;
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
        const size_t num_nodes_pack = 1 << p_diff_round;
        
        for(size_t i = 0; i < p_cur_nodes.size(); i += num_nodes_pack) {
            auto fp = std::accumulate(p_cur_nodes.begin() + i, p_cur_nodes.begin() + std::min(i + num_nodes_pack, p_cur_nodes.size()), RabinKarpFingerprint(), 
            [](RabinKarpFingerprint p_fp, const BlockNode &p_node_right) {
                return p_fp + p_node_right.fp;
            });
            u_int32_t new_block_id = i / num_nodes_pack;
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
     * @param p_marked_refs Sequence of raw reference factors
     */
    void postprocess_matches(std::vector<BlockNode> &p_unmarked_nodes, std::vector<u_int32_t> &p_ref_table, size_t p_round, std::vector<BlockRef> *p_marked_refs=nullptr) {
        const size_t block_size = in_ceil_size >> p_round;
        for(size_t i = 1; i < p_unmarked_nodes.size(); i++) {
            auto &node = p_unmarked_nodes[i];
            if(node.block_id * block_size + block_size > in_size) [[unlikely]] continue;
            if(p_ref_table[i] < node.block_id * block_size) {
                node.chain_info |= block_size;
                if(p_marked_refs) p_marked_refs->emplace_back(node.block_id * block_size, p_ref_table[i]);
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
        const u_int32_t block_size = in_ceil_size >> p_round;
        for(auto &node : p_unmarked_nodes) {
            u_int32_t tmp_block_size = std::min(block_size, in_size - node.block_id * block_size + 1);
            p_chain_ids.emplace_back(node.block_id * block_size, node.chain_info | tmp_block_size);
        }
    }
};