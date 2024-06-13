/*##############################################################################*/
// Reference to Externals:                                                      //
// - Modulo-Operation taken from https://github.com/pdinklag/fp ( MIT License ) //
/*##############################################################################*/

#pragma once

#include <concepts>
#include <iterator>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include "StreamView.hpp"
#include "Definition.hpp"

static size_t cur_exp = 0;
static size_t cur_inv_base = 1;

/**
 * @brief Class covers generation and manipulation of Rabin-Karp-Hashfunction 
 * H(i,j)=Sum(k=i,j){S[k]*base^(j-k)}%prime
 * 
 */
class RabinKarpFingerprint {
private:
    size_t m_acc_base = 1;

public:
    /**
     * @brief Hashvalue
     * 
     */
    size_t val = 0;
    static const size_t base = 65517;
    static const size_t base_inverse = 1717389705276502644;
    static const size_t prime_exp = 61;
    static const size_t prime = 2305843009213693951; // 2^61-1

    RabinKarpFingerprint() = default;
    RabinKarpFingerprint(const NumRange auto& p_data): val(calc_hash_value(p_data, &m_acc_base)){}
    RabinKarpFingerprint(const size_t p_acc_base, const size_t p_hash_value):m_acc_base(p_acc_base),val(p_hash_value){}
    RabinKarpFingerprint(const RabinKarpFingerprint& p_value):m_acc_base(p_value.m_acc_base),val(p_value.val){}

    static size_t mod(__uint128_t const value, size_t prime) {
        __uint128_t const v = value + 1;
        size_t const z = ((v >> prime_exp) + v) >> prime_exp;
        return (value + z) & prime;
    }

    /**
     * @brief Calculate Hashvalue from data
     * 
     * @param p_data Data to be processed
     * @param p_acc_base Export auxiliary number
     * @return __uint128_t Hashvalue
     */
    static size_t calc_hash_value(NumRange auto& p_data, size_t *p_acc_base=nullptr){
        __uint128_t acc_base = 1;
        __uint128_t val = 0;

        for(auto p: p_data | std::views::reverse){
            val += p*acc_base;
            acc_base = mod(acc_base*base, prime);
            val = mod(val, prime);
        }

        if(p_acc_base != nullptr){
            (*p_acc_base) = static_cast<size_t>(acc_base);
        }
        return static_cast<size_t>(val);
    }

    /**
     * @brief Generate Hashvalue from concatenating two underlying data phrases
     * 
     * @param p_value Hashvalue(Dataphrase) to append
     * @return RabinKarpFingerprint Hashvalue of concatenation
     */
    friend RabinKarpFingerprint operator+(const RabinKarpFingerprint& p_left, const RabinKarpFingerprint& p_right){
        size_t res_acc_base = mod(static_cast<__uint128_t>(p_left.m_acc_base) * p_right.m_acc_base, prime);
        size_t res_hash = mod((static_cast<__uint128_t>(p_left.val) * p_right.m_acc_base)+ p_right.val, prime);
        return RabinKarpFingerprint(res_acc_base, res_hash);
    }

    /**
     * @brief Generate two Fingerprints by splitting underlying Dataphrase (a little mor efficient)
     * 
     * @param p_data Underlying Dataphrase (No Coherence-Check possible)
     * @return std::pair<RabinKarpFingerprint, RabinKarpFingerprint> Fingerprint after splitting
     */
    std::pair<RabinKarpFingerprint, RabinKarpFingerprint> split(const NumRange auto& p_data, size_t p_pos) const {
        if(p_pos == 0) return {RabinKarpFingerprint(1, 0), *this};
        if(p_pos == p_data.size()) return {*this, RabinKarpFingerprint(1, 0)};
        if(p_pos > p_data.size()) throw std::invalid_argument("Split position out of range");

        auto right_data = p_data | std::views::drop(p_pos);
        RabinKarpFingerprint right_fp(right_data);
        __uint128_t inv_acc_base = 1, inv_base = base_inverse, exp = right_data.size();
        while(exp > 0) {
            if(exp%2) {
                inv_acc_base = mod(inv_acc_base * inv_base, prime);
            }
            exp >>= 1;
            inv_base = mod(inv_base * inv_base, prime);
        }
        size_t left_hash = mod((static_cast<__uint128_t>(val) + prime - right_fp.val) * inv_acc_base, prime);
        size_t left_acc_base = mod(static_cast<__uint128_t>(m_acc_base) * inv_acc_base, prime);
        return {RabinKarpFingerprint(left_acc_base, left_hash), right_fp};
    }

    RabinKarpFingerprint split_off(RabinKarpFingerprint &p_right, size_t p_cut_length) const {
        if(p_cut_length == 0) return *this;

        __uint128_t inv_acc_base;
        if(cur_exp == p_cut_length){
            inv_acc_base = cur_inv_base;
        }
        else{
            cur_inv_base = calc_inv_acc_base(p_cut_length);
            cur_exp = p_cut_length;
            inv_acc_base = cur_inv_base;
        }
        size_t left_hash = mod((static_cast<__uint128_t>(val) + prime - p_right.val) * inv_acc_base, prime);
        size_t left_acc_base = mod(static_cast<__uint128_t>(m_acc_base) * inv_acc_base, prime);
        return RabinKarpFingerprint(left_acc_base, left_hash);
    }

    static size_t calc_inv_acc_base(size_t p_exp){
        __uint128_t inv_acc_base = 1, inv_base = base_inverse;
        while(p_exp > 0) {
            if(p_exp%2) {
                inv_acc_base = mod(inv_acc_base * inv_base, prime);
            }
            p_exp >>= 1;
            inv_base = mod(inv_base * inv_base, prime);
        }
        return mod(inv_acc_base, prime);
    }
    /**
     * @brief Slide Dataphrase to the right
     * 
     * @param p_left Character to drop from left side
     * @param p_right Character to append to right side
    */
    void shift_right(const std::integral auto p_left, const std::integral auto p_right){
        __uint128_t val_ = val;

        val_ = mod(val_ * base + p_right, prime);
        val = mod(val_ + prime - mod(static_cast<__uint128_t>(m_acc_base) * p_left, prime), prime);        
    }
    
    bool operator==(const RabinKarpFingerprint& p_value) const{
        return m_acc_base == p_value.m_acc_base && val == p_value.val;
    }
};