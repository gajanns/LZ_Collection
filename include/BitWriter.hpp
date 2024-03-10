#ifndef BIT_WRITER_HPP
#define BIT_WRITER_HPP

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
     * @tparam T Type of data to write
     * @param p_src Data to write
     * @param p_bit_size First p_bit_size bits of p_src to write
     * @return size_t Number of bits successfully written
     */
    template<typename T>
    size_t writeFrom(T p_src, size_t p_bit_size) {
        return writeFrom(reinterpret_cast<char*>(&p_src), sizeof(T), p_bit_size);
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
     * @brief Write data of type T
     * 
     * @tparam T Type of data to write
     * @param p_src Data to write(cover Type T)
     * @return size_t Number of bits successfully written
     */
    template<typename T>
    inline size_t writeFrom(T& p_src){return writeFrom(p_src, sizeof(T)*8);}

    /**
     * @brief Flush the buffer(not to be done repeatedly)
     * 
     */
    void flush();
};

#endif
