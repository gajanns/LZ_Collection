#pragma once

#include <iostream>
#include <span>
#include<vector>
#include <ranges>


/**
 * @brief Class enables read-access to istream w or w/o pre-buffering and random access to slices of data
 * 
 */
class InStreamView {

private:
    std::istream *m_in;
    std::vector<char8_t> m_buffer;
    size_t m_size = 0, m_offset = 0;
    bool m_buffered;

public:
    InStreamView(std::istream &p_in, bool buffered = true): m_in(&p_in), m_buffered(buffered) {
        m_in->seekg(0, std::ios::end);
        m_size = m_in->tellg();
        m_in->seekg(0, std::ios::beg);

        if(buffered) {
            m_buffer.resize(m_size);
            if(!m_in->read(reinterpret_cast<char*>(m_buffer.data()), m_size)) {
                std::cerr << "Cannot read from Input-Stream" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    ~InStreamView() {
        m_in->clear();
        m_in->seekg(0, std::ios::beg);
    }

    /**
     * @brief Create a light-weight view to a slice of the input stream
     * 
     * @param p_offset Startposition of slice
     * @param p_size Length of slice
     * @return std::span<const char8_t> Reference to slice
     */
    std::span<const char8_t> slice_ref(size_t p_offset, size_t p_size) {
        if(p_size == 0) {
            return std::span<const char8_t>();
        }
        if(!m_buffered) {
            std::cerr << "Cannot create slice reference without buffer" << std::endl;
            exit(EXIT_FAILURE);
        }
        if(p_offset >= m_size) {
            std::cerr << "Input-Stream: Out of Bounds" << std::endl;
            exit(EXIT_FAILURE);
        }
        return std::span<const char8_t>(m_buffer.data()+p_offset, std::min(p_size, m_size-p_offset));
    }

    /**
     * @brief Create a copy of a slice of the input stream
     * 
     * @param p_offset Startposition of slice
     * @param p_size Length of slice
     * @return std::vector<char8_t> Copy of slice
     */
    std::vector<char8_t> slice_val(size_t p_offset, size_t p_size) {
        if(p_size == 0) {
            return std::vector<char8_t>();
        }
        if(p_offset >= m_size) {
            std::cerr << "Input-Stream: Out of Bounds" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::vector<char8_t> result(p_size);
        if(!m_buffered) {
            m_in->seekg(p_offset, std::ios::beg);
            if(!m_in->read(reinterpret_cast<char*>(result.data()), std::min(p_size, m_size-p_offset))) {
                std::cerr << "Cannot read from Input-Stream" << std::endl;
                exit(EXIT_FAILURE);
            }
            m_in->seekg(m_offset, std::ios::beg);
        }
        else {
            std::copy(m_buffer.begin()+p_offset, m_buffer.begin()+p_offset+std::min(p_size, m_size-p_offset), result.begin());
        }
        return result;
    }

    /**
     * @brief Get size of input stream
     * 
     * @return size_t Size of stream
     */
    size_t size() {
        return m_size;
    }

    /**
     * @brief Read single Byte from stream
     * 
     * @param c Target space for read data
     * @return true Data read
     * @return false Data not read
     */
    bool get(char8_t &c) {
        if(m_offset >= m_size) {
            return false;
        }

        if(!m_buffer.empty()) {
            c = m_buffer[m_offset];
            m_offset++;
            return true;
        }
        else if(m_in->read(reinterpret_cast<char*>(&c), 1)) {
            m_offset++;
            return true;
        }
        return false;
    }

};

/**
 * @brief Class enables single-pass write-access to ostream and random read-access to slices of data
 * 
 */
class OutStreamView {

private:
    std::iostream *m_io;
    size_t m_size = 0, m_offset = 0;

public:
    OutStreamView(std::iostream &p_io): m_io(&p_io) {}
    ~OutStreamView() {
        m_io->flush();
        m_io->seekp(0, std::ios::beg);
        m_io->seekg(0, std::ios::beg);
        m_io->clear();
    }

    /**
     * @brief Write sequence of data to output stream
     * 
     * @tparam T Range-Type of data
     * @param p_data Data to be written
     */
    template<typename T>
    requires std::ranges::range<T> && std::is_same_v<std::ranges::range_value_t<T>, char8_t>
    void write(T p_data) {
        if(!m_io->write(reinterpret_cast<const char*>(p_data.data()), p_data.size())) {
            std::cerr << "Cannot write to Output-Stream" << std::endl;
            exit(EXIT_FAILURE);
        }
        m_offset += p_data.size();
        if(m_offset > m_size) {
            m_size = m_offset;
        }
    }

    /**
     * @brief Write single Byte to output stream
     * 
     * @param p_data Data to be written
     */
    void put(char8_t p_data) {
        if(!m_io->write(reinterpret_cast<const char*>(&p_data), 1)) {
            std::cerr << "Cannot write to Output-Stream" << std::endl;
            exit(EXIT_FAILURE);
        }
        m_offset++;
        if(m_offset > m_size) {
            m_size = m_offset;
        }
    }

    /**
     * @brief Get size of output stream
     * 
     * @return size_t Size of stream
     */
    size_t size() {
        return m_size;
    }

    /**
     * @brief Create a copy of a slice of the output stream
     * 
     * @param p_offset Startposition of slice
     * @param p_size Length of slice
     * @return std::vector<char8_t> Copy of slice
     */
    std::vector<char8_t> slice_val(size_t p_offset, size_t p_size) {
        if(p_size == 0) {
            return std::vector<char8_t>();
        }
        if(p_offset >= m_size) {
            std::cerr << "Output-Stream: Out of Bounds" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::vector<char8_t> result(p_size);
        m_io->seekg(p_offset, std::ios::beg);
        if(!m_io->read(reinterpret_cast<char*>(result.data()), std::min(p_size, m_size-p_offset))) {
            std::cerr << "Cannot read from Output-Stream" << std::endl;
            exit(EXIT_FAILURE);
        }
        m_io->seekg(m_offset, std::ios::beg);
        return result;
    }
};