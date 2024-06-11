#include "BlockTablePar.hpp"


template <>
std::vector<u_int32_t> BlockTablePar<std::span<const char8_t>>::create_fp_table(ShardedMap &p_fp_table, const std::vector<BlockNode> &p_unmarked_nodes, size_t p_cur_round) {
    size_t block_size = in_ceil_size >> p_cur_round;
    size_t nodes_real_size = p_unmarked_nodes.back().block_id * block_size + block_size > in_size ? p_unmarked_nodes.size() - 1 : p_unmarked_nodes.size();

    std::vector<u_int32_t> ref_table(p_unmarked_nodes.size());
    std::iota(ref_table.begin(), ref_table.end(), 0);    

    p_fp_table.batch_insert(1, nodes_real_size, [&p_unmarked_nodes, &ref_table](const u_int32_t node_id){ return std::pair(p_unmarked_nodes[node_id].fp.val, &ref_table[node_id]); });

    #pragma omp parallel for
    for(size_t i = 1; i < nodes_real_size; i++) {
        if(ref_table[i] < i) ref_table[i] = p_unmarked_nodes[ref_table[i]].block_id * block_size;
        else {
            auto match_it = p_fp_table.find(p_unmarked_nodes[i].fp.val);
            ref_table[i] = p_unmarked_nodes[match_it->second].block_id * block_size;
        }
    }

    return ref_table;
}

template<>
void BlockTablePar<std::span<const char8_t>>::preprocess_matches(u_int32_t p_pos, size_t p_fp, ShardedMap &p_fp_table, std::vector<u_int32_t> &p_ref_table) {
    auto match_it = p_fp_table.find(p_fp);
    if(match_it != p_fp_table.end() && p_ref_table[match_it->second] > p_pos) p_ref_table[match_it->second] = p_pos;
}