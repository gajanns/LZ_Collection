#pragma once

#include "Coder.hpp"
#include "Factor.hpp"
#include "BitWriter.hpp"
#include "BitReader.hpp"

class LZWEncoder : public Coder::Encoder<LZW::factor> {
private:
    BitWriter m_bitout;
    size_t m_bytes_written = 0, m_bit_counter = 0;
    size_t dict_size = 256;

public:
    LZWEncoder(std::ostream &p_out): m_bitout(BitWriter(p_out)){};
    ~LZWEncoder(){};

    void encode(LZW::factor p_value) {
        size_t bit_size = 1, tmp = dict_size++;
        while(tmp >>= 1) bit_size++;
        m_bitout.writeFrom(p_value, bit_size);
        m_bit_counter += bit_size;
        m_bytes_written += m_bit_counter/8;
        m_bit_counter %= 8;
    }

    size_t bytes_written() {
        return m_bytes_written;
    }

    void flush() {
        m_bitout.flush();
    }
};

class LZWDecoder : public Coder::Decoder<LZW::factor> {
private:
    BitReader m_bitin;
    size_t m_bytes_read = 0, m_bit_counter = 0, dict_size = 256;

public:
    LZWDecoder(std::istream &p_in): m_bitin(BitReader(p_in)){};
    ~LZWDecoder(){};

    LZW::factor decode() {
        size_t bit_size = 1, tmp = dict_size++;
        while(tmp >>= 1) bit_size++;
        LZW::factor id;
        auto bits_read = m_bitin.readInto(id, bit_size);
        m_bit_counter += bits_read;
        m_bytes_read += m_bit_counter/8;
        m_bit_counter %= 8;
        return bit_size == bits_read ? id : -1;
    }

    size_t bytes_read() {
        return m_bytes_read;
    }

    bool reached_end() {
        return m_bitin.reached_end();
    }
};