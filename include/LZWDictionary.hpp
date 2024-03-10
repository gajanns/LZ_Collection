#ifndef LZWDICTIONARY_HPP
#define LZWDICTIONARY_HPP

#include "Dictionary.hpp"
#include "Factor.hpp"
#include <map>

class LZWDictionary : public Dictionary::Dictionary<LZW::factor> {
private:
    std::map<LZW::factor, std::string> m_id_to_val;
    std::map<std::string, LZW::factor> m_val_to_id;
    LZW::factor m_next_id = 0;

public:
    void addEntry(LZW::factor p_factor_id, std::string p_factor) override;
    void addEntry(std::string p_factor) override;

    std::pair<LZW::factor,std::string> getFactor(LZW::factor p_factor_id) override;
    std::pair<LZW::factor,std::string> getFactor(std::string p_factor) override;
    inline size_t size() { return m_id_to_val.size();};
};

#endif