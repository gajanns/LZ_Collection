#include "gtest/gtest.h"
#include "SuffixArray.hpp"

TEST(SuffixArrayTest, SuffixArrayNaive) {
    std::string test = "banana";
    std::u8string_view test_view = std::u8string_view(reinterpret_cast<const char8_t*>(test.data()), test.size());
    std::vector<int> result = SuffixArray::generate_suffixarray_naive(test_view);
    std::vector<int> expected = {6, 5, 3, 1, 0, 4, 2};
    EXPECT_EQ(result, expected);
}

TEST(SuffixArrayTest, SuffixArrayIS) {
    std::string test = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy";
    std::u8string_view test_view = std::u8string_view(reinterpret_cast<const char8_t*>(test.data()), test.size());
    
    std::vector<int> result = SuffixArray::generate_suffix_array(test_view, 255);   
    std::vector<int> expected = SuffixArray::generate_suffixarray_naive(test_view);
    EXPECT_EQ(result, expected);
}