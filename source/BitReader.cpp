#include "BitReader.hpp"

BitReader::BitReader(std::istream &p_in) {
    m_in = &p_in;
}

size_t BitReader::readInto(char* p_dest, size_t p_size, size_t p_bit_size) {

    if(p_bit_size > 8*p_size) {
        std::cerr << "Cannot read requested amount of data";
        exit(EXIT_FAILURE);
    }

    size_t tmp_size = p_bit_size;

    while(tmp_size > 0) {
        if(m_bits_in_buffer == 0) {
            if(!m_in->eof()) {
                m_in->read((char*) &m_current_byte, 1);
            }
            else {
                break;
            }
            m_bits_in_buffer = 8;
        }

        size_t bits_to_read = std::min(tmp_size%8?tmp_size%8:8, m_bits_in_buffer);
        p_dest[(tmp_size-1)/8] <<= bits_to_read;
        p_dest[(tmp_size-1)/8] |= (m_current_byte >> size_t{8 - bits_to_read}) & ((1 << bits_to_read) - 1);
        tmp_size -= bits_to_read;
        m_bits_in_buffer -= bits_to_read;
        m_current_byte <<= bits_to_read;
    }
    return p_bit_size-tmp_size;
}

bool BitReader::reached_end() {
    return m_bits_in_buffer == 0 && m_in->eof();
}
