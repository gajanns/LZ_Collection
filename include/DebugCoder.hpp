#pragma once

#include "Coder.hpp"
#include "Definition.hpp"
#include <string>
#include <iostream>
#include <sstream>


namespace LZW
{
    class DebugEncoder: public Coder::Encoder<LZW::factor_id> {
    private:
        std::ostream *m_out;
        size_t m_bytes_written = 0;
    public:
        DebugEncoder(std::ostream &p_out): m_out(&p_out){};
        ~DebugEncoder(){};

        int encode_impl(LZW::factor_id p_value) {
            std::stringstream ss;
            ss << p_value;
            *m_out << ss.str() << "|";
            m_bytes_written += ss.str().size();
            return 1;
        }

        size_t bytes_written() {
            return m_bytes_written;
        }

        void flush() {
            m_out->flush();
        }
    };

    class DebugDecoder : public Coder::Decoder<LZW::factor_id> {
    private:
        std::istream *m_in;
        size_t m_bytes_read = 0;

    public:
        DebugDecoder(std::istream &p_in): m_in(&p_in){};
        ~DebugDecoder(){};

        int decode_impl(LZW::factor_id &p_value) {
            std::string p_res;
            char tmp;
            while(m_in -> get(tmp) && tmp != '|') {
                p_res += tmp;
                m_bytes_read++;
            }
            m_bytes_read++;
            if(p_res.empty()) {
                return 0;
            }
            p_value = std::stoi(p_res);
            return 1;
        }

        size_t bytes_read() {
            return m_bytes_read;
        }

        bool reached_end() {
            return m_in->eof();
        }
    };
}

namespace LZ77_Wndw
{
    class DebugEncoder: public Coder::Encoder<LZ77_Wndw::factor_id> {
    private:
        std::ostream *m_out;
        size_t m_bytes_written = 0;
    public:
        DebugEncoder(std::ostream &p_out): m_out(&p_out){};
        ~DebugEncoder(){};

        int encode_impl(LZ77_Wndw::factor_id p_value) {
            std::stringstream ss;
            ss << p_value.offset << "," << p_value.length << "," << p_value.next_char;
            *m_out << ss.str() << "|";
            m_bytes_written += ss.str().size();
            return 1;
        }

        size_t bytes_written() {
            return m_bytes_written;
        }

        void flush() {
            m_out->flush();
        }
    };

    class DebugDecoder : public Coder::Decoder<LZ77_Wndw::factor_id> {
    private:
        std::istream *m_in;
        size_t m_bytes_read = 0;

    public:
        DebugDecoder(std::istream &p_in): m_in(&p_in){};
        ~DebugDecoder(){};

        int decode_impl(LZ77_Wndw::factor_id &p_value) {
            std::string p_res;
            char tmp;

            if(std::getline(*m_in, p_res, '|')){
                m_bytes_read += p_res.size();
                int tmp = std::sscanf(p_res.c_str(), "%d,%d,%c", &(p_value.offset), &(p_value.length), &(p_value.next_char));
                if(tmp < 3){
                    p_value.next_char = '\0';
                }
                if(tmp < 2){
                    return 0;
                }
                return 1;
            }
            return 0;
        }

        size_t bytes_read() {
            return m_bytes_read;
        }

        bool reached_end() {
            return m_in->eof();
        }
    };
}
