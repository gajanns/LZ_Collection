#include <iostream>
#include <fstream>
#include <filesystem>
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
const std::string report_path = "report/";

std::string report_header = "input,in_size,algorithm,out_size,n_factors,comp_time,mem_usage";
std::string report_format = "%s,%zu,%s,%zu,%zu,%zu,%zu";

struct ExecutionSetup {
    std::vector<std::string> fin_names; 
    std::string fout_name;
    std::vector<Algorithm> algorithms;
    bool compress = true;
    bool benchmark = false;
    bool progressive = false;
    bool progressive_speedup = false;
    size_t num_steps = 1;
};

void print_usage(){
    std::cout << "Usage: LZ_Collection [input] [output] [algorithm] [direction] [benchmark] [progressive=Number of Steps] [speedup-benchmark]" << 
    std::endl << "\tinput: Filename of Input or \"all\" for all Files in data/(only per File Compression)" <<
    std::endl << "\toutput: Filename of Output" <<
    std::endl << "\talgorithm: LZW | LZ77 | Approx.LZ77 | Approx.LZ77Par | \"all\" for all Algorithms(only per File Compression)" <<
    std::endl << "\t(optional)direction: -d => (Decompress) , -c => (Compress, default)" << 
    std::endl << "\t(optional)benchmark: -b => Populate Performance-Data into report.csv" << 
    std::endl << "\t(optional)progressive: -bp=numsteps => Benchmark Progressive Compression with numsteps Steps(only Single File/Algorithm Compression)" << 
    std::endl << "\t(optional)speedup-benchmark: -bs => Benchmark Speedup of Approx.LZ77Par against Approx.LZ77" << std::endl;
}

