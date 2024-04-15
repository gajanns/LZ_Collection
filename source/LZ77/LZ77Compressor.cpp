#include "LZ77Compressor.hpp"
#include "SuffixArray.hpp"
#include <bit>

void LZ77Compressor::compress_impl(StreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) {
    std::string input_str = p_in.readAll();
    auto sa = SuffixArray::generate_suffix_array(input_str, 255);
    std::vector<int32_t> psv(sa.size()-1, -1), nsv(sa.size()-1, -1);
    sa[0] = -1;
    sa.push_back(-1);
    
    for(size_t i = 1; i < sa.size()-1; i++) {
        auto j = sa[i-1];
        auto k = sa[i];

        while( k < j ) {
            nsv[j] = k;
            j = psv[j];
        }
        psv[k] = j;
    }

    size_t k = 0;
    while(k < input_str.size()) {

        size_t lcp_psv=0, lcp_nsv=0;
        if(psv[k] != -1) {
            while(k+lcp_psv < input_str.length() && input_str[k+lcp_psv] == input_str[psv[k]+lcp_psv]) lcp_psv++;
        }
        if(nsv[k] != -1) {
            while(k+lcp_nsv < input_str.length() && input_str[k+lcp_nsv] == input_str[nsv[k]+lcp_nsv]) lcp_nsv++;
        }

        if(lcp_psv < lcp_nsv) {
            p_out.encode(LZ77::factor_id{.value = static_cast<size_t>(nsv[k]), .length = lcp_nsv});
            k = k+lcp_nsv;
        }
        else {
            if(lcp_psv == 0) {
                p_out.encode(LZ77::factor_id{.value = input_str[k], .length = lcp_psv});
                k++;
            }
            else {
                p_out.encode(LZ77::factor_id{.value = static_cast<size_t>(psv[k]), .length = lcp_psv});
                k = k+lcp_psv;
            }
        }
    }
}

void LZ77Compressor::decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, StreamView &p_out){

    LZ77::factor_id id;
    while(p_in.decode(id)) {

        if(id.length == 0) {
            if(std::get<char>(id.value) != 0){
                p_out.write(std::string{std::get<char>(id.value)});
            }
            continue;
        }
        else {
            std::string tmp;
            auto size = p_out.extractSlice(tmp, std::get<size_t>(id.value), id.length);
            
            for(int i = id.length; i>0;){
                if(i >= size){
                    p_out.write(tmp);
                    i -= size;
                }
                else{
                    p_out.write(tmp.substr(0,i));
                    break;
                }
            }
        }
    }
}