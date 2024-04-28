#include "ApproxLZ77Compressor.hpp"

void ApproxLZ77Compressor::compress_impl(InStreamView &p_in, Coder::Encoder<LZ77::factor_id> &p_out) {
    std::span<const char8_t> input_span(p_in.slice_ref(0, p_in.size()));
    // ToDo: Implement Approx. LZ77 compression
}

void ApproxLZ77Compressor::decompress_impl(Coder::Decoder<LZ77::factor_id> &p_in, OutStreamView &p_out){

    LZ77::factor_id id;
    while(p_in.decode(id)) {

        if(id.length == 0 && std::get<char8_t>(id.value) != 0) {
            p_out.put(std::get<char8_t>(id.value));
        }
        else {
            auto data = p_out.slice_val(std::get<size_t>(id.value), id.length);

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