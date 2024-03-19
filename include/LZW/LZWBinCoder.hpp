#pragma once

#include "Coder.hpp"
#include "Definition.hpp"
#include "BitWriter.hpp"
#include "BitReader.hpp"

/**
 * @brief Encoder of LZW-Factors into binary format
 * 
 */
class LZWEncoder : public Coder::Encoder<LZW::factor_id> {
private:
    BitWriter m_bitout;
    size_t m_bytes_written = 0, m_bit_counter = 0;
    size_t dict_size = LZW::initial_dict_size;

public:
    LZWEncoder(std::ostream &p_out): m_bitout(BitWriter(p_out)){};
    ~LZWEncoder(){};

    int encode(LZW::factor_id p_value) {
        size_t bit_size = 1, tmp = dict_size++;
        while(tmp >>= 1) bit_size++;
        if(m_bitout.writeFrom(p_value, bit_size) != bit_size) {
            return 0;
        }
        m_bit_counter += bit_size;
        m_bytes_written += m_bit_counter/8;
        m_bit_counter %= 8;
        return 1;
    }

    size_t bytes_written() {
        return m_bytes_written;
    }

    void flush() {
        if(m_bit_counter) m_bitout.flush();
    }
};

class LZWDecoder : public Coder::Decoder<LZW::factor_id> {
private:
    BitReader m_bitin;
    size_t m_bytes_read = 0, m_bit_counter = 0, dict_size = LZW::initial_dict_size;

public:
    LZWDecoder(std::istream &p_in): m_bitin(BitReader(p_in)){};
    ~LZWDecoder(){};

    int decode(LZW::factor_id &p_value) {
        size_t bit_size = 1, tmp = dict_size++;
        while(tmp >>= 1) bit_size++;
        auto bits_read = m_bitin.readInto(p_value, bit_size);
        m_bit_counter += bits_read;
        m_bytes_read += m_bit_counter/8;
        m_bit_counter %= 8;
        return bit_size == bits_read;
    }

    size_t bytes_read() {
        return m_bytes_read;
    }

    bool reached_end() {
        return m_bitin.reached_end();
    }
};