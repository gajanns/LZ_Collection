#include "gtest/gtest.h"
#include "BitWriter.hpp"
#include "BitReader.hpp"
#include <fstream>

TEST(BitWriterTest, WriteInt) {
    std::stringstream ss;
    BitWriter writer(ss);
    writer.writeFrom(170, 8);
    writer.flush();
    EXPECT_EQ((char)0b10101010, ss.str()[0]);
}

TEST(BitReaderTest, ReadInt) {
    std::ofstream ofile("test");
    BitWriter writer(ofile);
    writer.writeFrom(0x1234,5);
    writer.writeFrom(0x5678,7);
    writer.flush();
    ofile.close();

    std::ifstream ifile("test");
    BitReader reader(ifile);
    size_t in;
    reader.readInto(in, 12);
    EXPECT_EQ(in,(size_t)0xA78);
}
