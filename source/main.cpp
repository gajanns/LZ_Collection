#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>
#include "Compressor.hpp"
#include "LZWCompressor.hpp"
#include "LZWBinCoder.hpp"
#include "LZ77BinCoder.hpp"
#include "ApproxLZ77BinCoder.hpp"
#include "LZ77Compressor.hpp"
#include "DebugCoder.hpp"
#include "Definition.hpp"
#include "StreamView.hpp"
#include "ApproxLZ77Compressor.hpp"
#include "ApproxLZ77ParCompressor.hpp"
#include <omp.h>


enum Algorithm {lzw, lz77, appr77seq, appr77par, all};
const std::vector<Algorithm> algo_ids = {lzw, lz77, appr77seq ,appr77par};
const std::vector<std::string> algo_names = {"LZW", "LZ77", "Approx.LZ77", "Approx.LZ77Par"};
 
std::filesystem::path data_path("data");
const std::string report_path = "report/report.csv";

const std::string report_header = "input,in_size,algorithm,out_size,n_factors,comp_time,mem_usage";
const std::string report_format = "%s,%zu,%s,%zu,%zu,%zu,%zu";

struct ExecutionSetup {
    std::vector<std::string> fin_names; 
    std::string fout_name;
    std::vector<Algorithm> algorithms;
    bool compress = true;
    bool benchmark = true;
};

void print_usage(){
    std::cout << "Usage: LZ_Collection [input] [output] [algorithm] [direction] [benchmark]" << 
    std::endl << "\tinput: Filename of Input or \"all\" for all files in data/" <<
    std::endl << "\toutput: Filename of Output" <<
    std::endl << "\talgorithm: LZW | LZ77 | Appr77Seq | Appr77Par \"all\" for all Algorithms" <<
    std::endl << "\t(optional)direction: -d => (Decompress) , -c => (Compress, default)" << 
    std::endl << "\t(optional)benchmark: -b => Populate Performance-Data into report.csv";
}

void extract_userinput(ExecutionSetup &exec_setup, int argc, char *argv[]){
    if (argc < 4 || argc > 6) {
        std::cerr << "Usage not correct" << std::endl;
        print_usage();
        exit(-1);
    }

    if(std::string{argv[1]} != "all") {
        exec_setup.fin_names.emplace_back(argv[1]);
    }
    else {
        for(auto file: std::filesystem::directory_iterator(data_path)) {
            exec_setup.fin_names.push_back(file.path());
        }
    }
    exec_setup.fout_name = std::string{argv[2]};

    std::string algo_str(argv[3]);
    if(algo_str == "all") {
        for(auto algo: algo_ids){
            exec_setup.algorithms.push_back(algo);
        }
    }
    else if(auto it = std::find(algo_names.begin(), algo_names.end(), algo_str);it != algo_names.end()){
        exec_setup.algorithms.push_back(algo_ids[it - algo_names.begin()]);
    }
    else {
        std::cerr << "Unknown Algorithm" << std::endl;
        print_usage();
        exit(EXIT_FAILURE);
    }

    for(auto i = 5; i < argc; i++) {

        std::string tmp{argv[i]};

        if(tmp == "-c") {
            exec_setup.compress = true;
        }
        else if(tmp == "-d") {
            exec_setup.compress = false;
        }
        else if(tmp == "-b") {
            exec_setup.benchmark = true;
        }
    }
}

void execute_algorithms(ExecutionSetup &exec_setup, std::unique_ptr<std::fstream> &output_stream, std::unique_ptr<std::fstream> &report_stream) {
    if(report_stream) {
        (*report_stream) << report_header;
    }

    for(size_t idx = 0; auto in_file_name: exec_setup.fin_names) {
        InStreamView in_stream(in_file_name.c_str(), 100000UL);

        for(auto& algo: exec_setup.algorithms) {
            std::cout << "Starting " << exec_setup.fin_names[idx] << " with " << algo_names[algo] << std::endl;
            Compression::CompressionStatistics stats;
            switch(algo) {
                case lzw: {
                    LZWEncoder encoder(*output_stream);
                    LZWCompressor::Instance().compress(in_stream, encoder);
                    stats = LZWCompressor::Instance().m_stats;
                    break;
                }
                case lz77: {
                    LZ77Encoder encoder(*output_stream, in_stream.size());
                    LZ77Compressor::Instance().compress(in_stream, encoder);
                    stats = LZ77Compressor::Instance().m_stats;
                    break;
                }
                case appr77seq: {
                    ApproxLZ77Encoder encoder(*output_stream, in_stream.size());
                    ApproxLZ77Compressor::Instance().compress(in_stream, encoder);
                    stats = ApproxLZ77Compressor::Instance().m_stats;
                    break;
                }
                case appr77par: {
                    ApproxLZ77Encoder encoder(*output_stream, in_stream.size());
                    ApproxLZ77ParCompressor::Instance().compress(in_stream, encoder);
                    stats = ApproxLZ77ParCompressor::Instance().m_stats;
                    break;
                }
                default:
                    break;
            }

            if(report_stream != nullptr) {
                char tmp[500];
                report_stream->write("\n", 1);
                auto size = sprintf(tmp, report_format.c_str(), exec_setup.fin_names[idx].c_str(), stats.m_input_size, algo_names[algo].c_str(), stats.m_output_size,
                                    stats.m_factor_count, stats.m_run_time_milliseconds, stats.m_mem_usage);
                report_stream->write(tmp, size);
            }
            std::cout << "Finished " << exec_setup.fin_names[idx] << " with " << algo_names[algo] << std::endl;
        }
        idx++;
    }
    
}

int main(int argc, char** argv){

    omp_set_num_threads(num_threads);

    ExecutionSetup setup;
    std::vector<InStreamView> input_streams;

    if(argc == 1) {
        setup.fin_names.push_back("data/dummy.6018B");
        setup.fout_name = "dummy_res";
        setup.algorithms.push_back(lzw);
        setup.benchmark = true;
    }
    else{
        extract_userinput(setup, argc, argv);
    }

    std::unique_ptr<std::fstream> output_stream(new std::fstream(setup.fout_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc));
    if(!output_stream->is_open()){
        std::cerr << "Could not open output-file" <<std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<std::fstream> report_stream = nullptr;
    if(setup.benchmark) {
        report_stream = std::unique_ptr<std::fstream>(new std::fstream("report/report.csv",  std::ios::in | std::ios::out | std::ios::trunc));
    } 
    
    execute_algorithms(setup, output_stream, report_stream);
}