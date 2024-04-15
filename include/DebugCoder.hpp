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

namespace LZ77
{
    class DebugEncoder: public Coder::Encoder<LZ77::factor_id> {
    private:
        std::ostream *m_out;
        size_t m_bytes_written = 0;
    public:
        DebugEncoder(std::ostream &p_out): m_out(&p_out){};
        ~DebugEncoder(){};

        int encode_impl(LZ77::factor_id p_value) {
            std::stringstream ss;

            ss << p_value.length;
            if(p_value.length == 0){
                ss << "," << std::get<char>(p_value.value);
            } else {
                ss << "," << std::get<size_t>(p_value.value);
            }
            
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

    class DebugDecoder : public Coder::Decoder<LZ77::factor_id> {
    private:
        std::istream *m_in;
        size_t m_bytes_read = 0;

    public:
        DebugDecoder(std::istream &p_in): m_in(&p_in){};
        ~DebugDecoder(){};

        int decode_impl(LZ77::factor_id &p_value) {
            std::string p_res;

            if(std::getline(*m_in, p_res, '|')){
                m_bytes_read += p_res.size();


                int tmp = std::sscanf(p_res.c_str(), "%zu,", &(p_value.length));
                if(tmp < 1){
                    return 0;
                }

                if(p_value.length == 0 ) {
                    char c;
                    tmp = std::sscanf(p_res.c_str(), "%zu,%c", &(p_value.length), &c);
                    p_value.value = c;
                }
                else {
                    size_t pos;
                    tmp = std::sscanf(p_res.c_str(), "%zu,%zu", &(p_value.length), &(pos));
                    p_value.value = pos;
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
