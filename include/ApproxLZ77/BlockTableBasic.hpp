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
        size_t block_id;
        size_t chain_info;
        RabinKarpFingerprint fp;
        BlockNode(size_t p_block_id, size_t p_chain_info, RabinKarpFingerprint p_fp): block_id(p_block_id), chain_info(p_chain_info), fp(p_fp){}
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

    std::pair<std::shared_ptr<BlockNode>, std::shared_ptr<BlockNode>> split_block_node(BlockNode *p_block_node, size_t block_size) {
        std::shared_ptr<BlockNode> left_node = nullptr, right_node = nullptr;
        if(p_block_node->block_id * 2 * block_size + 2 * block_size <= input_data.size()) {
            auto [left_fp, right_fp] = p_block_node->fp.split(
                std::span<const char8_t>(input_data.data() + p_block_node->block_id * 2 * block_size, 2 * block_size), 
                block_size);
            left_node = std::make_shared<BlockNode>(p_block_node->block_id*2, 0, left_fp);
            right_node = std::make_shared<BlockNode>(p_block_node->block_id*2+1, 0, right_fp);
        }
        else if(p_block_node->block_id * 2 * block_size + block_size < input_data.size()) {
            auto left_fp = RabinKarpFingerprint(std::span<const char8_t>(input_data.data() + p_block_node->block_id * 2 * block_size, block_size));
            auto right_fp = RabinKarpFingerprint(std::span<const char8_t>(input_data.begin() + p_block_node->block_id * 2 * block_size + block_size, input_data.end()));
            left_node = std::make_shared<BlockNode>(p_block_node->block_id*2, 0, left_fp);
            right_node = std::make_shared<BlockNode>(p_block_node->block_id*2+1, 0, right_fp);
        }
        else {
            auto left_fp = RabinKarpFingerprint(std::span<const char8_t>(input_data.begin() + p_block_node->block_id * 2 * block_size, input_data.end()));
            left_node = std::make_shared<BlockNode>(p_block_node->block_id*2, 0, left_fp);
        }
        return {left_node, right_node};
    }

public:

    BlockTableBasic(const Sequence &p_input_data) : input_data(p_input_data) {}

    auto init_nodes(size_t p_init_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_init_round;
        size_t num_blocks = 1 << p_init_round;
        auto unmarked_nodes = std::map<size_t, std::shared_ptr<BlockNode>>();

        for(size_t i = 0; i < num_blocks; i++) {
            if(i*block_size >= input_data.size()) break;
            if((i+1)*block_size > input_data.size()) {
                unmarked_nodes[i] = std::make_shared<BlockNode>(i, 0, RabinKarpFingerprint(std::span<const char8_t>(input_data.begin() + i*block_size, input_data.end())));
            }
            else {
                auto block = std::make_shared<BlockNode>(i, 0, RabinKarpFingerprint(std::span<const char8_t>(input_data.data() + i*block_size, block_size)));
                unmarked_nodes[i] = std::move(block);
            }
        }
        return unmarked_nodes;
    }

    auto create_fp_table(std::map<size_t, std::shared_ptr<BlockNode>> &p_unmarked_nodes, size_t p_cur_round) {
        std::unordered_map<size_t, std::set<std::shared_ptr<BlockNode>>> fp_table;
        size_t block_size = std::bit_ceil(input_data.size()) >> p_cur_round;
        for(auto &node : p_unmarked_nodes) {
            if(node.first * block_size + block_size > input_data.size()) break;
            if(!fp_table.contains(node.first)) fp_table[node.first] = std::set<std::shared_ptr<BlockNode>>();
            fp_table[node.second->fp.val].insert(node.second);
        }
        return fp_table;
    }

    auto next_nodes(std::map<size_t, std::shared_ptr<BlockNode>> &p_prev_nodes, std::map<size_t, size_t> &p_chain_ids, size_t p_prev_round) {
        size_t prev_block_size = std::bit_ceil(input_data.size()) >> p_prev_round;
        size_t cur_block_size = prev_block_size >> 1;
        std::map<size_t, std::shared_ptr<BlockNode>> next_unmarked_nodes;

        for(auto it = p_prev_nodes.begin(); it != p_prev_nodes.end(); it++) {
            auto [block_id, block_node] = (*it);

            std::shared_ptr<BlockNode> sibling_node = nullptr;
            if(block_id % 2 == 0) {
                if((++it) != p_prev_nodes.end()) sibling_node = it->second;
                it--;
            }
            else {
                sibling_node = (--it)->second;
                it++;
            }

            bool is_marked = block_node->chain_info & prev_block_size;
            bool is_sibling_marked = sibling_node && sibling_node->chain_info & prev_block_size;
            
            if(is_marked && (!sibling_node || is_sibling_marked)) {
                if(block_id % 2 == 0) {
                    p_chain_ids[block_id * prev_block_size + prev_block_size - 1] = block_node->chain_info;
                }
                else {
                    p_chain_ids[block_id * prev_block_size] = block_node->chain_info;
                }    
            }
            
            if(!is_marked) {
                auto [left_node, right_node] = split_block_node(block_node.get(), cur_block_size);

                if(block_id % 2 == 1) {
                    if(is_sibling_marked) {
                        left_node->chain_info = sibling_node->chain_info;
                    }
                    if(right_node) {
                        right_node->chain_info = block_node->chain_info;
                    }
                }
                else {
                    if(is_sibling_marked) {
                        right_node->chain_info = sibling_node->chain_info;
                    }
                    left_node->chain_info = block_node->chain_info;
                }
                
                if(left_node) next_unmarked_nodes[left_node->block_id] = left_node;
                if(right_node) next_unmarked_nodes[right_node->block_id] = right_node;
            }
            
        }
        return next_unmarked_nodes;
    }

    void match_blocks(size_t p_pos, size_t p_fp, std::unordered_map<size_t, std::set<std::shared_ptr<BlockNode>>> &p_fp_table, std::map<size_t, size_t> &p_marked_refs, size_t p_cur_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_cur_round;
        if(!p_fp_table.contains(p_fp)) return;

        auto match_candidates = &p_fp_table[p_fp];

        for(auto block : (*match_candidates)) {
            if(block->block_id * block_size <= p_pos || block->chain_info & block_size) continue;

            if(std::equal(input_data.begin() + p_pos, input_data.begin() + p_pos + block_size, input_data.begin() + block->block_id * block_size)) {
                block->chain_info |= block_size;
                p_marked_refs[block->block_id*block_size] = p_pos;
            }
        }
    }

    void populate_unmarked_chain(std::map<size_t, std::shared_ptr<BlockNode>> &p_unmarked_nodes, std::map<size_t, size_t> &p_chain_ids, size_t p_round) {
        size_t block_size = std::bit_ceil(input_data.size()) >> p_round;
        for(auto &node : p_unmarked_nodes) {
            p_chain_ids[node.first * block_size] = node.second->chain_info | block_size;
        }
    }
};