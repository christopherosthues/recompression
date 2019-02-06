
#include <chrono>
#include <iostream>
#include <vector>

#include <sdsl/construct_sa.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/csa_sada.hpp>
#include <sdsl/csa_wt.hpp>

#include "defs.hpp"
#include "lce_query.hpp"
#include "parallel_recompression.hpp"
#include "parallel_order_less_recompression.hpp"
#include "util.hpp"


int main(int argc, char *argv[]) {
    if (argc < 7) {
        std::cerr << "lce_query_bench [path] [file_name(s)] [recomp (parallel_ls) | naive | rmq] [repeats] i j" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    typedef recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t>::text_t text_t;

    size_t repeats = (size_t)recomp::util::str_to_int(argv[4]);

    text_t text;
    auto i = (size_t)recomp::util::str_to_int(argv[5]);
    auto j = (size_t)recomp::util::str_to_int(argv[6]);

    for (size_t k = 0; k < files.size(); ++k) {
        for (size_t repeat = 0; repeat < repeats; ++repeat) {
            for (size_t l = 0; l < algos.size(); ++l) {
                std::cout << "Iteration: " << repeat << std::endl;
                std::string algo = algos[l];
                std::cout << "Using algo " << algo << std::endl;

                std::string file_name = argv[1] + files[k];

                size_t pos = file_name.find_last_of('/');
                std::string dataset;
                if (pos != std::string::npos) {
                    dataset = file_name.substr(pos + 1);
                } else {
                    dataset = file_name;
                }

                recomp::util::replace_all(dataset, "_", "\\_");

                recomp::util::read_file(file_name, text);
                if (algo == "recomp") {
                    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;
                    recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t> recompression;
                    recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);

                    const auto startTime = recomp::timer::now();

                    auto lce = recomp::lce_query::lce_query(rlslp, i, j);

                    const auto endTime = recomp::timer::now();
                    const auto timeSpan = endTime - startTime;
                    std::cout << "RESULT algo=recompression lce=" << lce << " i=" << i << " j=" << j << " time="
                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
                } else if (algo == "naive") {
                    size_t sum_lce = 0;
                    size_t max_i = 0;
                    size_t max_j = 0;
                    size_t max_lce = 0;
                    size_t sum = 0;
                    for (size_t i = 0; i < text.size(); ++i) {
                        for (size_t j = i + 1; j < text.size(); ++j) {
                            const auto startTime = recomp::timer::now();
                            size_t lce = recomp::lce_query::lce_query_naive(text, i, j);
                            const auto endTime = recomp::timer::now();
                            const auto timeSpan = endTime - startTime;
                            std::cout << "RESULT algo=naive lce=" << lce << " i=" << i << " j=" << j << " time="
                                      << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                      << std::endl;
                            sum_lce += lce;
                            sum++;
                            if (max_lce < lce) {
                                max_lce = lce;
                                max_i = i;
                                max_j = j;
                            }
                        }
                    }
                    std::cout << "RESULT algo=naive_lce max_lce=" << max_lce << " max_i=" << max_i << " max_j="
                              << max_j << " lces=" << sum_lce << " sum=" << sum << std::endl;
                } else if (algo == "rmq") {
                    // TODO(Chris): RMQ on lcp array
                    // compute first A, then H, then ISA and then RMQ data structure on H
                    // LCE(i,j) = RMQ_H(ISA[i], ISA[j])
//                    sdsl::_construct_sa_IS()
                    sdsl::cache_config config;
//                    config.
                    sdsl::construct_sa<8>(config);
                    const auto startTime = recomp::timer::now();
                    auto lce = 0;
                    const auto endTime = recomp::timer::now();
                    const auto timeSpan = endTime - startTime;
                    std::cout << "RESULT algo=rmq lce=" << lce << " i=" << i << " j=" << j << " time="
                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
                } else {
                    std::cout << "No such algorithm '" << algo << "'." << std::endl;
                    return -1;
                }
            }
        }
    }

    return 0;
}
