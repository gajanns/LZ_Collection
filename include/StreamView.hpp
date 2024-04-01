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
    std::streamsize m_in_size;
    size_t m_bytes_written = 0, m_bytes_read = 0;
public:
    StreamView(std::istream &p_in, std::ostream &p_out): m_in(&p_in), m_out(&p_out){
        m_in->seekg(0,std::ios::beg);
        auto beg = m_in->tellg();
        m_in->seekg(0,std::ios::end);
        auto end = m_in->tellg();
        m_in_size = end-beg;
        m_in->seekg(0,std::ios::beg);
    }
    StreamView(std::iostream &p_iostream): StreamView(p_iostream, p_iostream){}
    ~StreamView(){}

    /**
     * @brief Extract slice of data from specific area in stream.Doesn't affect other read-operations.
     * 
     * @param p_dest Buffer to copy data into
     * @param p_offset Startposition of slice in stream
     * @param p_size  Length of slice
     * @return int Actual Number of bytes read.
     */
    size_t extractSlice(char* p_dest, auto p_offset, auto p_size) {
        std::streampos cur_read_pos = m_in->tellg();
        m_in->seekg(p_offset, m_in->beg);
        m_in->read(p_dest, p_size);
        m_in->clear();
        m_in->seekg(cur_read_pos);
        return m_in->gcount();
    }

    /**
     * @brief Extract slice of data from specific area in stream.Doesn't affect other read-operations.
     * 
     * @param p_dest String to copy data into
     * @param p_offset Startposition of slice in stream
     * @param p_size  Length of slice
     * @return int Actual Number of bytes read.
     */
    int extractSlice(std::string &p_dest, auto p_offset, auto p_size) {
        if(p_dest.size() != p_size) {
            p_dest.resize(p_size);
        }
        auto size = extractSlice(&p_dest[0], p_offset, p_size);
        if(p_dest.size() != size) {
            p_dest.resize(size);
        }
        return size;
    }

    /**
     * @brief Read single byte/char from stream.
     * 
     * @param c Target space for read data.
     * @return int 1: Data read, 0: Data not read.
     */
    int get(char &c) {
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
        return m_in_size;
    }

};

/**
 * @brief Class represents specific slice of Inputstream. Data will be dynamically loaded upon call.
 * 
 */
class StreamSliceReference{
private:
    StreamView *m_view;

public:
    const size_t m_pos, m_length;

    StreamSliceReference(StreamView& p_view, size_t p_pos, size_t p_length):m_view(&p_view), m_pos(p_pos), m_length(p_length){}

    /**
     * @brief Load reference String form Inputstream
     * 
     * @param res Slice of Stream
     */
    void getData(std::string& res) const{
        m_view->extractSlice(res, m_pos, m_length);
    }

    bool operator==(StreamSliceReference& p_ref){
        std::string s1, s2; 
        getData(s1);
        p_ref.getData(s2);
        return s1 == s2;
    }
};