#ifndef TEXT_CODER_HPP
#define TEXT_CODER_HPP

#include "Coder.hpp"
#include "Factor.hpp"
#include <string>
#include <iostream>
#include <sstream>


namespace LZW
{
    class TextEncoder: public Coder::Encoder<LZW::factor> {
    private:
        std::ostream *m_out;
        size_t m_bytes_written = 0;
    public:
        TextEncoder(std::ostream &p_out): m_out(&p_out){};
        ~TextEncoder(){};

        inline void encode(LZW::factor p_value) {
            std::stringstream ss;
            ss << p_value;
            *m_out << ss.str() << "|";
            m_bytes_written += ss.str().size();
        }

        size_t bytes_written() {
            return m_bytes_written;
        }

        void flush() {
            m_out->flush();
        }
    };

    class TextDecoder : public Coder::Decoder<LZW::factor> {
    private:
        std::istream *m_in;
        size_t m_bytes_read = 0;

    public:
        TextDecoder(std::istream &p_in): m_in(&p_in){};
        ~TextDecoder(){};

        LZW::factor decode() {
            std::string p_value;
            char tmp;
            while(m_in -> get(tmp) && std::string{tmp} != "|") {
                p_value += tmp;
                m_bytes_read++;
            }
            m_bytes_read++;
            return std::stoi(p_value);
        }

        size_t bytes_read() {
            return m_bytes_read;
        }

        bool reached_end() {
            return m_in->eof();
        }
    };
}

namespace TextCoder
{
    class TextEncoder: public Coder::Encoder<std::string> {
    private:
        std::ostream *m_out;
        size_t m_bytes_written = 0;
    public:
        TextEncoder(std::ostream &p_out): m_out(&p_out){};
        ~TextEncoder(){};

        inline void encode(std::string p_value) {
            *m_out << p_value;
            m_bytes_written += p_value.size();
        }

        size_t bytes_written() {
            return m_bytes_written;
        }

        void flush() {
            m_out->flush();
        }
    };

    class TextDecoder : public Coder::Decoder<std::string> {
    private:
        std::istream *m_in;
        size_t m_bytes_read = 0;

    public:
        TextDecoder(std::istream &p_in): m_in(&p_in){};
        ~TextDecoder(){};

        std::string decode() {
            std::string p_value = "";
            char tmp;
            if(m_in -> get(tmp)) {
                p_value += tmp;
                m_bytes_read++;
            }
            return p_value;
        }

        size_t bytes_read() {
            return m_bytes_read;
        }

        bool reached_end() {
            return m_in->eof();
        }
    };
}


#endif
