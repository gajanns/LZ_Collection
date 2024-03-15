#pragma once

namespace Coder {
    /**
     * @brief Template for any Encoder
     * 
     * @tparam Factor Format of compressed factor as defined in factor.hpp
     */
    template<typename Factor> class Encoder {
    public:
        /**
         * @brief Encode factor into output
         * 
         * @param p_value Factor
         * @return int 1: Success, 0:Failed
         */
        virtual int encode(Factor p_value) = 0;

        /**
         * @brief Dump any residual data.
         * 
         */
        virtual void flush() = 0;

        /**
         * @brief Return number of bytes written through Encoder
         * 
         * @return size_t Number of Bytes
         */
        virtual size_t bytes_written() = 0;
    };

    /**
     * @brief Template for any Decoder
     * 
     * @tparam Factor Format of compressed factor as defined in factor.hpp
     */
    template<typename Factor> class Decoder {
    public:
        /**
         * @brief Decode factor from input
         * 
         * @param p_value Factor
         * @return int 1: Success, 0:Failed
         */
        virtual int decode(Factor &p_value) = 0;
        
        /**
         * @brief Return number of bytes read through Decoder
         * 
         * @return size_t Number of Bytes
         */
        virtual size_t bytes_read() = 0;

        /**
         * @brief Return whether anymore data can be read from Input
         * 
         * @return true 
         * @return false 
         */
        virtual bool reached_end() = 0;
    };
}