void extract_userinput(ExecutionSetup &exec_setup, int argc, char *argv[]){
    if (argc < 4 || argc > 7) {
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

    for(auto i = 4; i < argc; i++) {

        std::string tmp{argv[i]};

        if(tmp == "-c") {
            exec_setup.compress = true;
        }
        else if(tmp == "-d") {
            exec_setup.compress = false;
            if(exec_setup.algorithms.size() > 1) {
                std::cerr << "Decompression only possible with one Algorithm" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
            if(exec_setup.fin_names.size() > 1) {
                std::cerr << "Decompression only possible with one Input-File" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
        }
        else if(tmp == "-b") {
            exec_setup.benchmark = true;
        }
        else if(tmp.starts_with("-bp=")) {
            exec_setup.benchmark = true;
            exec_setup.progressive = true;

            if(!exec_setup.compress) {
                std::cerr << "Progressive Compression only possible with Compression" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
            if(exec_setup.fin_names.size() > 1) {
                std::cerr << "Progressive Compression only possible with one Input-File" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
            if(exec_setup.algorithms.size() > 1) {
                std::cerr << "Progressive Compression only possible with one Algorithm" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }

            try {
                exec_setup.num_steps = std::stoi(tmp.substr(4));
            }
            catch(const std::invalid_argument& e) {
                std::cerr << "Invalid Argument for Progressive Compression" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
        }
        else if (tmp == "-bs") {
            exec_setup.benchmark = true;
            exec_setup.progressive_speedup = true;

            if(!exec_setup.compress) {
                std::cerr << "Progressive Compression only possible with Compression" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
            if(exec_setup.fin_names.size() > 1) {
                std::cerr << "Progressive Compression only possible with one Input-File" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
            if(exec_setup.algorithms.size() > 1 || exec_setup.algorithms[0] != appr77par) {
                std::cerr << "Progressive Compression only possible with Approx.LZ77Par" << std::endl;
                print_usage();
                exit(EXIT_FAILURE);
            }
        }
        else {
            std::cerr << "Unknown Option" << std::endl;
            print_usage();
            exit(EXIT_FAILURE);
        }
    }
}

void push_stats(Compression::CompressionStatistics &p_stats, std::fstream &p_report_stream, std::string p_in_file, std::string p_algo) {
    static bool is_header = true;
    if(is_header) {
        for(auto field_name: p_stats.m_specialized_stats.m_field_names) {
            report_header += "," + field_name;
        }
        p_report_stream << report_header;
        is_header = false;
    }

    char tmp[500];
    p_report_stream.write("\n", 1);
    auto size = sprintf(tmp, report_format.c_str(), p_in_file.c_str(), p_stats.m_input_size, p_algo.c_str(), p_stats.m_output_size,
                        p_stats.m_factor_count, p_stats.m_run_time_milliseconds, p_stats.m_mem_usage);
    p_report_stream.write(tmp, size);
    for(auto field_value: p_stats.m_specialized_stats.m_field_values) {
        std::string field_str = std::to_string(field_value);
        p_report_stream << "," << field_str;
    }
}

Compression::CompressionStatistics compress(Algorithm p_algo, InStreamView &p_in, std::fstream &p_out) {
    Compression::CompressionStatistics stats;
    switch(p_algo) {
        case lzw: {
            LZWEncoder encoder(p_out);
            LZWCompressor::Instance().compress(p_in, encoder);
            stats = LZWCompressor::Instance().m_stats;
            break;
        }
        case lz77: {
            LZ77Encoder encoder(p_out, p_in.size());
            LZ77Compressor::Instance().compress(p_in, encoder);
            stats = LZ77Compressor::Instance().m_stats;
            break;
        }
        case appr77seq: {
            ApproxLZ77Encoder encoder(p_out, p_in.size());
            ApproxLZ77Compressor::Instance().compress(p_in, encoder);
            stats = ApproxLZ77Compressor::Instance().m_stats;
            break;
        }
        case appr77par: {
            ApproxLZ77Encoder encoder(p_out, p_in.size());
            ApproxLZ77ParCompressor::Instance().compress(p_in, encoder);
            stats = ApproxLZ77ParCompressor::Instance().m_stats;
            break;
        }
        default:
            break;
    }
    return stats;
}

Compression::CompressionStatistics decompress(Algorithm p_algo, std::ifstream &p_in, OutStreamView &p_out) {
    Compression::CompressionStatistics stats;
    switch(p_algo) {
        case lzw: {
            LZWDecoder decoder(p_in);
            LZWCompressor::Instance().decompress(decoder, p_out);
            stats = LZWCompressor::Instance().m_stats;
            break;
        }
        case lz77: {
            LZ77Decoder decoder(p_in);
            LZ77Compressor::Instance().decompress(decoder, p_out);
            stats = LZ77Compressor::Instance().m_stats;
            break;
        }
        case appr77seq: {
            ApproxLZ77Decoder decoder(p_in);
            ApproxLZ77Compressor::Instance().decompress(decoder, p_out);
            stats = ApproxLZ77Compressor::Instance().m_stats;
            break;
        }
        case appr77par: {
            ApproxLZ77Decoder decoder(p_in);
            ApproxLZ77ParCompressor::Instance().decompress(decoder, p_out);
            stats = ApproxLZ77ParCompressor::Instance().m_stats;
            break;
        }
        default:
            break;
    }
    return stats;
}

void compress_per_file(ExecutionSetup &exec_setup, std::unique_ptr<std::fstream> &output_stream, std::unique_ptr<std::fstream> &report_stream) {
    if(report_stream) {
        (*report_stream) << report_header;
    }

    for(auto in_file_name: exec_setup.fin_names) {
        InStreamView in_stream(in_file_name);

        for(auto& algo: exec_setup.algorithms) {
            std::cout << "Compressing " << in_file_name << " with " << algo_names[algo] << " ... " << std::flush;

            output_stream->seekg(0, std::ios::beg);
            output_stream->seekp(0, std::ios::beg);
            Compression::CompressionStatistics stats = compress(algo, in_stream, *output_stream);
            output_stream->flush();

            if(report_stream != nullptr) {
                char tmp[500];
                report_stream->write("\n", 1);
                auto size = sprintf(tmp, report_format.c_str(), in_file_name.c_str(), stats.m_input_size, algo_names[algo].c_str(), stats.m_output_size,
                                    stats.m_factor_count, stats.m_run_time_milliseconds, stats.m_mem_usage);
                report_stream->write(tmp, size);
            }
            std::cout << "Finished" << std::endl;
        }
    }
}

void compress_progressive(ExecutionSetup &exec_setup, std::unique_ptr<std::ifstream> &input_stream, std::unique_ptr<std::fstream> &output_stream, std::unique_ptr<std::fstream> &report_stream) {
    auto algo = exec_setup.algorithms[0];
    input_stream -> seekg(0, std::ios::end);
    size_t in_size = input_stream->tellg();
    input_stream -> seekg(0, std::ios::beg);

    size_t data_per_step = in_size / exec_setup.num_steps;


    for(size_t i = 1; i <= exec_setup.num_steps; i++) {
        size_t cur_size = i == exec_setup.num_steps ? in_size : i * data_per_step;
        InStreamView in_stream(*input_stream, cur_size);

        std::cout << "Compressing " << cur_size << " Bytes of " << exec_setup.fin_names[0] << " with " << algo_names[algo] << " ... " << std::flush;

        output_stream->seekg(0, std::ios::beg);
        output_stream->seekp(0, std::ios::beg);
        Compression::CompressionStatistics stats = compress(algo, in_stream, *output_stream);
        output_stream->flush();

        if(report_stream != nullptr) {
            push_stats(stats, *report_stream, exec_setup.fin_names[0], algo_names[algo]);
        }
        std::cout << "Finished" << std::endl;
    }
}

void compress_speedup_benchmark(ExecutionSetup &exec_setup, std::unique_ptr<std::ifstream> &input_stream, std::unique_ptr<std::fstream> &output_stream, std::unique_ptr<std::fstream> &report_stream) {

    for(size_t num_threads = 1; num_threads <= omp_get_max_threads(); num_threads+=1) {
        InStreamView in_stream(*input_stream);
        output_stream->seekg(0, std::ios::beg);
        output_stream->seekp(0, std::ios::beg);
        Compression::CompressionStatistics stats;

        if(num_threads == 1) {
            std::cout << "Compressing " << exec_setup.fin_names[0] << " with " << algo_names[appr77seq] << " ... " << std::flush;
            stats = compress(appr77seq, in_stream, *output_stream);
        }
        else {
            std::cout << "Compressing " << exec_setup.fin_names[0] << " with " << algo_names[appr77par] << " and " << num_threads << " Threads ... " << std::flush;
            omp_set_num_threads(num_threads);
            ApproxLZ77Par::num_threads = num_threads;
            ApproxLZ77Par::num_thread_mask = std::bit_floor(num_threads) - 1;
            stats = compress(appr77par, in_stream, *output_stream);
        }
        output_stream->flush();
        stats.m_input_size = num_threads;
        if(report_stream != nullptr) {
            push_stats(stats, *report_stream, exec_setup.fin_names[0], algo_names[appr77par]);
        }
        std::cout << "Finished" << std::endl;
    }
}

void decompress_file(ExecutionSetup &exec_setup, std::unique_ptr<std::ifstream> &input_stream, std::unique_ptr<std::fstream> &output_stream, std::unique_ptr<std::fstream> &report_stream) {
    if(report_stream) {
        (*report_stream) << report_header;
    }

    auto algo = exec_setup.algorithms[0];
    std::cout << "Decompressing " << exec_setup.fin_names[0] << " with " << algo_names[algo] << " ... " << std::flush;

    OutStreamView out_stream(*output_stream);
    Compression::CompressionStatistics stats = decompress(algo, *input_stream, out_stream);

    if(report_stream != nullptr) {
        char tmp[500];
        report_stream->write("\n", 1);
        auto size = sprintf(tmp, report_format.c_str(), exec_setup.fin_names[0].c_str(), stats.m_input_size, algo_names[algo].c_str(), stats.m_output_size,
                            stats.m_factor_count, stats.m_run_time_milliseconds, stats.m_mem_usage);
        report_stream->write(tmp, size);
    }
    std::cout << "Finished" << std::endl;
}

int main(int argc, char** argv){

    omp_set_num_threads(ApproxLZ77Par::num_threads);

    ExecutionSetup setup;

    if(argc == 1) {
        setup.fin_names.push_back("data/dummy.6018B");
        setup.fout_name = "dummy_res";
        setup.algorithms.push_back(lzw);
        setup.benchmark = true;
    }
    else{
        extract_userinput(setup, argc, argv);
    }

    std::unique_ptr<std::ifstream> input_stream;
    if(!setup.compress || setup.progressive || setup.progressive_speedup) {
        input_stream = std::unique_ptr<std::ifstream>(new std::ifstream(setup.fin_names[0], std::ios::in | std::ios::binary));
        if(!input_stream->is_open()){
            std::cerr << "Could not open input-file" <<std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::unique_ptr<std::fstream> output_stream(new std::fstream(setup.fout_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc));
    if(!output_stream->is_open()){
        std::cerr << "Could not open output-file" <<std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<std::fstream> report_stream = nullptr;
    if(setup.benchmark) {
        std::string report_file = [&]() {
            if(setup.progressive) {
                return report_path + "report_progressive_"+algo_names[setup.algorithms.back()]+".csv";
            }
            else if(setup.progressive_speedup) {
                return report_path + "report_speedup.csv";
            }
            else {
                return report_path + "report_by_file.csv";
            }
        }();
        report_stream = std::unique_ptr<std::fstream>(new std::fstream(report_file,  std::ios::in | std::ios::out | std::ios::trunc));
    }

    if(setup.compress) {
        if(setup.progressive) {
            compress_progressive(setup, input_stream, output_stream, report_stream);
        }
        else if(setup.progressive_speedup) {
            compress_speedup_benchmark(setup, input_stream, output_stream, report_stream);
        }
        else {
            compress_per_file(setup, output_stream, report_stream);
        }
    }
    else {
        decompress_file(setup, input_stream, output_stream, report_stream);
    }
}