
#include <chrono>
#include <iostream>
#include <regex>
#include <vector>

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
    if (argc < 5) {
        std::cerr << "lce_query_bench [filename] [recomp | naive | rmq] i j" << std::endl;
        return -1;
    }

    std::string file_name(argv[1]);
    std::string algo(argv[2]);

    typedef recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t>::text_t text_t;

    text_t text;
    auto i = (size_t)str_to_int(argv[3]);
    auto j = (size_t)str_to_int(argv[4]);
    recomp::util::read_file(file_name, text);
    if (algo == "recomp") {
        recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;
        recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t> recompression;
        recompression.recomp(text, rlslp);

        const auto startTime = std::chrono::system_clock::now();

        auto lce = recomp::lce_query::lce_query(rlslp, i, j);

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=recompression lce=" << lce << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
    } else if (algo == "naive") {
        const auto startTime = std::chrono::system_clock::now();
        size_t lce = recomp::lce_query::lce_query_naive(text, i, j);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=naive lce=" << lce << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
    } else if (algo == "rmq") {
        // TODO(Chris): RMQ on lcp array
        const auto startTime = std::chrono::system_clock::now();
        auto lce = 0;
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=rmq lce=" << lce << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
    } else {
        std::cout << "No such algorithm '" << algo << "'." << std::endl;
        return -1;
    }

    return 0;
}
