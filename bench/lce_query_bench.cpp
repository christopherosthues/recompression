
#include <chrono>
#include <iostream>
#include <regex>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"
#include "lce_query.hpp"
#include "parallel_recompression.hpp"
#include "util.hpp"


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

//    sscanf(input.c_str(), "%zu", &index); // for size_t
//    ss(stringstream) >> size_t
}

int main(int argc, char *argv[]) {
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    if (argc < 5) {
        std::cerr << "lce_query_bench [filename] [recomp | naive | rmq] i j" << std::endl;
        return -1;
    }

    std::string file_name(argv[1]);
    std::string algo(argv[2]);

    typedef recomp::recompression<recomp::var_t, recomp::term_t>::text_t text_t;

    text_t text;
    size_t i = str_to_int(argv[3]);
    size_t j = str_to_int(argv[4]);
    recomp::util::read_file(file_name, text);
    if (algo == "recomp") {
        recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;
        recomp::recompression<recomp::var_t, recomp::term_t> recompression;
        recompression.recomp(text, rlslp);

        const auto startTime = std::chrono::system_clock::now();

        auto lce = recomp::lce_query::lce_query(rlslp, i, j);

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        LOG(INFO) << "Time for LCE query on RLSLP: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
        LOG(INFO) << "Time for LCE query on RLSLP: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
        LOG(INFO) << "LCE query for RLSLP: " << lce;
    } else if (algo == "naive") {
        const auto startTime = std::chrono::system_clock::now();
        size_t lce = recomp::lce_query::lce_query_naive(text, i, j);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        LOG(INFO) << "Time for naive LCE query: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
        LOG(INFO) << "Time for naive LCE query: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
        LOG(INFO) << "naive LCE query: " << lce;
    } else if (algo == "rmq") {
        // TODO(Chris): RMQ on lcp array
        const auto startTime = std::chrono::system_clock::now();

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        LOG(INFO) << "Time for RMQ LCE query: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]";
        LOG(INFO) << "Time for RMQ LCE query: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
    } else {
        std::cerr << "Algorithm '" << algo << "' not known." << std::endl;
        return -1;
    }

    return 0;
}