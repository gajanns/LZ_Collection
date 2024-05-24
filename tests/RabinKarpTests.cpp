#include "gtest/gtest.h"
#include "RabinKarp.hpp"

TEST(RabinKarpTest, SimpleString) {
    std::string s{"One"};
    RabinKarpFingerprint fp(s);
    EXPECT_EQ(fp.val, 339112912802);
}

TEST(RabinKarpTest, ConcatString) {
    std::string s1{"One"};
    std::string s2{"Two"};
    std::string s = s1+s2;

    RabinKarpFingerprint fp1(s1), fp2(s2), fp(s), fp_concat = fp1+fp2;
    EXPECT_EQ(fp1.val, 339112912802);
    EXPECT_EQ(fp2.val, 360575888910);
    EXPECT_EQ(fp.val, 1834056854850233761);
    EXPECT_EQ(fp.val, fp_concat.val);
}

TEST(RabinKarpTest, ShiftRight) {
    std::string s{"OneTwo"}, s_ref{"neTwoO"};
    RabinKarpFingerprint fp(s), fp_ref(s_ref), fp_shift = 'O' << fp << 'O';
    EXPECT_EQ(fp_ref.val, fp_shift.val);
}

TEST(RabinKarpTest, ShiftRightAlt) {
    std::string s{"OneTwo"}, s_ref{"neTwoO"};
    RabinKarpFingerprint fp(s), fp_ref(s_ref), fp_shift = fp_ref;
    fp_shift.roll('O', 'O');
    EXPECT_EQ(fp_ref.val, fp_shift.val);
}

TEST(RabinKarpTest, SplitString) {
    std::string s1{"One"};
    std::string s2{"Two"};
    std::string s = s1+s2;

    RabinKarpFingerprint fp1(s1), fp2(s2), fp_concat(s);
    auto [fp1_, fp2_] = fp_concat.split(s, s1.size());
    EXPECT_EQ(fp1.val, fp1_.val);
    EXPECT_EQ(fp2.val, fp2_.val);
}