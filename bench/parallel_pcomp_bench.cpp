
#include <chrono>
#include <iostream>
#include <regex>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"
#include "util.hpp"
#define private public
#include "parallel_recompression.hpp"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "./parallel_pcomp_bench [file name]" << std::endl;
    }
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    recomp::parallel::recompression<recomp::var_t, recomp::term_t> recompression;
    typedef recomp::parallel::recompression<recomp::var_t, recomp::term_t>::text_t text_t;
    std::string file_name(argv[1]);
    text_t text;
    recomp::util::read_file<text_t>(file_name, text);

    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;

    if (text.size() < 31) {
        for (size_t i = 0; i < text.size(); ++i) {
            std::cout << (char) text[i];
        }
        std::cout << std::endl;
    }
    
    size_t pos = file_name.find_last_of('/');
    std::string dataset;
    if (pos != std::string::npos) {
        dataset = file_name.substr(pos + 1);
    } else {
        dataset = file_name;
    }

    std::regex reg("_");
    std::regex_replace(dataset, reg, "\\_");
    
    const auto startTime = std::chrono::system_clock::now();

    std::cout << "RESULT dataset=" << dataset << " algo=parallel_pcomp";
    recompression.pcomp(text, rlslp);
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    LOG(INFO) << "Time for parallel pcomp: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
    LOG(INFO) << "Time for parallel pcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";

    //std::cout << "RESULT dataset=" << dataset << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;
    std::cout << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;
}
