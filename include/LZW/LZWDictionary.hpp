#pragma once

#include "Dictionary.hpp"
#include "Definition.hpp"
#include <unordered_map>

/**
 * @brief Class represents naive dictionary for LZW.(ToDo:SuffixTrie)
 * 
 */
class LZWDictionary : public Dictionary::Dictionary<LZW::factor_id> {
private:
    std::unordered_map<LZW::factor_id, std::string> m_id_to_val;
    std::unordered_map<std::string, LZW::factor_id> m_val_to_id;
    LZW::factor_id m_next_id = 0;

public:
    void addEntry(LZW::factor_id p_factor_id, std::string p_factor) override;
    void addEntry(std::string p_factor) override;

    std::pair<LZW::factor_id,std::string> getFactor(LZW::factor_id p_factor_id) override;
    std::pair<LZW::factor_id,std::string> getFactor(std::string p_factor) override;
    inline size_t size() { return m_id_to_val.size();};
};