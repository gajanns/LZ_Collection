#include "BitWriter.hpp"

BitWriter::BitWriter(std::ostream &p_out){
    m_out = &p_out;
}

size_t BitWriter::writeFrom(char* p_src, size_t p_size, size_t p_bit_size){
    if(p_bit_size > 8*p_size) {
        std::cerr << "Not enough data to write from";
        exit(EXIT_FAILURE);
    }

    size_t tmp_size = p_bit_size;
    
    while(tmp_size > 0) {
        
        char bits_src = p_src[(tmp_size-1)/8];
        size_t bits_in_src = tmp_size%8?tmp_size%8:8;
        size_t bits_to_write = std::min(bits_in_src, 8-m_bits_in_buffer);
        m_current_byte <<= bits_to_write;
        m_current_byte |= (bits_src >> (bits_in_src-bits_to_write)) & ((1 << bits_to_write) - 1);
        tmp_size -= bits_to_write;
        m_bits_in_buffer += bits_to_write;

        if(m_bits_in_buffer == 8) {
            m_out->write((char*) &m_current_byte, 1);
            m_current_byte = 0;
            m_bits_in_buffer = 0;
        }
    }
    return p_bit_size-tmp_size;
}

void BitWriter::flush(){
    m_current_byte <<= 8 - m_bits_in_buffer;
    m_out->write((char*) &m_current_byte, 1);
    m_current_byte = 0;
    m_bits_in_buffer = 0;
}