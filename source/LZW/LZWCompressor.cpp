#include "LZWCompressor.hpp"
#include "LZWDictionary.hpp"
#include <string>


void LZWCompressor::compress_impl(Coder::Decoder<std::string> &p_in, Coder::Encoder<LZW::factor> &p_out) {
    std::string w;
    LZWDictionary dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary.addEntry(std::string(1, i));
    }
    int dictSize = 256;

    while(!p_in.reached_end()) {
        std::string c = p_in.decode();
        std::string wc = w + c;
        if (dictionary.contains(wc)) {
            w = wc;
        } else {
            p_out.encode(dictionary[w].first);
            dictionary.addEntry(wc);
            w = c;
        }
    }
    if (!w.empty()) {
        p_out.encode(dictionary[w].first);
    }
}

void LZWCompressor::decompress_impl(Coder::Decoder<LZW::factor> &p_in, Coder::Encoder<std::string> &p_out) {
    LZWDictionary dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary.addEntry(std::string(1, i));
    }
    int dictSize = 256;

    LZW::factor prev_code = p_in.decode();
    std::string prev_string = dictionary[prev_code].second;
    std::string curr_string;
    p_out.encode(prev_string);
    while(!p_in.reached_end()) {
        LZW::factor curr_code = p_in.decode();
        if(curr_code == 0) break;
        if (dictionary.contains(curr_code)) {
            curr_string = dictionary[curr_code].second;
        } else {
            curr_string = prev_string + prev_string[0];
        }
        p_out.encode(curr_string);
        dictionary.addEntry(prev_string + curr_string[0]);
        prev_string = curr_string;
    }
}
