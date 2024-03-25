#pragma once

#include <concepts>
#include <iterator>
#include <cstdlib>
#include <utility>
#include "StreamView.hpp"


template<typename T>
concept Hashable = requires { std::ranges::bidirectional_range<T> && std::is_integral_v<typename T::value_type>; };
    
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
    const size_t val;
    static const size_t base = 256;
    static const size_t base_inverse = 74;
    static const size_t prime = 997;


    RabinKarpFingerprint(Hashable auto& p_data): val(calc_hash_value(p_data, &m_acc_base)){}
    RabinKarpFingerprint(const size_t p_acc_base, const size_t p_hash_value):m_acc_base(p_acc_base),val(p_hash_value){}

    /**
     * @brief Calculate Hashvalue from data
     * 
     * @param p_data Data to be processed
     * @param p_acc_base Export auxiliary number
     * @return size_t Hashvalue
     */
    static size_t calc_hash_value(Hashable auto& p_data, size_t *p_acc_base=nullptr){
        size_t acc_base = 1;
        size_t val = 0;
        for(auto it = p_data.crbegin(); it!=p_data.crend();++it) {
            val += (*it)*acc_base;
            acc_base = (acc_base*base) % prime;
            val %= prime;
        }
        if(p_acc_base != nullptr){
            (*p_acc_base) = acc_base;
        }
        return val;
    }

    /**
     * @brief Generate Hashvalue from concatenating two underlying data phrases
     * 
     * @param p_value Hashvalue(Dataphrase) to append
     * @return RabinKarpFingerprint Hashvalue of concatenation
     */
    RabinKarpFingerprint operator+(RabinKarpFingerprint const& p_value){
        size_t res_acc_base = (m_acc_base * p_value.m_acc_base) % prime;
        size_t res_hash = ((val * p_value.m_acc_base)+ p_value.val)% prime;
        return RabinKarpFingerprint(res_acc_base, res_hash);
    }

    /**
     * @brief Generate Hashvalue as result of appending single Dataelement to underlying Dataphrase
     * 
     * @param p_left Single Dataelement to append
     * @param p_value Hashvalue of concatenation
     * @return RabinKarpFingerprint 
     */
    friend RabinKarpFingerprint operator<<(const std::integral auto& p_left, const RabinKarpFingerprint& p_value) {
        size_t acc_base = (p_value.m_acc_base*RabinKarpFingerprint::base_inverse)%RabinKarpFingerprint::prime;
        size_t hash = p_value.val + RabinKarpFingerprint::prime - (p_left * acc_base) % RabinKarpFingerprint::prime;

        return RabinKarpFingerprint(acc_base, hash % prime);
    }

    /**
     * @brief Generate Hashvalue as result of erase single Dataelement from left end of underlying Dataphrase
     * No coherence check of p_left possible.
     * 
     * @param p_left Single Dataelement to erase
     * @param p_value Hashvalue of concatenation
     * @return RabinKarpFingerprint 
     */
    friend RabinKarpFingerprint operator<<(const RabinKarpFingerprint& p_value, const std::integral auto& p_right) {
        size_t acc_base = (p_value.m_acc_base*RabinKarpFingerprint::base)%RabinKarpFingerprint::prime;
        size_t hash = (p_value.val * RabinKarpFingerprint::base + p_right) % RabinKarpFingerprint::prime;

        return RabinKarpFingerprint(acc_base, hash);
    }
    
    bool operator==(const RabinKarpFingerprint& p_value) const{
        return m_acc_base == p_value.m_acc_base && val == p_value.val;
    }
};

/**
 * @brief Hashfunction for StreamSliceReference in accordance to format of std::hash
 * 
 */
struct RabinKarpHasher : std::hash<StreamSliceReference> {
    std::size_t operator()(const StreamSliceReference& p_ref) const noexcept {
        std::string s;
        p_ref.getData(s);
        return RabinKarpFingerprint::calc_hash_value(s);
    }
};