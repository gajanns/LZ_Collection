#pragma once

#include "Coder.hpp"
#include "Definition.hpp"
#include "BitWriter.hpp"
#include "BitReader.hpp"

class LZ77Encoder : public Coder::Encoder<LZ77_Wndw::factor_id> {
private:
    BitWriter m_bitout;
    size_t m_bytes_written = 0, m_bit_counter = 0;
    const size_t offset_bit_size = log2(LZ77_Wndw::preview_buffer_size);
    const size_t length_bit_size = log2(LZ77_Wndw::window_size);
public:
    LZ77Encoder(std::ostream &p_out): m_bitout(BitWriter(p_out)){};
    ~LZ77Encoder(){};

    int encode_impl(LZ77_Wndw::factor_id p_value) {
        if(m_bitout.writeFrom(p_value.offset, offset_bit_size) != offset_bit_size ||
           m_bitout.writeFrom(p_value.length, length_bit_size) != length_bit_size ||
           m_bitout.writeFrom(p_value.next_char, 8) != 8) {
            return 0;
        }
        m_bit_counter += offset_bit_size + length_bit_size + 8;
        m_bytes_written += m_bit_counter/8;
        m_bit_counter %= 8;
        return 1;
    }

    size_t bytes_written() {
        return m_bytes_written;
    }

    void flush() {
        if(m_bit_counter) {m_bitout.flush();m_bytes_written++;}
    }
};

class LZ77Decoder : public Coder::Decoder<LZ77_Wndw::factor_id> {
private:
    BitReader m_bitin;
    size_t m_bytes_read = 0, m_bit_counter = 0;
    const size_t offset_bit_size = log2(LZ77_Wndw::preview_buffer_size);
    const size_t length_bit_size = log2(LZ77_Wndw::window_size);
public:
    LZ77Decoder(std::istream &p_in): m_bitin(BitReader(p_in)){};
    ~LZ77Decoder(){};

    int decode_impl(LZ77_Wndw::factor_id &p_value) {
        if(m_bitin.readInto(p_value.offset, offset_bit_size) != offset_bit_size ||
           m_bitin.readInto(p_value.length, length_bit_size) != length_bit_size ||
           m_bitin.readInto(p_value.next_char, 8) != 8) {
            return 0;
        }
        m_bit_counter += offset_bit_size + length_bit_size + 8;
        m_bytes_read += m_bit_counter/8;
        m_bit_counter %= 8;
        return 1;
    }

    size_t bytes_read() {
        return m_bytes_read;
    }

    bool reached_end() {
        return m_bitin.reached_end();
    }
};