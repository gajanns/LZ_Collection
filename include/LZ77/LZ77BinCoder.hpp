#pragma once

#include "Coder.hpp"
#include "Definition.hpp"
#include "BitWriter.hpp"
#include "BitReader.hpp"
#include <bit>

class LZ77Encoder : public Coder::Encoder<LZ77::factor_id> {
private:
    BitWriter m_bitout;
    size_t m_bytes_written = 0, m_bit_counter = 0;
    const size_t factor_bit_size;
public:
    LZ77Encoder(std::ostream &p_out, size_t input_size): m_bitout(BitWriter(p_out)), factor_bit_size(std::bit_width(input_size)){

        if(m_bitout.writeFrom(factor_bit_size, 8) != 8) {
            throw std::runtime_error("Error while writing to output stream");
        }
        m_bytes_written++;

        #if DYNAMIC_GRANULARITY == true
            LZ77::min_ref_size = (2 * factor_bit_size + 9) / 9;
        #endif
    };

    int encode_impl(LZ77::factor_id p_value) {

        if(p_value.length == 0){
            if(m_bitout.writeFrom(0, 1) != 1 ||
               m_bitout.writeFrom(std::get<char8_t>(p_value.value), 8) != 8) {
                return 0;
            }
            m_bit_counter += 9;
        }
        else {
            if(m_bitout.writeFrom(1, 1) != 1 ||
                m_bitout.writeFrom(std::get<size_t>(p_value.value), factor_bit_size) != factor_bit_size ||
                m_bitout.writeFrom(p_value.length, factor_bit_size) != factor_bit_size) {
                 return 0;
            }
            m_bit_counter += 1 + factor_bit_size*2;
        }

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

class LZ77Decoder : public Coder::Decoder<LZ77::factor_id> {
private:
    BitReader m_bitin;
    size_t m_bytes_read = 0, m_bit_counter = 0;
    size_t factor_bit_size;
public:
    LZ77Decoder(std::istream &p_in): m_bitin(BitReader(p_in)){
        if(m_bitin.readInto(factor_bit_size, 8) != 8) {
            throw std::runtime_error("Error while reading from input stream");
        }
        m_bytes_read++;
    };

    int decode_impl(LZ77::factor_id &p_value) {
        bool is_offset = 0;
        if(m_bitin.readInto(is_offset, 1) != 1) {
            return 0;
        }
        if(!is_offset){
            char8_t c;
            if(m_bitin.readInto(c, 8) != 8) {
                return 0;
            }
            p_value.value = c;
            p_value.length = 0;
            m_bit_counter += 9;
        }
        else {
            size_t offset;
            if(m_bitin.readInto(offset,factor_bit_size) !=factor_bit_size ||
                m_bitin.readInto(p_value.length, factor_bit_size) != factor_bit_size) {
                return 0;
            }
            p_value.value = offset;
            m_bit_counter += 1 + factor_bit_size*2;
        }

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