#pragma once

#include <span>
#include <unordered_map>
#include <numeric>
#include <bit>
#include <set>
#include "Definition.hpp"
#include "RabinKarp.hpp"
#include "BlockTableBasic.hpp"
#include <omp.h>
#include <list>

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

    auto split_block_node(BlockNode *p_block_node, size_t block_size) {
        auto block_start_it = input_data.begin() + p_block_node->block_id * 2 * block_size;
        auto block_end_it = (p_block_node->block_id + 1) * 2 * block_size <= input_data.size() ? input_data.begin() + (p_block_node->block_id + 1) * 2 * block_size 
                            : input_data.end();
        
        if(p_block_node->block_id * 2 * block_size + block_size < input_data.size()) {
            auto [left_fp, right_fp] = p_block_node->fp.split(std::span<const Item>(block_start_it, block_end_it), block_size);
            return std::pair<BlockNode, BlockNode>{BlockNode(p_block_node->block_id*2, 0, left_fp), BlockNode(p_block_node->block_id*2+1, 0, right_fp)};
        }
        else {
            auto left_fp = RabinKarpFingerprint(std::span<const Item>(block_start_it, block_end_it));
            return std::pair<BlockNode, BlockNode>{BlockNode(p_block_node->block_id*2, 0, left_fp), BlockNode()};
        }
    }

public:

    BlockTablePar(const Sequence &p_input_data) : input_data(p_input_data) {}

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
            if((i+1)*block_size > input_data.size()) {
                unmarked_nodes[i] = BlockNode(i, 0, RabinKarpFingerprint(std::span<const char8_t>(input_data.begin() + i*block_size, input_data.end())));
            }
            else {
                unmarked_nodes[i] = BlockNode(i, 0, RabinKarpFingerprint(std::span<const char8_t>(input_data.data() + i*block_size, block_size)));
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
    void create_fp_table(std::unordered_map<u_int32_t, std::list<BlockNode*>> &p_fp_table, std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round) {
        p_fp_table.clear();
        p_fp_table.reserve(p_unmarked_nodes.size());
        size_t block_size = std::bit_ceil(input_data.size()) >> p_cur_round;
        for(auto &node : p_unmarked_nodes) {
            if(node.block_id * block_size + block_size > input_data.size()) break;
            p_fp_table[node.fp.val].push_back(&node);
        }
    }

    /**
     * @brief Create previous instance of unmarked nodes from current unmarked nodes
     * 
     * @param p_cur_nodes The current unmarked nodes(Out)
     * @param p_diff_round Numer of rounds to move back
    */
    void previous_nodes(std::vector<BlockNode> &p_cur_nodes, size_t p_diff_round) {
        size_t num_nodes_pack = 1 << p_diff_round;
        size_t new_nodes_size = 0;
        for(size_t i = 0; i < p_cur_nodes.size(); i += num_nodes_pack) {
            auto fp = std::accumulate(p_cur_nodes.begin() + i, p_cur_nodes.begin() + std::min(i + num_nodes_pack, p_cur_nodes.size()), RabinKarpFingerprint(), 
            [](RabinKarpFingerprint p_fp, const BlockNode &p_node_right) {
                return p_fp + p_node_right.fp;
            });
            p_cur_nodes[new_nodes_size].block_id = new_nodes_size;
            p_cur_nodes[new_nodes_size].chain_info = 0;
            p_cur_nodes[new_nodes_size].fp = fp;
            new_nodes_size++;
        }
        p_cur_nodes.resize(new_nodes_size);
    }

    /**
     * @brief Create next round of unmarked nodes from current marked and unmarked nodes
     * 
     * @param p_prev_nodes The marked nodes from previous round
     * @param p_chain_ids Sequence of CherryNodes to be expanded in case of new fully marked blocks
     * @param p_prev_round The previous round of the algorithm
    */
    auto next_nodes(std::vector<BlockNode> &p_prev_nodes, std::set<CherryNode> &p_chain_ids, size_t p_prev_round) {
        size_t prev_block_size = std::bit_ceil(input_data.size()) >> p_prev_round;
        size_t cur_block_size = prev_block_size >> 1;
        std::vector<BlockNode> next_unmarked_nodes;

        for(size_t i = 0; i < p_prev_nodes.size(); i += 2) {
            BlockNode* block_node = &p_prev_nodes[i], *sibling_node = (i < p_prev_nodes.size()-1) ? &p_prev_nodes[i+1] : nullptr;

            bool is_marked = block_node->chain_info & prev_block_size;
            bool is_sibling_marked = sibling_node && sibling_node->chain_info & prev_block_size;
            
            if(is_marked && (!sibling_node || is_sibling_marked)) {
                p_chain_ids.emplace(block_node->block_id * prev_block_size + prev_block_size - 1, block_node->chain_info);
                if(sibling_node) {
                    p_chain_ids.emplace(sibling_node->block_id * prev_block_size, sibling_node->chain_info);
                }
                continue;
            }

            if(!is_marked) {
                auto [left_node, right_node] = split_block_node(block_node, cur_block_size);
                left_node.chain_info = block_node->chain_info;
                if(sibling_node && is_sibling_marked) right_node.chain_info = sibling_node->chain_info;
                next_unmarked_nodes.push_back(left_node);
                if(right_node.is_valid()) next_unmarked_nodes.push_back(right_node);
            }

            if(sibling_node && !is_sibling_marked) {
                auto [left_node, right_node] = split_block_node(sibling_node, cur_block_size);
                if(is_marked) left_node.chain_info = block_node->chain_info;
                next_unmarked_nodes.push_back(left_node);
                if(right_node.is_valid()) {
                    right_node.chain_info = sibling_node->chain_info;
                    next_unmarked_nodes.push_back(right_node);
                }
            }
        }
        return next_unmarked_nodes;        
    }

    /**
     * @brief Match blocks in current round using Fingerprint-Table
     * 
     * @param p_pos The current position in the input data
     * @param p_fp The fingerprint of the sliding block
     * @param p_fp_table The Fingerprint-Table to use
     * @param p_marked_refs The set of marked references to be expanded in case of exact matches
     * @param p_cur_round The current round of the algorithm
    */
    void match_blocks(size_t p_pos, u_int32_t p_fp, std::unordered_map<u_int32_t, std::list<BlockNode*>> &p_fp_table, size_t p_cur_round, std::set<BlockRef> *p_marked_refs=nullptr) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_cur_round;
        auto candidate_blocks = p_fp_table.find(p_fp);
        if(candidate_blocks == p_fp_table.end()) return;

        for(auto it = candidate_blocks->second.begin(); it != candidate_blocks->second.end(); it++) {
            auto block = *it;
            if(block->block_id * block_size <= p_pos) continue;

            if(std::equal(input_data.begin() + block->block_id * block_size, input_data.begin() + block->block_id * block_size + block_size, input_data.begin() + p_pos)) {
                block->chain_info |= block_size;
                if(p_marked_refs)p_marked_refs->emplace(block->block_id*block_size, p_pos);
                candidate_blocks->second.erase(it);
                break;
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
    void populate_unmarked_chain(std::vector<BlockNode> &p_unmarked_nodes, std::set<CherryNode> &p_chain_ids, size_t p_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_round;
        for(auto &node : p_unmarked_nodes) {
            size_t tmp_block_size = std::min(block_size, input_data.size() - node.block_id * block_size + 1);
            p_chain_ids.emplace(node.block_id * block_size, node.chain_info | tmp_block_size);
        }
    }
};