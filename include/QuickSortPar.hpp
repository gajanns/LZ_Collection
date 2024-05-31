#pragma once

/*####################################################################################################################################*/
// Reference:                                                                                                                         //
// - Parallel QuickSort taken from https://www.openmp.org/wp-content/uploads/sc16-openmp-booth-tasking-ruud.pdf from Ruud van der Pas //
/*####################################################################################################################################*/

#include <omp.h>
#include <algorithm>
#include <ranges>
#include <span>

namespace SortPar {
    const size_t par_limit = 100;

    template<std::ranges::range R>
    size_t partition(R& p_input)
    {
        using Item = typename R::value_type;
        Item pivot = p_input[0];
    
        size_t pivotIndex = 0;
        for (Item val : p_input | std::views::drop(1)) {
            if (val <= pivot) pivotIndex++;
        }
    
        std::swap(p_input[pivotIndex], p_input[0]);

        size_t i = 0, j = p_input.size() - 1; 
        while (i < pivotIndex && j > pivotIndex) {    
            while (p_input[i] <= pivot) i++; 
            while (p_input[j] > pivot) j--; 
            if (i < pivotIndex && j > pivotIndex) std::swap(p_input[i++], p_input[j--]);
        } 
        return pivotIndex;
    }

    template<std::ranges::range R>
    void quicksort_par(R& p_input) {
        using Item = typename R::value_type;

        if (p_input.size() < par_limit) {
            std::sort(p_input.begin(), p_input.end());
            return;
        }

        size_t partition_index = partition(p_input);
        std::span<Item> left, right;

        if(partition_index > 0) {
            #pragma omp task shared(p_input, partition_index) firstprivate(left)
            {
                left = std::span(p_input.data(), partition_index);
                quicksort_par(left);
            }
        }

        if(partition_index < p_input.size() - 1) {
            #pragma omp task shared(p_input, partition_index) firstprivate(right)
            {
                right = std::span(p_input.begin() + partition_index + 1, p_input.end());
                quicksort_par(right);
            }
        }

        #pragma omp taskwait    
    }
}