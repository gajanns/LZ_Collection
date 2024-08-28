#include "LZ77Compressor.hpp"
#include "SuffixArray.hpp"
#include <bit>

void LZ77Compressor::compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) {
    std::span<const char8_t> input_span(p_in.slice_ref(0, p_in.size()));
    //auto sa = SuffixArray::generate_suffix_array(input_span, 255);
    auto sa = SuffixArray::generate_suffix_array_libsais(input_span);
    std::vector<int32_t> psv(sa.size()-1, -1), nsv(sa.size()-1, -1);
    sa[0] = -1;
    
    for(size_t i = 1; i < sa.size(); i++) {
        auto j = sa[i-1];
        auto k = sa[i];

        while( k < j ) {
            nsv[j] = k;
            j = psv[j];
        }
        psv[k] = j;
    }

    size_t k = 0;
    while(k < input_span.size()) {

        u_int32_t lcp_psv=0, lcp_nsv=0;
        if(psv[k] != -1) {
            while(k+lcp_psv < input_span.size() && input_span[k+lcp_psv] == input_span[psv[k]+lcp_psv]) lcp_psv++;
        }
        if(nsv[k] != -1) {
            while(k+lcp_nsv < input_span.size() && input_span[k+lcp_nsv] == input_span[nsv[k]+lcp_nsv]) lcp_nsv++;
        }

        u_int32_t lcp = std::max(lcp_psv, lcp_nsv);
        if(lcp <= LZ77::min_ref_size) {
            p_out.encode(LZ77::factor_id{.value = input_span[k++], .length = 0});
        }
        else {
            u_int32_t pos = lcp_psv >= lcp_nsv ? psv[k] : nsv[k];
            p_out.encode(LZ77::factor_id{.value = pos, .length = lcp});
            k += lcp;
        }
    }
}

void LZ77Compressor::decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, OutStreamView &p_out){

    LZ77::factor_id id;
    while(p_in.decode(id)) {

        if(id.length == 0) {
            if(std::get<char8_t>(id.value) != 0){
                p_out.put(std::get<char8_t>(id.value));
            }
            continue;
        }
        else {

            auto data = p_out.slice_val(std::get<u_int32_t>(id.value), id.length);            
            for(int i = id.length; i>0;){
                if(i >= data.size()){
                    p_out.write(data);
                    i -= data.size();
                }
                else{
                    p_out.write(std::span(data.data(), i));
                    break;
                }
            }
        }
    }
}