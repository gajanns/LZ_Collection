#pragma once

#include <span>
#include <unordered_map>
#include <bit>
#include <set>
#include "Definition.hpp"
#include "RabinKarp.hpp"

template<typename Sequence> requires NumRange<Sequence>
class BlockTableSTD {
    using Item = typename Sequence::value_type;

private:
    Sequence input_data;

    std::unordered_map<size_t, RabinKarpFingerprint> unmarked_blocks;

    struct pair_cmp {
        bool operator()(const std::pair<size_t, ApproxLZ77::factor_id> &a, const std::pair<size_t, ApproxLZ77::factor_id> &b) const {
            return a.first < b.first;
        }
    };
    std::set<std::pair<size_t, ApproxLZ77::factor_id>, pair_cmp> factor_phrases;

    std::unordered_map<RabinKarpFingerprint, std::set<size_t> , RabinKarpFingerprint::HashOp, RabinKarpFingerprint::EqualOp> fp_block_table;

public:
    size_t round, block_size, log_block_size;

    BlockTableSTD(Sequence &p_input): input_data(p_input), round(0), block_size(std::bit_ceil(input_data.size())), log_block_size(std::bit_width(block_size)) {};
    ~BlockTableSTD() = default;

    auto zero_fp() {
        if(unmarked_blocks.empty()) {
            auto tmp = std::span(input_data.data(), 0);
            return RabinKarpFingerprint(tmp);
        }
        return unmarked_blocks.at(0);
    }

    void next_block_table(){
        round++;
        block_size >>= 1;
        log_block_size--;
        fp_block_table.clear();
        
        if(round == 1) {
            auto left_fp = RabinKarpFingerprint(std::span(input_data.data(), block_size));
            auto right_fp = RabinKarpFingerprint(std::span(input_data.begin() + block_size, input_data.end()));
            unmarked_blocks.insert({0, left_fp});
            unmarked_blocks.insert({1, right_fp});
            
            fp_block_table[left_fp].insert(0);
            if(2*block_size <= input_data.size()) {
                fp_block_table[right_fp].insert(1);
            }
            return;
        }
        
        std::unordered_map<size_t, RabinKarpFingerprint> new_unmarked_blocks;
        new_unmarked_blocks.reserve(unmarked_blocks.size()*2);

        for(auto p: unmarked_blocks){

            if(2*p.first*block_size + 2*block_size <= input_data.size()) {
                auto [tmp_fp1, tmp_fp2] = p.second.split(std::span(input_data.data() + p.first*2*block_size, 2*block_size), block_size);

                fp_block_table[tmp_fp1].insert(p.first*2);
                fp_block_table[tmp_fp2].insert(p.first*2+1);
                new_unmarked_blocks.insert({p.first*2, tmp_fp1});
                new_unmarked_blocks.insert({p.first*2+1, tmp_fp2});
            }
            else if(2*p.first*block_size + block_size <= input_data.size()) {
                auto tmp_fp1 = RabinKarpFingerprint(std::span(input_data.data() + p.first*2*block_size, block_size));
                auto tmp_fp2 = RabinKarpFingerprint(std::span(input_data.begin() + p.first*2*block_size + block_size, input_data.end()));

                fp_block_table[tmp_fp1].insert(p.first*2);
                new_unmarked_blocks.insert({p.first*2, tmp_fp1});
                new_unmarked_blocks.insert({p.first*2+1, tmp_fp2});
            }
            else if(p.first*2*block_size <= input_data.size()) {
                auto tmp_fp1 = RabinKarpFingerprint(std::span(input_data.begin() + p.first*2*block_size, input_data.end()));

                new_unmarked_blocks.insert({p.first*2, tmp_fp1});
            }
        }
        std::swap(unmarked_blocks, new_unmarked_blocks);
    }

    void add_factor_phrase(size_t p_pos, size_t p_block){

        factor_phrases.insert({p_block*block_size, ApproxLZ77::factor_id{.value = p_pos, .log_length = log_block_size}});
    }

    void add_factor_singular(size_t p_block){
        factor_phrases.insert({p_block*block_size, ApproxLZ77::factor_id{.value = input_data[p_block*block_size], .log_length = 0}});
    }

    void match_block(size_t p_pos, RabinKarpFingerprint p_pattern_fp){
        auto it = fp_block_table.find(p_pattern_fp);
        if(it != fp_block_table.end()){
            std::vector<size_t> to_erase;
            for(auto block: it->second){
                if(p_pos < block*block_size && std::equal(input_data.begin()+p_pos, input_data.begin()+ p_pos + block_size, input_data.begin()+block*block_size)){
                    unmarked_blocks.erase(block);
                    add_factor_phrase(p_pos, block);
                    to_erase.push_back(block);
                }
            }
            for(auto block: to_erase){
                it->second.erase(block);
            }
        }
    }

    void insert_residuals(){
        if(block_size != 1) {
            std::cerr << "Error: residual block size is not 1" << std::endl;
            exit(EXIT_FAILURE);
        }
        for(auto p: unmarked_blocks){
            if(p.first*block_size < input_data.size()){
                add_factor_singular(p.first);
            }
        }
    }

    auto factors_begin_iter(){
        return factor_phrases.cbegin();
    }

    auto factors_end_iter(){
        return factor_phrases.cend();
    }
};