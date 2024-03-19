#include <iostream>
#include <fstream>
#include <sstream>
#include "LZWCompressor.hpp"
#include "LZWBinCoder.hpp"
#include "DebugCoder.hpp"
#include "StreamView.hpp"


enum Algorithm {lzw, lz77, appr77seq, appr77par};
size_t used_memory = 0;

void print_usage(){
    std::cout << "Usage: LZ_Collection [input] [output] [algorithm] [direction]" << 
    std::endl << "\tinput: Filename of Input" <<
    std::endl << "\toutput: Filename of Output" <<
    std::endl << "\talgorithm: LZW | LZ77 | Appr77Seq | Appr77Par" <<
    std::endl << "\t(optional)direction: -d => (Decompress) , -c => (Compress, default)" << std::endl;
}

void extract_userinput(std::string &input_str, std::string &output_str, Algorithm &algo, bool &decompress, int &argc, char *argv[]){
    if (argc < 4 || argc > 5) {
        std::cerr << "Usage not correct" << std::endl;
        print_usage();
        exit(-1);
    }
    input_str = std::string{argv[1]};
    output_str = std::string{argv[2]};
    std::string algo_str(argv[3]);
    
    if (algo_str == "LZW" || algo_str == "lzw") {
        algo = lzw;
    }
    else if (algo_str == "LZ77" || algo_str == "lz77") {
        algo = lz77;
    }
    else if (algo_str == "Appr77Seq" || algo_str == "appr77seq") {
        algo = appr77seq;
    }
    else if (algo_str == "Appr77Par" || algo_str == "appr77par") {
        algo = appr77par;
    }
    else {
        std::cerr << "Unknown Algorithm" << std::endl;
        print_usage();
        exit(-1);
    }

    if(argc == 5 && argv[4] == "-d") {
        decompress = true;
    }
    else if(argc == 5 && argv[4] == "-c") {
        decompress = false;
    }
}

void debug(){
    LZWCompressor compressor;
    std::stringstream ss_in, ss_out, ss_out2;
    ss_in << "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
"labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores "
             "et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem "
             "ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et "
             "dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. "
             "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut "
             "labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores "
             "et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem "
             "ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et "
             "dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. "
             "Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
    StreamView sv_in(ss_in);
    LZWEncoder encoder(ss_out);
    compressor.compress(sv_in, encoder);
        ss_out.clear();
    ss_out.seekg(0);

    LZWDecoder decoder(ss_out);
    StreamView sv_out(ss_out2);
    compressor.decompress(decoder, sv_out);
    }

int main(int argc, char** argv){

    if(argc == 1) {
        debug();
    }
    else {
        std::string input_str, output_str;
        Algorithm algorithm;
        bool decompress = false;
        extract_userinput(input_str, output_str, algorithm, decompress, argc, argv);
        // ToDo: Select suitable Inputdata(Files) so command-line call makes sense
    }
}