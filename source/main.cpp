#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "LZWCompressor.hpp"
#include "LZWBinCoder.hpp"
#include "LZ77BinCoder.hpp"
#include "LZ77Compressor.hpp"
#include "DebugCoder.hpp"
#include "StreamView.hpp"


enum Algorithm {lzw, lz77, appr77seq, appr77par, all};
std::string algo_names[] = {"LZW", "LZ77", "Appr.LZ77_Seq", "Appr.LZ77_Par"};

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

void execute_algorithms(std::vector<std::pair<std::string, std::iostream&>> &input_streams, std::ostream &output_stream, 
                                        const std::vector<Algorithm> &algorithms, std::ostream *report_stream = nullptr) {
    if(report_stream) {
        (*report_stream)    << "Input,Input-Size[Bytes],Algorithm,Output-Size[Bytes],"
                            << "Compression-Ratio,Compression-Time[ms],Memory-Usage[Bytes]\n";
    }

    for(auto in_stream: input_streams) {
        StreamView in_view(in_stream.second);
        for(auto algo:algorithms) {
            Compression::CompressionStatistics stats;
            
            switch(algo) {
                case Algorithm::lzw: {
                    LZWCompressor comp;
                    LZWEncoder encoder(output_stream);
                    comp.compress(in_view, encoder);
                    stats = comp.m_stats;
                    break;
                }
                case Algorithm::lz77: {
                    LZ77Compressor comp;
                    LZ77Encoder encoder(output_stream);
                    comp.compress(in_view, encoder);
                    stats = comp.m_stats;
                    break;
                }
                default:
                    std::cerr << "Not implemented" << std::endl;
                    exit(-1);
            }
            in_stream.second.seekg(std::ios::beg);
            output_stream.seekp(std::ios::beg);
            char buf[512];
            auto n = std::sprintf(buf, "%s,%d,%s,%d,%.2f,%d,%d\n",in_stream.first.c_str(), (int)stats.m_input_size, algo_names[algo].c_str(), 
                                (int)stats.m_output_size, (double)stats.m_input_size/stats.m_output_size, (int)stats.m_run_time_milliseconds, 
                                                                                                                    (int)stats.m_mem_usage);
            (*report_stream).write(buf, n);
        }
    }
}

int main(int argc, char** argv){
    std::string input_str, output_str;
    Algorithm algorithm;
    bool decompress = false;
    if(argc == 1) {
        input_str = "data/dummy.txt";
        output_str = "dummy_res";
        algorithm = lzw;
    }
    else {
        extract_userinput(input_str, output_str, algorithm, decompress, argc, argv);
    }

    std::fstream f_in(input_str);
    std::ofstream f_out(output_str);
    if(!f_in.is_open()){
        std::cerr << "Could not open Input-File" << std::endl;
        exit(-1);
    }

    if(!f_out.is_open()){
        std::cerr << "Could not open Output-File" << std::endl;
        exit(-1);
    }

    std::ofstream f_report("report.csv");
    if(!f_report.is_open()){
        std::cerr << "Could not open Report-File" << std::endl;
        exit(-1);
    }
    
    std::vector<std::pair<std::string, std::iostream&>> input{{input_str,f_in}};
    std::vector<Algorithm> algorithms{algorithm};
    execute_algorithms(input, f_out, algorithms, &f_report);
}