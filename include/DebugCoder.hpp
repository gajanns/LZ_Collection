#pragma once

#include "Coder.hpp"
#include "Definition.hpp"
#include <string>
#include <iostream>
#include <sstream>


namespace DebugCoder
{
    template<typename Factor>
    class DebugEncoder: public Coder::Encoder<Factor> {
    private:
        std::ostream *m_out;
        size_t m_bytes_written = 0;

    public:
        DebugEncoder(std::ostream &p_out): m_out(&p_out){};
        ~DebugEncoder(){};

        int encode_impl(Factor p_value) {
            std::string tmp;
            if(!(p_value >> tmp)) {
                return 0;
            }
            *m_out << tmp << "|";
            m_bytes_written += tmp.size();
            return 1;
        }

        size_t bytes_written() {
            return m_bytes_written;
        }

        void flush() {
            m_out->flush();
        }
    };

    template<typename Factor>
    class DebugDecoder : public Coder::Decoder<Factor> {
    private:
        std::istream *m_in;
        size_t m_bytes_read = 0;

    public:
        DebugDecoder(std::istream &p_in): m_in(&p_in){};
        ~DebugDecoder(){};

        int decode_impl(Factor &p_value) {
            std::string p_res;
            if(!std::getline(*m_in, p_res, '|')) {
                return 0;
            }
            if(!(p_value << p_res)) {
                return 0;
            }
            m_bytes_read += p_res.size();
            return 1;
        }

        size_t bytes_read() {
            return m_bytes_read;
        }

        bool reached_end() {
            return m_in->eof();
        }
    };
} // namespace DebugCoder
