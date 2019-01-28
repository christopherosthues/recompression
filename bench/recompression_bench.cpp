
#include <chrono>
#include <iostream>
#include <memory>
#include <regex>
#include <thread>
#include <vector>

#include "defs.hpp"
#include "util.hpp"
#include "recompression.hpp"

#include "parallel_recompression.hpp"
#include "full_parallel_recompression.hpp"
#include "parallel_order_less_recompression.hpp"
#include "parallel_order_great_recompression.hpp"

#include "sequential_recompression.hpp"
#include "fast_recompression.hpp"

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
    if (argc < 3) {
        std::cerr << "./recompression_bench [file name] [sequential | parallel | full_parallel | parallel_ls | "
                     "parallel_gr | fast] [cores] [output]" << std::endl;
    }

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

    std::string algo = argv[2];

    std::cout << "Using algo " << algo << std::endl;

    int cores = std::thread::hardware_concurrency();
    if (argc > 3) {
        cores = str_to_int(argv[3]);
    }
    std::cout << "Using " << cores << " threads" << std::endl;

    std::unique_ptr<recomp::recompression<recomp::var_t, recomp::term_t>> recomp;

    if (algo == "sequential") {
        recomp = std::make_unique<recomp::sequential_recompression<recomp::var_t, recomp::term_t>>(dataset);
    } else if (algo == "parallel") {
        recomp = std::make_unique<recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t>>(dataset);
    } else if (algo == "full_parallel") {
        recomp = std::make_unique<recomp::parallel::full_parallel_recompression<recomp::var_t, recomp::term_t>>(dataset);
    } else if (algo == "parallel_ls") {
        recomp = std::make_unique<recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t>>(dataset);
    } else if (algo == "parallel_gr") {
        recomp = std::make_unique<recomp::parallel::recompression_order_gr<recomp::var_t, recomp::term_t>>(dataset);
    } else if (algo == "fast") {
        recomp = std::make_unique<recomp::recompression_fast<recomp::var_t, recomp::term_t>>(dataset);
    } else {
        std::cerr << "No such algo " << algo << std::endl;
        return -1;
    }

    typedef recomp::recompression<recomp::var_t, recomp::term_t>::text_t text_t;
    text_t text;
    recomp::util::read_file<text_t>(file_name, text);

    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;

    const auto startTime = std::chrono::system_clock::now();

    recomp->recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << "Time for " << algo << " recompression: "
              << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
    std::cout << "Time for " << algo << " recompression: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;

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

    if (argc > 4) {
        std::string output(argv[4]);
        if (output == "output") {
            std::cout << res;
        }
    }
}
