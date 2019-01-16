
#include <chrono>
#include <iostream>
#include <regex>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"
#include "util.hpp"
#define private public
#include "recompression.hpp"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "./recomp_bench [file name]" << std::endl;
    }
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    recomp::recompression<recomp::var_t, recomp::term_t> recompression;
    typedef recomp::recompression<recomp::var_t, recomp::term_t>::text_t text_t;
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
    dataset = std::regex_replace(dataset, reg, "\\_");

    const auto startTime = std::chrono::system_clock::now();

    recompression.recomp(text, rlslp);
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    LOG(INFO) << "Time for sequential recompression: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
    LOG(INFO) << "Time for sequential recompression: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";

    std::cout << "RESULT dataset=" << dataset << " algo=sequential_recomp" << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;
}
