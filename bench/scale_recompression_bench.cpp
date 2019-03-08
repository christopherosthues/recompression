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
#include "parallel_lp_recompression.hpp"
#include "full_parallel_recompression.hpp"
#include "parallel_order_less_recompression.hpp"
#include "parallel_order_great_recompression.hpp"
#include "parallel_rnd_recompression.hpp"


int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cerr << "./scale_recompression_bench [path] [file_name(s)] [parallel | full_parallel | parallel_lp | parallel_ls | parallel_gr | parallel_rnd] [cores] [repeats] [begin] [steps]" << std::endl;
        return -1;
    }

    size_t cores = (size_t)recomp::util::str_to_int(argv[4]);
    std::cout << "Using max " << cores << " cores" << std::endl;
    if (cores <= 0) {
        return -1;
    }

    size_t repeats = (size_t)recomp::util::str_to_int(argv[5]);
    std::cout << "Using " << repeats << " repeats" << std::endl;
    if (repeats <= 0) {
        return -1;
    }

    size_t begin = (size_t)recomp::util::str_to_int(argv[6]);
    std::cout << "Begin with " << begin << std::endl;
    if (begin <= 0) {
        return -1;
    }

    size_t steps = (size_t)recomp::util::str_to_int(argv[7]);
    std::cout << "Using " << steps << " steps" << std::endl;
    if (steps <= 0) {
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    for (size_t j = 0; j < files.size(); ++j) {
        for (size_t step = 1; step <= cores;) {
            for (size_t repeat = 0; repeat < repeats; ++repeat) {
                for (size_t i = 0; i < algos.size(); ++i) {
                    std::cout << "Iteration: " << repeat << std::endl;
                    std::string algo = algos[i];
                    std::cout << "Using algo " << algo << std::endl;
                    std::cout << "Using " << step << " cores" << std::endl;

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

                    std::unique_ptr<recomp::recompression<recomp::var_t, recomp::term_t>> recomp;

                    if (algo == "parallel") {
                        recomp = std::make_unique<recomp::parallel::parallel_recompression<recomp::var_t, recomp::term_t>>(
                                dataset);
                    } else if (algo == "parallel_lp") {
                        recomp = std::make_unique<recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>>(
                                dataset);
                    } else if (algo == "parallel_rnd") {
                        recomp = std::make_unique<recomp::parallel::parallel_rnd_recompression<recomp::var_t, recomp::term_t>>(
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
                    } else {
                        std::cerr << "No such algo " << algo << std::endl;
                        return -1;
                    }

                    typedef recomp::recompression<recomp::var_t, recomp::term_t>::text_t text_t;
                    text_t text;
                    recomp::util::read_file(file_name, text);

                    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;

                    const auto startTime = recomp::timer::now();

                    recomp->recomp(text, rlslp, recomp::CHAR_ALPHABET, step);
                    const auto endTime = recomp::timer::now();
                    const auto timeSpan = endTime - startTime;
                    std::cout << "Time for " << algo << " recompression: "
                              << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]"
                              << std::endl;
                    std::cout << "Time for " << algo << " recompression: "
                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]"
                              << std::endl;

                    std::string res = rlslp.derive_text();
                    rlslp.resize(0);
                    rlslp.shrink_to_fit();

                    std::string c_text;
                    recomp::util::read_text_file(file_name, c_text);
                    if (res == c_text) {
                        std::cout << "Correct" << std::endl;
                    } else {
                        std::cout << "Failure" << std::endl;
                    }
                }
            }
            if (step == 1) {
                if (begin != 1) {
                    step = begin;
                } else {
                    step = 2;
                }
            } else {
                step += steps;
            }
        }
    }
}
