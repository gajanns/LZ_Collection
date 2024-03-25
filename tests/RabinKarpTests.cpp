#include "gtest/gtest.h"
#include "RabinKarp.hpp"

TEST(RabinKarpTest, SimpleString) {
    std::string s{"One"};
    RabinKarpFingerprint fp(s);
    EXPECT_EQ(fp.val, 268);
}

TEST(RabinKarpTest, ConcatString) {
    std::string s1{"One"};
    std::string s2{"Two"};
    std::string s = s1+s2;

    RabinKarpFingerprint fp1(s1), fp2(s2), fp(s), fp_concat = fp1+fp2;
    EXPECT_EQ(fp1.val, 268);
    EXPECT_EQ(fp2.val, 255);
    EXPECT_EQ(fp.val, 612);
    EXPECT_EQ(fp.val, fp_concat.val);
}

TEST(RabinKarpTest, ShiftRight) {
    std::string s{"OneTwo"}, s_ref{"neTwoO"};
    RabinKarpFingerprint fp(s), fp_ref(s_ref), fp_shift = 'O' << fp << 'O';
    EXPECT_EQ(fp_ref.val, fp_shift.val);
}