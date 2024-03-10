#ifndef CODER_HPP
#define CODER_HPP


namespace Coder {
    template<typename T> class Encoder {
    public:
        virtual void encode(T p_value) = 0;
        virtual void flush() = 0;
        virtual size_t bytes_written() = 0;
    };

    template<typename T> class Decoder {
    public:
        virtual T decode() = 0;
        virtual size_t bytes_read() = 0;
        virtual bool reached_end() = 0;
    };
}

#endif