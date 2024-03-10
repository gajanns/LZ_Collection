#ifndef DICTIONARY_HPP
#define DICTIONAY_HPP

#include <string>

namespace Dictionary
{
    template<typename ID> class Dictionary {
    
    public:
        virtual void addEntry(ID p_factor_id, std::string p_factor) = 0;
        virtual void addEntry(std::string p_factor) = 0;

        virtual std::pair<ID,std::string> getFactor(ID p_factor_id) = 0;
        inline std::pair<ID,std::string> operator [](ID p_factor_id) {return getFactor(p_factor_id);}
        virtual std::pair<ID,std::string> getFactor(std::string p_factor) = 0;
        inline std::pair<ID,std::string> operator [](std::string p_factor) {return getFactor(p_factor);}
        inline bool contains(ID p_factor_id) {return getFactor(p_factor_id).first != -1;};
        inline bool contains(std::string p_factor) {return getFactor(p_factor).first != -1;};
        virtual size_t size() = 0;
    };
}

#endif