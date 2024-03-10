#ifndef BIT_READER_HPP
#define BIT_READER_HPP

#include <iostream>



class BitReader {
private:
    std::istream* m_in;
    unsigned char m_current_byte;
    size_t m_bits_in_buffer = 0;

public:
    BitReader(std::istream &p_in);

    /**
     * @brief Read data into a variable of type T
     * 
     * @tparam T Type of data to read
     * @param p_dest Destination variable
     * @param p_bit_size Number of bits to read
     * @return size_t Number of bits successfully read
     */
    template<typename T>
    size_t readInto(T& p_dest, size_t p_bit_size) {
        p_dest = 0;
        return readInto(reinterpret_cast<char*>(&p_dest), sizeof(T), p_bit_size);
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
     * @brief Read data into a variable of type T
     * 
     * @tparam T Type of data to read
     * @param p_dest Destination variable
     * @return size_t Number of bits successfully read
     */
    template<typename T>
    inline size_t readInto(T& p_dest){return readInto(p_dest, sizeof(T)*8);}

    /**
     * @brief Check if the end of the stream has been reached
     * 
     * @return true End of stream reached
     * @return false End of stream not reached
     */
    bool reached_end();
};

#endif
