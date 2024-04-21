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
            std::u8string p_res;
            if(!(p_value >> p_res)) {
                return 0;
            }
            (*m_out).write(reinterpret_cast<const char*>(p_res.data()), p_res.size());
            *m_out << "|";
            m_bytes_written += p_res.size();
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
            std::u8string p_res;
            char8_t c;
            while(m_in->get(reinterpret_cast<char&>(c))) {
                m_bytes_read++;
                if(c == '|') {
                    break;
                }
                p_res += c;
            }
            if(!(p_value << p_res)) {
                return 0;
            }
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
