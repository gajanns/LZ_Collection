#include "gtest/gtest.h"
#include "LZ77Compressor.hpp"
#include "StreamView.hpp"
#include "MemoryTracker.hpp"
#include "LZ77BinCoder.hpp"

TEST(StatisticsTests, FactorCountEmptyString) {
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "";
    InStreamView sv_in(ss_in);
    LZ77Encoder encoder(ss_out, sv_in.size());

    LZ77Compressor::Instance().compress(sv_in, encoder);
    ss_out.clear();
    ss_out.seekg(0);
    EXPECT_TRUE(LZ77Compressor::Instance().m_stats.m_factor_count == 0);

    LZ77Decoder decoder2(ss_out);
    OutStreamView sv_out2(ss_out2);
    LZ77Compressor::Instance().decompress(decoder2, sv_out2);
    EXPECT_TRUE(LZ77Compressor::Instance().m_stats.m_factor_count == 0);
}

TEST(StatisticsTests, FactorCountText) {
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "abaababaab";
    InStreamView sv_in(ss_in);
    LZ77Encoder encoder(ss_out, sv_in.size());
    LZ77Compressor::Instance().compress(sv_in, encoder);
    EXPECT_TRUE(LZ77Compressor::Instance().m_stats.m_factor_count == 5);
    ss_out.clear();
    ss_out.seekg(0);

    LZ77Decoder decoder2(ss_out);
    OutStreamView sv_out2(ss_out2);
    LZ77Compressor::Instance().decompress(decoder2, sv_out2);
    EXPECT_TRUE(LZ77Compressor::Instance().m_stats.m_factor_count == 5);
}

TEST(StatisticsTests, OutputSizeText) {
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
            "labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores "
            "et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem "
            "ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et "
            "dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. "
            "Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
    InStreamView sv_in(ss_in);
    LZ77Encoder encoder(ss_out, sv_in.size());
    LZ77Compressor::Instance().compress(sv_in, encoder);
    EXPECT_TRUE(LZ77Compressor::Instance().m_stats.m_output_size == ss_out.str().size());
}

#ifdef PERF
TEST(StatisticsTests, VectorMemory) {
    MemoryTracker::start_mem_record();
    std::vector<u_char> v(256);
    MemoryTracker::stop_mem_record();
    EXPECT_EQ(MemoryTracker::peak_mem_usage, 256);
}
#endif