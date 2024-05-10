#pragma once

#include <span>
#include <unordered_map>
#include <bit>
#include <set>
#include "Definition.hpp"
#include "RabinKarp.hpp"
#include "ApproxLZ77Compressor.hpp"

using namespace ApproxLZ77;

namespace ApproxLZ77 {
    struct BlockNode {
        int64_t block_id = -1;
        size_t chain_info = 0;
        RabinKarpFingerprint fp;
        
        BlockNode() = default;
        BlockNode(int64_t p_block_id, size_t p_chain_info, RabinKarpFingerprint p_fp): block_id(p_block_id), chain_info(p_chain_info), fp(p_fp){}
    };

    struct BlockRef {
        size_t block_position;
        size_t ref_position;

        bool operator==(const BlockRef &p_rhs) const {
            return block_position == p_rhs.block_position && ref_position == p_rhs.ref_position;
        }

        bool operator<(const BlockRef &p_rhs) const {
            return block_position < p_rhs.block_position;
        }

        BlockRef(size_t p_block_position, size_t p_ref_position): block_position(p_block_position), ref_position(p_ref_position){}
    };
}

template<typename Sequence> requires NumRange<Sequence>
class BlockTableBasic {
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

    BlockTableBasic(const Sequence &p_input_data) : input_data(p_input_data) {}

    auto init_nodes(size_t p_init_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_init_round;
        size_t num_blocks = 1 << p_init_round;
        auto unmarked_nodes = std::vector<BlockNode>();

        for(size_t i = 0; i < num_blocks; i++) {
            if(i*block_size >= input_data.size()) break;
            if((i+1)*block_size > input_data.size()) {
                unmarked_nodes.emplace_back(i, 0, RabinKarpFingerprint(std::span<const char8_t>(input_data.begin() + i*block_size, input_data.end())));
            }
            else {
                unmarked_nodes.emplace_back(i, 0, RabinKarpFingerprint(std::span<const char8_t>(input_data.data() + i*block_size, block_size)));
            }
        }
        return unmarked_nodes;
    }

    void create_fp_table(std::unordered_map<size_t, std::vector<BlockNode*>> &p_fp_table, std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round) {
        p_fp_table.clear();
        size_t block_size = std::bit_ceil(input_data.size()) >> p_cur_round;
        for(auto &node : p_unmarked_nodes) {
            if(node.block_id * block_size + block_size > input_data.size()) break;
            p_fp_table[node.fp.val].push_back(&node);
        }
    }

    auto next_nodes(std::vector<BlockNode> &p_prev_nodes, std::map<size_t, size_t> &p_chain_ids, size_t p_prev_round) {
        size_t prev_block_size = std::bit_ceil(input_data.size()) >> p_prev_round;
        size_t cur_block_size = prev_block_size >> 1;
        std::vector<BlockNode> next_unmarked_nodes;

        for(size_t i = 0; i < p_prev_nodes.size(); i += 2) {
            BlockNode* block_node = &p_prev_nodes[i], *sibling_node = (i < p_prev_nodes.size()-1) ? &p_prev_nodes[i+1] : nullptr;

            bool is_marked = block_node->chain_info & prev_block_size;
            bool is_sibling_marked = sibling_node && sibling_node->chain_info & prev_block_size;
            
            if(is_marked && (!sibling_node || is_sibling_marked)) {
                p_chain_ids[block_node->block_id * prev_block_size + prev_block_size - 1] = block_node->chain_info;
                if(sibling_node) {
                    p_chain_ids[sibling_node->block_id * prev_block_size] = sibling_node->chain_info;
                }
                continue;
            }

            if(!is_marked) {
                auto [left_node, right_node] = split_block_node(block_node, cur_block_size);
                left_node.chain_info = block_node->chain_info;
                if(sibling_node && is_sibling_marked) right_node.chain_info = sibling_node->chain_info;
                next_unmarked_nodes.push_back(left_node);
                if(right_node.block_id != -1) next_unmarked_nodes.push_back(right_node);
            }

            if(sibling_node && !is_sibling_marked) {
                auto [left_node, right_node] = split_block_node(sibling_node, cur_block_size);
                if(is_marked) left_node.chain_info = block_node->chain_info;
                next_unmarked_nodes.push_back(left_node);
                if(right_node.block_id != -1) {
                    right_node.chain_info = sibling_node->chain_info;
                    next_unmarked_nodes.push_back(right_node);
                }
            }
        }
        return next_unmarked_nodes;        
    }

    void match_blocks(size_t p_pos, size_t p_fp, std::unordered_map<size_t, std::vector<BlockNode*>> &p_fp_table, std::set<BlockRef> &p_marked_refs, size_t p_cur_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_cur_round;
        auto candidate_blocks = p_fp_table.find(p_fp);
        if(candidate_blocks == p_fp_table.end()) return;

        for(auto block : candidate_blocks->second | std::views::reverse) {
            if(block->block_id * block_size <= p_pos) break;
            if(block->chain_info & block_size) continue;

            if(std::equal(input_data.begin() + block->block_id * block_size, input_data.begin() + block->block_id * block_size + block_size, input_data.begin() + p_pos)) {
                block->chain_info |= block_size;
                p_marked_refs.emplace(block->block_id*block_size, p_pos);
            }
        }
    }

    void populate_unmarked_chain(std::vector<BlockNode> &p_unmarked_nodes, std::map<size_t, size_t> &p_chain_ids, size_t p_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_round;
        for(auto &node : p_unmarked_nodes) {
            p_chain_ids[node.block_id * block_size] = node.chain_info | block_size;
        }
    }
};