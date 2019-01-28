
#include <chrono>
#include <iostream>
#include <regex>
#include <thread>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"
#include "util.hpp"
#define private public
#include "parallel_recompression.hpp"

int str_to_int(std::string s) {
    std::istringstream ss(s);
    int n;
    if (!(ss >> n)) {
        std::cerr << "Invalid number: " << s;
        return -1;
    } else if (!ss.eof()) {
        std::cerr << "Trailing characters after number: " << s;
        return -1;
    }
    return n;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "./parallel_recomp_bench [file name] [cores]" << std::endl;
    }
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    std::string file_name(argv[1]);

    size_t pos = file_name.find_last_of('/');
    std::string dataset;
    if (pos != std::string::npos) {
        dataset = file_name.substr(pos + 1);
    } else {
        dataset = file_name;
    }

    std::regex reg("_");
    dataset = std::regex_replace(dataset, reg, "\\_");
    
    int cores = std::thread::hardware_concurrency();
    if (argc > 2) {
        cores = str_to_int(argv[2]);
    }
    std::cout << "Using " << cores << " threads" << std::endl;

    recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t> recompression{dataset};
    // recompression.cores = 2;
    typedef recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
    text_t text;
    recomp::util::read_file<text_t>(file_name, text);

    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;

    if (text.size() < 31) {
        for (size_t i = 0; i < text.size(); ++i) {
            std::cout << (char) text[i];
        }
        std::cout << std::endl;
    }
    
    const auto startTime = std::chrono::system_clock::now();

    recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    LOG(INFO) << "Time for parallel recompression: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
    LOG(INFO) << "Time for parallel recompression: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";

//    std::cout << "RESULT dataset=" << dataset << " algo=parallel_recomp" << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
//              << " rlslp_size=" << rlslp.size() << " terminals=" << rlslp.terminals << std::endl;
    
    std::string res = rlslp.derive_text();
    rlslp.resize(0);
    rlslp.shrink_to_fit();

    std::string c_text;
    recomp::util::read_text_file(file_name, c_text);
    if (res == c_text) {
        std::cerr << "Correct" << std::endl;
    } else {
        std::cerr << "Failure" << std::endl;
    }

//    std::cout << std::to_string(rlslp) << std::endl;
//    std::cout << c_text;
}
