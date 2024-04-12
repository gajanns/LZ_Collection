#include "gtest/gtest.h"
#include "SuffixArray.hpp"

TEST(SuffixArrayTest, SuffixArrayNaive) {
    std::string test = "banana";
    std::vector<int> result = SuffixArray::generate_suffixarray_naive(test);
    std::vector<int> expected = {6, 5, 3, 1, 0, 4, 2};
    EXPECT_EQ(result, expected);
}

TEST(SuffixArrayTest, SuffixArrayIS) {
    std::string test = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy";
    std::vector<int> result = SuffixArray::generate_suffix_array(test, 255);
    std::vector<int> expected = SuffixArray::generate_suffixarray_naive(test);
    EXPECT_EQ(result, expected);
}