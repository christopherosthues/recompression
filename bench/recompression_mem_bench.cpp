#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "defs.hpp"
#include "util.hpp"
#include "recompression.hpp"

#include "parallel_recompression.hpp"
#include "full_parallel_recompression.hpp"
#include "parallel_order_less_recompression.hpp"
#include "parallel_order_great_recompression.hpp"
#include "parallel_lp_recompression.hpp"
#include "parallel_rnd_recompression.hpp"

#include "fast_recompression.hpp"
#include "recompression_hash.hpp"

#ifdef MALLOC_COUNT
#include "malloc_count.h"
#endif


int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cerr << "./bench_recompression_mem [path] [file_name(s)] [parallel | parallel_lp | full_parallel | parallel_ls | parallel_gr | fast | hash | parallel_rnd] [cores] [repeats] [prefix]" << std::endl;
        return -1;
    }

    size_t cores = (size_t)recomp::util::str_to_int(argv[4]);
    std::cout << "Using " << cores << " cores" << std::endl;
    if (cores <= 0) {
        return -1;
    }

    size_t repeats = (size_t)recomp::util::str_to_int(argv[5]);
    std::cout << "Using " << repeats << " repeats" << std::endl;
    if (repeats <= 0) {
        return -1;
    }

    size_t prefix = 0;
    if (argc > 6) {
        prefix = (size_t)recomp::util::str_to_int(argv[6]);
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    for (size_t j = 0; j < files.size(); ++j) {
        for (size_t repeat = 0; repeat < repeats; ++repeat) {
            for (size_t i = 0; i < algos.size(); ++i) {
                std::cout << "Iteration: " << repeat << std::endl;
                std::string algo = algos[i];
                std::cout << "Using algo " << algo << std::endl;

                std::string file_name(argv[1]);
                file_name += files[j];

                size_t pos = file_name.find_last_of('/');
                std::string dataset;
                if (pos != std::string::npos) {
                    dataset = file_name.substr(pos + 1);
                } else {
                    dataset = file_name;
                }

                recomp::util::replace_all(dataset, "_", "\\_");

#ifdef MALLOC_COUNT
                malloc_count_reset_peak();
#endif

                std::unique_ptr<recomp::recompression<recomp::var_t, recomp::term_t>> recomp;

                if (algo == "parallel_lp") {
                    recomp = std::make_unique<recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>>(
                            dataset);
                } else if (algo == "parallel_rnd") {
                    recomp = std::make_unique<recomp::parallel::parallel_rnd_recompression<recomp::var_t, recomp::term_t>>(
                            dataset);
                } else if (algo == "parallel") {
                    recomp = std::make_unique<recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t>>(
                            dataset);
                } else if (algo == "full_parallel") {
                    recomp = std::make_unique<recomp::parallel::full_parallel_recompression<recomp::var_t, recomp::term_t>>(
                            dataset);
                } else if (algo == "parallel_ls") {
                    recomp = std::make_unique<recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t>>(
                            dataset);
                } else if (algo == "parallel_gr") {
                    recomp = std::make_unique<recomp::parallel::recompression_order_gr<recomp::var_t, recomp::term_t>>(
                            dataset);
                } else if (algo == "fast") {
                    recomp = std::make_unique<recomp::recompression_fast<recomp::var_t, recomp::term_t>>(dataset);
                } else if (algo == "hash") {
                    recomp = std::make_unique<recomp::recompression_hash<recomp::var_t, recomp::term_t>>(dataset);
                } else {
                    std::cerr << "No such algo " << algo << std::endl;
                    return -1;
                }

                typedef recomp::recompression<recomp::var_t, recomp::term_t>::text_t text_t;
                text_t text;
                recomp::util::read_file(file_name, text, prefix);

                recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;

                const auto startTime = recomp::timer::now();

                recomp->recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
                const auto endTime = recomp::timer::now();
                const auto timeSpan = endTime - startTime;
#ifdef MALLOC_COUNT
                malloc_count_reset_peak();
                std::cout << "RESULT algo=" << recomp->name << "_recompression dataset=" << dataset
                          << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level="
                          << recomp->level << " cores=" << cores << " memory=" << malloc_count_peak() << std::endl;
#endif // MALLOC_COUNT
                std::cout << "Time for " << algo << " recompression: "
                          << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
                std::cout << "Time for " << algo << " recompression: "
                          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]"
                          << std::endl;
////
////#ifdef MALLOC_COUNT
////                malloc_count_reset_peak();
////                std::cout << "Memory: " << malloc_count_peak() << std::endl;
////#endif // MALLOC_COUNT
//                std::string res = rlslp.derive_text();
//                rlslp.resize(0);
//                rlslp.shrink_to_fit();
//
//                std::string c_text;
//                recomp::util::read_text_file(file_name, c_text, prefix);
//                if (res == c_text) {
//                    std::cout << "Correct" << std::endl;
//                } else {
//                    std::cout << "Failure" << std::endl;
//                }
////#ifdef MALLOC_COUNT
////                std::cout << "Memory: " << malloc_count_peak() << std::endl;
////                malloc_count_reset_peak();
////                std::cout << "Memory: " << malloc_count_peak() << std::endl;
////#endif // MALLOC_COUNT
            }
        }
    }
}
