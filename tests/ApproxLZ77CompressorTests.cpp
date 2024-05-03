#include "gtest/gtest.h"
#include "ApproxLZ77Compressor.hpp"
#include "StreamView.hpp"
#include "ApproxLZ77BinCoder.hpp"

TEST(ApproxLZ77CompressorTests, CompressEmptyString) {
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "";
    InStreamView sv_in(ss_in);
    ApproxLZ77Encoder encoder(ss_out, sv_in.size());

    ApproxLZ77Compressor::Instance().compress(sv_in, encoder);
    ss_out.clear();
    ss_out.seekg(0);

    ApproxLZ77Decoder decoder2(ss_out);
    OutStreamView sv_out2(ss_out2);
    ApproxLZ77Compressor::Instance().decompress(decoder2, sv_out2);
    EXPECT_TRUE(ss_in.str().compare(ss_out2.str())==0);
}

TEST(ApproxLZ77CompressorTests, CompressText) {
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
             "labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores "
             "et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem "
             "ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et "
             "dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. "
             "Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
    InStreamView sv_in(ss_in);
    ApproxLZ77Encoder encoder(ss_out, sv_in.size());

    ApproxLZ77Compressor::Instance().compress(sv_in, encoder);
    ss_out.clear();
    ss_out.seekg(0);

    ApproxLZ77Decoder decoder2(ss_out);
    OutStreamView sv_out2(ss_out2);
    ApproxLZ77Compressor::Instance().decompress(decoder2, sv_out2);
    EXPECT_TRUE(ss_in.str().compare(ss_out2.str())==0);
}