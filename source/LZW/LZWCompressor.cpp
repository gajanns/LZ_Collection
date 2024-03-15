#include "LZWCompressor.hpp"
#include "LZWDictionary.hpp"
#include <string>


void LZWCompressor::compress_impl(StreamView &p_in, Coder::Encoder<LZW::factor_id> &p_out) {
    std::string w;
    LZWDictionary dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary.addEntry(std::string(1, i));
    }
    int dictSize = 256;

    char c;
    while(p_in.get(c)) {
        std::string wc = w + c;
        if (dictionary.contains(wc)) {
            w = wc;
        } else {
            p_out.encode(dictionary[w].first);
            dictionary.addEntry(wc);
            w = std::string{c};
        }
    }
    if (!w.empty()) {
        p_out.encode(dictionary[w].first);
    }
}

void LZWCompressor::decompress_impl(Coder::Decoder<LZW::factor_id> &p_in, StreamView &p_out) {
    LZWDictionary dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary.addEntry(std::string(1, i));
    }
    int dictSize = 256;

    LZW::factor_id prev_code;
    if(!p_in.decode(prev_code)){
        return;
    }
    std::string prev_string = dictionary[prev_code].second;
    std::string curr_string; 
    LZW::factor_id curr_code;
    p_out.write(prev_string);
    while(!p_in.reached_end() && p_in.decode(curr_code)) {
        if(curr_code == 0) break;
        if (dictionary.contains(curr_code)) {
            curr_string = dictionary[curr_code].second;
        } else {
            curr_string = prev_string + prev_string[0];
        }
        p_out.write(curr_string);
        dictionary.addEntry(prev_string + curr_string[0]);
        prev_string = curr_string;
    }
}
