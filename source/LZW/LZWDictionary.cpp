#include "LZWDictionary.hpp"

void LZWDictionary::addEntry(LZW::factor_id p_factor_id, std::string p_factor) {
    m_id_to_val[p_factor_id] = p_factor;
    m_val_to_id[p_factor] = p_factor_id;
    m_next_id++;
}

void LZWDictionary::addEntry(std::string p_factor) {
    addEntry(m_next_id, p_factor);
}

std::pair<LZW::factor_id,std::string> LZWDictionary::getFactor(LZW::factor_id p_factor_id) {
    if (m_id_to_val.find(p_factor_id) != m_id_to_val.end()) {
        return {p_factor_id, m_id_to_val[p_factor_id]};
    }
    return {-1, ""};
}

std::pair<LZW::factor_id,std::string> LZWDictionary::getFactor(std::string p_factor) {
    if (m_val_to_id.find(p_factor) != m_val_to_id.end()) {
        return {m_val_to_id[p_factor], p_factor};
    }
    return {-1, ""};
}