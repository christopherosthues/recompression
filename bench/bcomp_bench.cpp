
#include <chrono>
#include <iostream>
#include <regex>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"
#define private public
#include "parallel_recompression.hpp"
#include "util.hpp"


int main(int argc, char *argv[]) {
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    recomp::recompression<recomp::var_t, recomp::term_t> recompression;
    recomp::recompression<recomp::var_t, recomp::term_t>::text_t text;
    std::string file_name(argv[1]);
//    recomp::util::read_file(file_name, text);

    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;

    if (text.size() < 31) {
        for (size_t i = 0; i < text.size(); ++i) {
            std::cout << (char) text[i];
        }
        std::cout << std::endl;
    }

    const auto startTime = std::chrono::system_clock::now();

    recompression.bcomp(text, rlslp);
//    recomp::bcomp(text, rlslp);
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    LOG(INFO) << "Time for parallel bcomp: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
    LOG(INFO) << "Time for parallel bcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";

    size_t pos = file_name.find_last_of('/');
    std::string dataset;
    if (pos != std::string::npos) {
        dataset = file_name.substr(pos + 1);
    } else {
        dataset = file_name;
    }

    std::regex reg("_");
    std::regex_replace(dataset, reg, "\\_");

    std::cout << "RESULT dataset=" << dataset << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;
}