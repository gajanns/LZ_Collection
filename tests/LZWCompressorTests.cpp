#include "gtest/gtest.h"
#include "LZWCompressor.hpp"
#include "StreamView.hpp"
#include "LZWBinCoder.hpp"

TEST(LZWCompressorTests, CompressEmptyString) {
    LZWCompressor compressor;
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "";
    StreamView sv_in(ss_in,ss_in);
    LZWEncoder encoder(ss_out);

    compressor.compress(sv_in, encoder);
    ss_out.clear();
    ss_out.seekg(0);

    LZWDecoder decoder2(ss_out);
    StreamView sv_out2(ss_out2, ss_out2);
    compressor.decompress(decoder2, sv_out2);
    EXPECT_TRUE(ss_in.str().compare(ss_out2.str())==0);
}

TEST(LZWCompressorTests, CompressText) {
    LZWCompressor compressor;
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
             "labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores "
             "et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem "
             "ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et "
             "dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. "
             "Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
    StreamView sv_in(ss_in,ss_in);
    LZWEncoder encoder(ss_out);

    compressor.compress(sv_in, encoder);
    ss_out.clear();
    ss_out.seekg(0);

    LZWDecoder decoder2(ss_out);
    StreamView sv_out2(ss_out2, ss_out2);
    compressor.decompress(decoder2, sv_out2);

    std::string str_in = ss_in.str();
    std::string str_out2 = ss_out2.str();
    EXPECT_TRUE(ss_in.str().compare(ss_out2.str())==0);
}