#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>
#include "LZWCompressor.hpp"
#include "LZWBinCoder.hpp"
#include "LZ77BinCoder.hpp"
#include "LZ77Compressor.hpp"
#include "DebugCoder.hpp"
#include "Definition.hpp"
#include "StreamView.hpp"



std::filesystem::path data_path("data");

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
        for( auto algo : algorithm_to_name ){
            exec_setup.algorithms.push_back(algo.first);
        }
    }
    else if(algorithm_from_name.contains(algo_str)) {
        exec_setup.algorithms.push_back(algorithm_from_name.at(algo_str));
    }
    else {
        std::cerr << "Unknown Algorithm" << std::endl;
        print_usage();
        exit(-1);
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

void execute_algorithms(ExecutionSetup &exec_setup, std::vector<std::unique_ptr<std::fstream>> &input_streams, 
                        std::unique_ptr<std::ofstream> &output_stream, std::unique_ptr<std::ofstream> &report_stream) {
    if(report_stream) {
        (*report_stream)<< "Input,Input-Size[Bytes],Algorithm,Output-Size[Bytes],"
                        << "Compression-Ratio,nFactors,Compression-Time[ms],Memory-Usage[Bytes]\n";
    }

    for(size_t idx = 0; auto& in_stream: input_streams) {
        StreamView view(*in_stream);
        for(auto& algo: exec_setup.algorithms) {
            Compression::CompressionStatistics stats;
            switch(algo) {
                case lzw: {
                    LZWEncoder encoder(*output_stream);
                    LZWCompressor comp;
                    comp.compress(view, encoder);
                    stats = comp.m_stats;
                    break;
                }
                case lz77: {
                    LZ77Encoder encoder(*output_stream);
                    LZ77Compressor comp;
                    comp.compress(view, encoder);
                    stats = comp.m_stats;
                    break;
                }
                default:
                    continue;
                    break;
            }
            in_stream->clear();
            in_stream->seekg(0);
            output_stream->clear();
            output_stream->seekp(0);
            if(report_stream != nullptr) {
                char tmp[500];
                auto size = sprintf(tmp, "%s,%zu,%s,%zu,%.2f,%zu,%zu,%zu\n", exec_setup.fin_names[idx].c_str(), stats.m_input_size, algorithm_to_name.at(algo).c_str(), stats.m_output_size,
                (double)stats.m_input_size/stats.m_output_size, stats.m_factor_count, stats.m_run_time_milliseconds, stats.m_mem_usage);
                report_stream->write(tmp, size);
            }
        }
        idx++;
    }
    
}

int main(int argc, char** argv){
    
    ExecutionSetup setup;
    std::vector<std::unique_ptr<std::fstream>> input_streams;

    if(argc == 1) {
        setup.fin_names.push_back("data/dummy.txt");
        setup.fout_name = "dummy_res";
        setup.algorithms.push_back(lzw);
        setup.benchmark = true;
    }
    else{
        extract_userinput(setup, argc, argv);
    }

    for( auto in_name: setup.fin_names) {
        std::unique_ptr<std::fstream> in_stream(new std::fstream(in_name));
        if(!in_stream->is_open()){
            std::cerr << "Could not open input-file" <<std::endl;
            exit(EXIT_FAILURE);
        }
        input_streams.push_back(std::move(in_stream));
    }

    std::unique_ptr<std::ofstream> output_stream(new std::ofstream(setup.fout_name));
    if(!output_stream->is_open()){
        std::cerr << "Could not open output-file" <<std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<std::ofstream> report_stream = nullptr;
    if(setup.benchmark) {
        report_stream = std::unique_ptr<std::ofstream>(new std::ofstream("report/report.csv"));
    } 
    
    execute_algorithms(setup, input_streams, output_stream, report_stream);
}