#pragma once

#include <iostream>

/**
 * @brief Write data to a stream bit by bit
 * 
 */
class BitWriter {
private:
    std::ostream* m_out;
    unsigned char m_current_byte;
    size_t m_bits_in_buffer = 0;

public:
    BitWriter(std::ostream &p_out);

    /**
     * @brief Write data of type 
     * 
     * @param p_src Data to write
     * @param p_bit_size First p_bit_size bits of p_src to write
     * @return size_t Number of bits successfully written
     */
    size_t writeFrom(auto p_src, size_t p_bit_size) {
        return writeFrom(reinterpret_cast<char*>(&p_src), sizeof(p_src), p_bit_size);
    }

    /**
     * @brief Write data from a char array
     * 
     * @param p_src Data to write
     * @param p_size Size of p_src
     * @param p_bit_size First p_bit_size bits of p_src to write
     * @return size_t Number of bits successfully written
     */
    size_t writeFrom(char* p_src, size_t p_size, size_t p_bit_size);

    /**
     * @brief Write data of any type
     * 
     * @param p_src Data to write(cover Type)
     * @return size_t Number of bits successfully written
     */
    size_t writeFrom(auto p_src){return writeFrom(p_src, sizeof(p_src)*8);}

    /**
     * @brief Flush the buffer(not to be done repeatedly)
     * 
     */
    void flush();
};