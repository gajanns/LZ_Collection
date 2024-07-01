#pragma once

namespace Coder {
    /**
     * @brief Template for any Encoder
     * 
     * @tparam Factor Format of compressed factor as defined in factor.hpp
     */
    template<typename Factor, bool buffered = true> class Encoder {

    private:
        size_t m_factor_count = 0;
        std::vector<Factor> m_factors;

        virtual int encode_impl(Factor p_value) = 0;
        virtual void flush_impl() = 0;
    public:
        /**
         * @brief Encode factor into output
         * 
         * @param p_value Factor
         * @return int 1: Success, 0:Failed
         */
        int encode(Factor p_value){
            m_factor_count++;
            if(buffered) {
                m_factors.push_back(p_value);
                return 1;
            }
            return encode_impl(p_value);
        }

        /**
         * @brief Dump any residual data.
         * 
         */
        virtual void flush() {
            if(buffered) {
                for(auto &factor: m_factors) {
                    encode_impl(factor);
                }
                m_factors.clear();
            }
            flush_impl();
        }

        /**
         * @brief Return number of bytes written through Encoder
         * 
         * @return size_t Number of Bytes
         */
        virtual size_t bytes_written() = 0;

        /**
         * @brief Return number of Factors written through Encoder
         * 
         * @return size_t 
         */
        size_t factors_written() {
            return m_factor_count;
        }
    };

    /**
     * @brief Template for any Decoder
     * 
     * @tparam Factor Format of compressed factor as defined in factor.hpp
     */
    template<typename Factor> class Decoder {
    private:
        size_t m_factor_count = 0;

        virtual int decode_impl(Factor &p_value) = 0;
    public:
        /**
         * @brief Decode factor from input
         * 
         * @param p_value Factor
         * @return int 1: Success, 0:Failed
         */
        int decode(Factor &p_value) {
            m_factor_count++;
            return decode_impl(p_value);
        }
        
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

        /**
         * @brief Return number of Factors read through Encoder
         * 
         * @return size_t 
         */
        size_t factors_read() {
            return m_factor_count;
        }
    };
}