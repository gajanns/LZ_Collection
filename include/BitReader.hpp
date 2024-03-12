#pragma once

#include <iostream>



class BitReader {
private:
    std::istream* m_in;
    unsigned char m_current_byte;
    size_t m_bits_in_buffer = 0;

public:
    BitReader(std::istream &p_in);

    /**
     * @brief Read data into a variable any type
     * 
     * @param p_dest Destination variable
     * @param p_bit_size Number of bits to read
     * @return size_t Number of bits successfully read
     */
    size_t readInto(auto& p_dest, size_t p_bit_size) {
        p_dest = 0;
        return readInto(reinterpret_cast<char*>(&p_dest), sizeof(p_dest), p_bit_size);
    }

    /**
     * @brief Read data into a char array
     * 
     * @param p_dest Destination array
     * @param p_size Size of p_dest
     * @param p_bit_size Number of bits to read
     * @return size_t Number of bits successfully read
     */
    size_t readInto(char* p_dest, size_t p_size, size_t p_bit_size);

    /**
     * @brief Read data into a variable any type
     * 
     * @param p_dest Destination variable
     * @return size_t Number of bits successfully read
     */
    size_t readInto(auto& p_dest){return readInto(p_dest, sizeof(p_dest)*8);}

    /**
     * @brief Check if the end of the stream has been reached
     * 
     * @return true End of stream reached
     * @return false End of stream not reached
     */
    bool reached_end();
};
