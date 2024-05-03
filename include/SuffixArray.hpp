#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include <numeric>
#include "Definition.hpp"

/**
 * Suffix Array by Induced Sorting
 * Modified from Source: https://www.rahmannlab.de/lehre/alsa21/02-3-sais.pdf
 * ToDo: Waste less space => Optimal Time and Space Construction of Suffix Arrays and LCP Arrays for Integer Alphabets[https://arxiv.org/pdf/1703.01009.pdf]
*/
namespace SuffixArray
{
    enum LS_Type:bool {S, L};

    std::vector<int> generate_suffix_array(const NumRange auto &p_value, size_t alphabet_size);

    /**
     * @brief Generate Suffix Array using Naive Sorting
     * 
     * @param p_value Data-Sequence to generate Suffix Array from(without sentinel)
     * @return std::vector<int> Suffix Array
     */
    inline std::vector<int> generate_suffixarray_naive(NumRange auto p_value) {
    
        std::vector<decltype(p_value)> suffixes;
        std::vector<int> result;
        result.reserve(p_value.size()+1);
        result.push_back(p_value.size());

        for(auto it = p_value.begin(); it != p_value.end(); it++){
            decltype(p_value) suffix(it, p_value.end());
            suffixes.push_back(suffix);
        }
        std::sort(suffixes.begin(), suffixes.end());
        for(auto i: suffixes){
            result.push_back(p_value.size()-i.size());
        }
        return result;
    }

    /**
     * @brief Preprocess Suffix Array using LMS Substrings
     * 
     * @param p_sa Suffix Array
     * @return std::vector<int> Initial Suffix Array
     */
    void estimate_sa(const NumRange auto &p_value, std::vector<size_t> p_bucket_sizes, 
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

    /**
     * @brief Fill L-Type Suffixes into Buckets of Suffix Array
     * 
     * @param p_value Data-Sequence
     * @param p_bucket_sizes Bucket Sizes defined by first character
     * @param p_types Type-Sequence of Data-Sequence
     * @param p_sa Modified Suffix Array (Output)
     */
    void induce_sort_L(const NumRange auto &p_value, std::vector<size_t> p_bucket_sizes, std::vector<int> &p_sa) {
        
        for(auto p : p_sa) {
            if(p > 0 && (p == static_cast<int>(p_value.size()) || p_value[p-1] >= p_value[p])) {
                p_sa[(p_bucket_sizes[p_value[p-1]-1]++)] = p-1;
            }
        }
    }

    /**
     * @brief Fill S-Type Suffixes into Buckets of Suffix Array
     * 
     * @param p_value Data-Sequence
     * @param p_bucket_sizes Bucket Sizes defined by first character
     * @param p_types Type-Sequence of Data-Sequence
     * @param p_sa Modified Suffix Array (Output)
     */
    void induce_sort_S(const NumRange auto &p_value, std::vector<size_t> p_bucket_sizes, std::vector<int> &p_sa) {
        size_t id = p_value.size()-1;
        for(auto p : p_sa | std::ranges::views::reverse) {
            if(p > 0 && p < static_cast<int>(p_value.size()) && ( p_value[p-1] < p_value[p] || (p_value[p-1] == p_value[p] && p_bucket_sizes[p_value[p-1]-1] <= id))) {
                p_sa[(--p_bucket_sizes[p_value[p-1]])] = p-1;
            }
            id--;
        }
    }

    void phase2(const NumRange auto &p_value, std::vector<size_t> &p_bucket_sizes, std::vector<size_t> const &p_lms_positions, std::vector<int> &p_sa) {
        
        estimate_sa(p_value, p_bucket_sizes, p_lms_positions, p_sa);
        induce_sort_L(p_value, p_bucket_sizes, p_sa);
        induce_sort_S(p_value, p_bucket_sizes, p_sa);
    }

    /**
     * @brief Compare two LMS Substrings
     * 
     * @param p_value Data-Sequence
     * @param p_types Type-Sequence
     * @param p_pos1 Offset of first LMS Substring
     * @param p_pos2 Offset of second LMS Substring
     * @return true LMS Substrings are unequal
     * @return false LM Substrings are equal
     */
    bool lms_substrings_unequal(const NumRange auto &p_value, std::vector<LS_Type> const &p_types, size_t p_pos1, size_t p_pos2) {
        if(p_pos1 == p_pos2) {
            return false;
        }

        bool is_lms1 = false, is_lms2 = false;
        size_t i = 0;
        while(true) {
            if (std::max(p_pos1+i, p_pos2+i) == p_value.size() || p_value[p_pos1+i] != p_value[p_pos2+i] || p_types[p_pos1+i] != p_types[p_pos2+i]) {
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

    /**
     * @brief Reduce Data-Sequence into Sequence of LMS Substrings
     * 
     * @param p_value Data-Sequence
     * @param p_types Type-Sequence
     * @param p_sa Current Suffix Array of Data-Sequence
     * @param p_lms_positions Sequence of LMS Positions
     * @return std::tuple<std::vector<int>, size_t, std::vector<int>> {Reduced Data-Sequence, Reduced Alphabet Size, Position Map}
     */
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
        
        phase2(p_value, p_bucket_sizes, p_lms_positions, p_sa);

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

    /**
     * @brief Generate Suffix Array using Induced Sorting
     * 
     * @param p_value Data-Sequence to generate Suffix Array from(without Sentinel)
     * @param alphabet_size Size of Alphabet(without Sentinel)
     * @return std::vector<int> Suffix Array
     */
    std::vector<int> generate_suffix_array(const NumRange auto &p_value, size_t alphabet_size) {
        
        std::vector<int> result(p_value.size()+1, -1);
        std::vector<LS_Type> types(p_value.size()+1, S);
        std::vector<size_t> bucket_sizes(alphabet_size+1, 0);
        std::vector<size_t> lms_positions;

        /* #region Compute Types (Sentinel not in p_value) */
        for(int i = p_value.size()-1; i >= 0; i--) {
            if( i == static_cast<int>(p_value.size())-1) {
                types[i] = L;
                continue;
            }
            types[i] = (p_value[i] < p_value[i+1]) ? S : (p_value[i] > p_value[i+1]) ? L : types[i+1];
        }
        /* #endregion */

        /* #region Compute Bucket-Sizes (Sentinel not in Alphabet) */
        std::for_each(p_value.begin(), p_value.begin()+p_value.size(), [&bucket_sizes](auto c) {
            bucket_sizes[c]++;
        });
        bucket_sizes[0] = 1; // Sentinel

        std::inclusive_scan(bucket_sizes.begin(), bucket_sizes.end(), bucket_sizes.begin());
        /* #endregion */

        /* #region Compute LMS Positions */
        for(size_t i = 1; i < types.size(); i++) {
            if(types[i] == S && types[i-1] == L) {
                lms_positions.push_back(i);
            }
        }
       /* #endregion */

        phase1(p_value, bucket_sizes, types, lms_positions, result);
        phase2(p_value, bucket_sizes, lms_positions, result);
        return result;
    }

    /**
     * @brief Generate Inverse Suffix Array
     * 
     * @param p_sa Suffix Array
     * @return std::vector<int> Inverse Suffix Array
     */
    inline std::vector<int> invert_suffix_array(const std::vector<int> &p_sa) {
        
        std::vector<int> result(p_sa.size(), -1);
        for(size_t i = 0; i < p_sa.size(); i++) {
            result[p_sa[i]] = i;
        }
        return result;
    }
}
