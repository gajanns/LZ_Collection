#include "LZWCompressor.hpp"
#include "LZTrie.hpp"
#include <string>


void LZWCompressor::compress_impl(StreamView &p_in, Coder::Encoder<LZW::factor_id> &p_out) {

    LZTrie<char> trie;
    for (int i = 0; i < LZW::alphabet_size; i++) {
        trie.insert_back(std::string(1,i)[0]);
    }

    char c;
    int32_t last_code = -1;
    while(p_in.get(c)) {
        if(!trie.traverse(c)) {
            trie.insert_back(c);
            trie.reset_traversal();
            trie.traverse(c);
            p_out.encode(LZW::factor_id{.value = last_code});
        }
        last_code = trie.m_current->code;
    }

    if(last_code != -1) {
        p_out.encode(LZW::factor_id{.value = last_code});
    }
}

void LZWCompressor::decompress_impl(Coder::Decoder<LZW::factor_id> &p_in, StreamView &p_out) {

    std::vector<std::string> dictionary;
    dictionary.reserve(LZW::alphabet_size);

    for (int i = 0; i < LZW::alphabet_size; i++) {
        dictionary.push_back(std::string(1, i));
    }

    LZW::factor_id prev_code;
    if(!p_in.decode(prev_code)){
        return;
    }
    std::string prev_string = dictionary[prev_code.value];
    std::string curr_string; 
    LZW::factor_id curr_code;
    p_out.write(prev_string);
    while(!p_in.reached_end() && p_in.decode(curr_code) && curr_code != 0) {
    
        if (dictionary.size() > curr_code.value){
            curr_string = dictionary[curr_code.value];
        } 
        else {
            curr_string = prev_string + prev_string[0];
        }
        p_out.write(curr_string);
        dictionary.push_back(prev_string + curr_string[0]);
        prev_string = curr_string;
    }
}
