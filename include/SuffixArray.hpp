#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include <numeric>
#include "Definition.hpp"

/**
 * Suffix Array by induced sorting
 * https://zork.net/~st/jottings/sais.html
 * https://www.rahmannlab.de/lehre/alsa21/02-3-sais.pdf
*/

namespace SuffixArray
{
    enum LS_Type {S, L};
    std::vector<int> generate_suffix_array(const NumRange auto &p_value, size_t alphabet_size);

    std::vector<int> generate_suffixarray_naive(const std::string &p_value) {
    
        std::vector<std::string> suffixes;
        suffixes.push_back("");
        for(auto it = p_value.begin(); it != p_value.end(); it++){
            suffixes.push_back(std::string{it, p_value.end()});
        }

        std::vector<int> result;
        std::sort(suffixes.begin(), suffixes.end());
        for(auto i: suffixes){
            result.push_back(p_value.size()-i.size());
        }

        return result;
    }

    std::vector<SuffixArray::LS_Type> compute_types(const NumRange auto &p_value) {
        
        std::vector<LS_Type> result(p_value.size()+1, S);
        
        for(int i = p_value.size()-1; i >= 0; i--) {
            if( i == static_cast<int>(p_value.size())-1) {
                result[i] = L;
                continue;
            }
            result[i] = (p_value[i] < p_value[i+1]) ? S : (p_value[i] > p_value[i+1]) ? L : result[i+1];
        }

        return result;
    }

    std::vector<size_t> count_cumulative_characters(const NumRange auto &p_value, size_t alphabet_size) {
        
        std::vector<size_t> result(alphabet_size+1, 0);

        std::for_each(p_value.begin(), p_value.begin()+p_value.size(), [&result](auto c) {
            result[c]++;
        });
        result[0] = 1; // Sentinel

        std::inclusive_scan(result.begin(), result.end(), result.begin());
        return result;
    }

    std::vector<size_t> find_lms_positions(std::vector<LS_Type> &p_type) {
        
        std::vector<size_t> result;
        for(size_t i = 1; i < p_type.size(); i++) {
            if(p_type[i] == S && p_type[i-1] == L) {
                result.push_back(i);
            }
        }
        return result;
    }

    void initialize_pos_from_lms(const NumRange auto &p_value, std::vector<size_t> p_bucket_sizes, 
    std::vector<size_t> const &p_lms_positions, std::vector<int> &p_sa) {
        
        std::fill(p_sa.begin(), p_sa.end(), -1);

        for( auto p : p_lms_positions | std::ranges::views::reverse) {
            if(p == p_value.size()) {
                p_sa[0] = p;
                continue;
            }
            p_sa[--p_bucket_sizes[p_value[p]]] = p;
        }
    }

    void induce_sort_L(const NumRange auto &p_value, std::vector<size_t> p_bucket_sizes, std::vector<LS_Type> const &p_types, std::vector<int> &p_sa) {
        
        for( auto p : p_sa) {
            if(p > 0 && p_types[p-1] == L) {
                p_sa[(p_bucket_sizes[p_value[p-1]-1]++)] = p-1;
            }
        }
    }

    void induce_sort_S(const NumRange auto &p_value, std::vector<size_t> p_bucket_sizes, std::vector<LS_Type> const &p_types, std::vector<int> &p_sa) {
        
        for( auto p : p_sa | std::ranges::views::reverse) {
            if(p > 0 && p_types[p-1] == S) {
                p_sa[(--p_bucket_sizes[p_value[p-1]])] = p-1;
            }
        }
    }

    void phase2(const NumRange auto &p_value, std::vector<size_t> &p_bucket_sizes, 
    std::vector<LS_Type> const &p_types, std::vector<size_t> const &p_lms_positions, std::vector<int> &p_sa) {
        
        initialize_pos_from_lms(p_value, p_bucket_sizes, p_lms_positions, p_sa);
        induce_sort_L(p_value, p_bucket_sizes, p_types, p_sa);
        induce_sort_S(p_value, p_bucket_sizes, p_types, p_sa);
    }

    bool lms_substrings_unequal(const NumRange auto &p_value, std::vector<LS_Type> const &p_types, size_t p_pos1, size_t p_pos2) {
        
        bool is_lms1 = false, is_lms2 = false;
        size_t i = 0;
        while(true) {
            if (p_value[p_pos1+i] != p_value[p_pos2+i] || p_types[p_pos1+i] != p_types[p_pos2+i]) {
                return true;
            }
            if(is_lms1 && is_lms2) {
                return false;
            }
            i++;

            is_lms1 = p_types[p_pos1+i] == S && p_types[p_pos1+i-1] == L;
            is_lms2 = p_types[p_pos2+i] == S && p_types[p_pos2+i-1] == L;

            if(is_lms1 != is_lms2) {
                return true;
            }
        }
    }

    std::tuple<std::vector<int>, size_t, std::vector<int>> reduce_text(const NumRange auto &p_value, std::vector<LS_Type> const &p_types, std::vector<int> &p_sa, std::vector<size_t> &p_lms_positions) {
        std::vector<int> lms_names(p_sa.size()-1, -1);
        size_t last_lms = lms_names.size();
        size_t reduced_alphabet_size = 0, j = 0;

        for( auto p : p_sa) {
            if(p == static_cast<int>(p_value.size()) || p <= 0 || p_types[p] != S || p_types[p-1] != L) {
                continue;
            }

            p_lms_positions[j++] = p;
            if(lms_substrings_unequal(p_value, p_types, last_lms, p)) {
                reduced_alphabet_size++;
            }
            lms_names[p] = reduced_alphabet_size;
            last_lms = p;
            
        }

        std::vector<int> reduced_string;
        std::vector<int> position_map;
        
        size_t id = 0;
        for(auto name: lms_names) {
            if(name != -1) {
                reduced_string.push_back(name);
                position_map.push_back(id);
            }
            id++;
        }
        position_map.push_back(p_value.size());

        return {reduced_string, reduced_alphabet_size, position_map};
    }

    void phase1(const NumRange auto &p_value, std::vector<size_t> &p_bucket_sizes, std::vector<LS_Type> &p_types, std::vector<size_t> &p_lms_positions, std::vector<int> &p_sa) {
        
        phase2(p_value, p_bucket_sizes, p_types, p_lms_positions, p_sa);

        auto [reduced_string, reduced_alphabet_size, reduced_lms_positions] = 
        reduce_text(p_value, p_types, p_sa, p_lms_positions);

        if(reduced_string.size() != reduced_alphabet_size) {
            auto reduced_sa = generate_suffix_array(reduced_string, reduced_alphabet_size);

            size_t id = 0;
            for(auto pos: reduced_sa) {
                p_lms_positions[id++] = reduced_lms_positions[pos];
            }
        }
    }

    std::vector<int> generate_suffix_array(const NumRange auto &p_value, size_t alphabet_size) {
        
        std::vector<int> result(p_value.size()+1, -1);
        std::vector<LS_Type> types = compute_types(p_value);
        std::vector<size_t> bucket_sizes = count_cumulative_characters(p_value, alphabet_size);
        std::vector<size_t> lms_positions = find_lms_positions(types);

        phase1(p_value, bucket_sizes, types, lms_positions, result);
        phase2(p_value, bucket_sizes, types, lms_positions, result);
        return result;
    }

    std::vector<int> invert_suffix_array(const std::vector<int> &p_sa) {
        
        std::vector<int> result(p_sa.size(), -1);
        for(size_t i = 0; i < p_sa.size(); i++) {
            result[p_sa[i]] = i;
        }
        return result;
    }
}
