#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

/**
 * @brief Class allows random read-access to slices of istream in addition to simple read/write operations
 * 
 */
class StreamView {
private:
    std::istream *m_in;
    std::ostream *m_out;
    size_t m_bytes_written = 0, m_bytes_read = 0;
public:
    StreamView(std::istream &p_in, std::ostream &p_out): m_in(&p_in), m_out(&p_out){}
    StreamView(std::stringstream &p_sstream): m_in(&p_sstream), m_out(&p_sstream){}
    StreamView(std::fstream &p_fstream): m_in(&p_fstream), m_out(&p_fstream){}
    ~StreamView(){}

    /**
     * @brief Extract slice of data from specific area in stream.Doesn't affect other read-operations.
     * 
     * @param p_dest Buffer to copy data into
     * @param p_offset Startposition of slice in stream
     * @param p_size  Length of slice
     * @return int Actual Number of bytes read. -1 if failed to read.
     */
    int extractSlice(char* p_dest, auto p_offset, auto p_size) {
        std::streampos cur_read_pos = m_in->tellg();
        m_in->seekg(p_offset, m_in->beg);
        if(m_in->read(p_dest, p_size)) {
            m_in->seekg(cur_read_pos);
            return m_in->gcount();
        }
        return -1;
    }

    /**
     * @brief Extract slice of data from specific area in stream.Doesn't affect other read-operations.
     * 
     * @param p_dest String to copy data into
     * @param p_offset Startposition of slice in stream
     * @param p_size  Length of slice
     * @return int Actual Number of bytes read. -1 if failed to read.
     */
    int extractSlice(std::string &p_dest, auto p_offset, auto p_size) {
        if(p_dest.size() < p_size) {
            p_dest.resize(p_size);
        }
        return extractSlice(&p_dest[0], p_offset, p_size);
    }

    /**
     * @brief Read single byte/char from stream.
     * 
     * @param c Target space for read data.
     * @return int 1: Data read, 0: Data not read.
     */
    int get(char &c) {
        m_bytes_read++;
        return (m_in->get(c)?1:0);
    }

    /**
     * @brief Write string of data into ostream.
     * 
     * @param p_value Data to be written
     * @return int Number bytes actually written
     */
    int write(std::string p_value) {
        (*m_out) << p_value;
        m_bytes_written += p_value.size();
        return p_value.size();
    }

    /**
     * @brief Get number of bytes written into ostream through StreamView
     * 
     * @return size_t Number of Bytes
     */
    size_t bytes_written() {
        return m_bytes_written;
    }

    /**
     * @brief Get number of bytes read from istream through StreamView
     * 
     * @return size_t Number of Bytes
     */
    size_t bytes_read() {
        return m_bytes_read;
    }

};