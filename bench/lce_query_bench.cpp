#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include <sdsl/rmq_support.hpp>

#include <prezzalce/lce.hpp>
#include <prezzalce/util.hpp>

#include "recompression.hpp"


int main(int argc, char *argv[]) {
    if (argc < 9) {
        std::cerr << "./bench_lce_query [path] [file_name(s)] [recomp (parallel_lp) | naive | prezza | rmq] [repeats] [begin] [end] [step] [z | w] [coder (fixed | plain | sorted)] [rlslp_path] [prefix]" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    size_t repeats = (size_t)recomp::util::str_to_int(argv[4]);

    std::srand(0);

    size_t begin = (size_t) recomp::util::str_to_int(argv[5]);
    size_t end = (size_t) recomp::util::str_to_int(argv[6]);
    size_t step = (size_t) recomp::util::str_to_int(argv[7]);

    std::string z = argv[8];

    std::string coder;
    if (argc > 9) {
        coder = std::string(argv[9]);
    }
    std::string rlslp_path;
    if (argc > 10) {
        rlslp_path = std::string(argv[10]);
    }

    size_t prefix = 0;
    if (argc > 11) {
        prefix = (size_t)recomp::util::str_to_int(argv[11]);
    }

    for (size_t k = 0; k < files.size(); ++k) {
        std::string file_name = argv[1] + files[k];
        size_t file_size = recomp::util::file_size_in_bytes(file_name);
        if (prefix > 0) {
            file_size = std::min(file_size, prefix);
        }

        lce::lceDataStructure prezza;
        lce::buildLCEDataStructure(&prezza, file_name);

        size_t pos = file_name.find_last_of('/');
        std::string dataset;
        if (pos != std::string::npos) {
            dataset = file_name.substr(pos + 1);
        } else {
            dataset = file_name;
        }

        recomp::util::replace_all(dataset, "_", "\\_");

        recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;
        if (!coder.empty()) {
            std::string coder_file;
            if (!rlslp_path.empty()) {
                coder_file = rlslp_path + files[k];
            } else {
                coder_file = file_name;
            }

            std::cout << "Load" << std::endl;
            rlslp = recomp::coder::decode(coder, coder_file);
            if (rlslp.is_empty) {
                std::cout << "Unknown coder '" + coder + "'. Generating rlslp with parallel_lp_recompression."
                          << std::endl;
                typedef recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
                text_t text;
                if (z == "z") {
                    recomp::util::read_file(file_name, text, prefix);
                } else {
                    recomp::util::read_file_without_zeroes(file_name, text, prefix);
                }

                recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t> recompression;
                recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
            }
            std::cout << "Loaded" << std::endl;
        }
        if (coder.empty()) {
            typedef recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
            text_t text;
            if (z == "z") {
                recomp::util::read_file(file_name, text, prefix);
            } else {
                recomp::util::read_file_without_zeroes(file_name, text, prefix);
            }

            recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t> recompression;
            recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
        }

        std::string plain_text;
        if (z == "z") {
            recomp::util::read_text_file(file_name, plain_text, prefix);
        } else {
            recomp::util::read_text_file_without_zeroes(file_name, plain_text, prefix);
        }

        sdsl::lcp_dac<> lcp;
        sdsl::construct(lcp, file_name, 1);

        sdsl::csa_sada<> sa;
        sdsl::construct(sa, file_name, 1);

        sdsl::rmq_succinct_sct<> rmq(&lcp);

        for (size_t accesses = begin; accesses <= end; accesses += step) {
            std::vector<size_t> indices(accesses);
            for (size_t i = 0; i < accesses; ++i) {
                indices[i] = recomp::util::random_number(file_size);
            }

            for (size_t repeat = 0; repeat < repeats; ++repeat) {
                std::vector<size_t> lces(algos.size(), 0);
                for (size_t l = 0; l < algos.size(); ++l) {
                    std::cout << "Iteration: " << repeat << std::endl;
                    std::string algo = algos[l];
                    std::cout << "Using algo " << algo << std::endl;

                    if (algo == "recomp") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < accesses; ++i) {
                            lces[l] += recomp::lce_query::lce_query(rlslp, indices[i], indices[(i + 1) % accesses]);
                        }
                        const auto endTime = recomp::timer::now();
                        const auto timeSpan = endTime - startTime;
                        std::cout << "RESULT algo=recompression dataset=" << dataset << " queries=" << accesses
                                  << " lce="
                                  << lces[l] << " time="
                                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                  << std::endl;
                    } else if (algo == "naive") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < accesses; ++i) {
                            lces[l] += recomp::lce_query::lce_query_naive(plain_text, indices[i],
                                                                          indices[(i + 1) % accesses]);
                        }
                        const auto endTime = recomp::timer::now();
                        const auto timeSpan = endTime - startTime;
                        std::cout << "RESULT algo=naive dataset=" << dataset << " queries=" << accesses << " lce="
                                  << lces[l] << " time="
                                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                  << std::endl;
                    } else if (algo == "prezza") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < accesses; ++i) {
                            lces[l] += lce::fastlce(&prezza, indices[i], indices[(i + 1) % accesses]);
                        }
                        const auto endTime = recomp::timer::now();
                        const auto timeSpan = endTime - startTime;
                        std::cout << "RESULT algo=prezza dataset=" << dataset << " queries=" << accesses << " lce="
                                  << lces[l] << " time="
                                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                  << std::endl;
                    } else if (algo == "rmq") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < accesses; ++i) {
                            auto isa_i = sa.isa[indices[i]];
                            auto isa_i1 = sa.isa[indices[(i + 1) % accesses]];
                            auto left = std::min(isa_i, isa_i1);
                            auto right = std::max(isa_i, isa_i1);
                            if (left == right) {
                                lces[l] += plain_text.size() - indices[i];
                            } else {
                                //                            std::cout << "l: " << left << ", r: " << right << std::endl;
//                            std::cout << "rmq: " << rmq(std::min(left, right), std::max(left, right)) << std::endl;
//                            std::cout << "lcp size: " << lcp.size() << std::endl;
//                            std::cout << "lcp: " << lcp[rmq(std::min(left, right), std::max(l, right))] << std::endl;
                                lces[l] += lcp[rmq(left + 1, right)];
                            }
                        }
                        const auto endTime = recomp::timer::now();
                        const auto timeSpan = endTime - startTime;
                        std::cout << "RESULT algo=rmq dataset=" << dataset << " queries=" << accesses << " lce="
                                  << lces[l] << " time="
                                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                  << std::endl;
                    } else {
                        std::cout << "No such algorithm '" << algo << "'." << std::endl;
                        return -1;
                    }
                }
                for (size_t l = 0; l < algos.size() - 1; ++l) {
                    if (lces[l] != lces[l + 1]) {
                        std::cout << "Failure: " << algos[l] << " " << lces[l] << " != " << lces[l + 1] << " "
                                  << algos[l + 1] << std::endl;
                    }
                }
            }
        }
    }

    return 0;

//    if (argc < 7) {
//        std::cerr << "lce_query_bench [path] [file_name(s)] [recomp (parallel_ls) | naive | rmq] [repeats] i j" << std::endl;
//        return -1;
//    }
//
//    std::vector<std::string> files;
//    recomp::util::split(argv[2], " ", files);
//
//    std::vector<std::string> algos;
//    recomp::util::split(argv[3], " ", algos);
//
//    size_t repeats = (size_t)recomp::util::str_to_int(argv[4]);
//
//    auto i = (size_t)recomp::util::str_to_int(argv[5]);
//    auto j = (size_t)recomp::util::str_to_int(argv[6]);
//
//    for (size_t k = 0; k < files.size(); ++k) {
//        for (size_t repeat = 0; repeat < repeats; ++repeat) {
//            for (size_t l = 0; l < algos.size(); ++l) {
//                std::cout << "Iteration: " << repeat << std::endl;
//                std::string algo = algos[l];
//                std::cout << "Using algo " << algo << std::endl;
//
//                std::string file_name = argv[1] + files[k];
//
//                size_t pos = file_name.find_last_of('/');
//                std::string dataset;
//                if (pos != std::string::npos) {
//                    dataset = file_name.substr(pos + 1);
//                } else {
//                    dataset = file_name;
//                }
//
//                recomp::util::replace_all(dataset, "_", "\\_");
//
//                if (algo == "recomp") {
//                    typedef recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t>::text_t text_t;
//                    text_t text;
//                    recomp::util::read_file(file_name, text);
//
//                    recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;
//                    recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t> recompression;
//                    recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
//
//                    const auto startTime = recomp::timer::now();
//
//                    auto lce = recomp::lce_query::lce_query(rlslp, i, j);
//
//                    const auto endTime = recomp::timer::now();
//                    const auto timeSpan = endTime - startTime;
//                    std::cout << "RESULT algo=recompression lce=" << lce << " i=" << i << " j=" << j << " time="
//                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
//                } else if (algo == "naive") {
//                    typedef recomp::parallel::recompression_order_ls<recomp::var_t, recomp::term_t>::text_t text_t;
//                    text_t text;
//                    recomp::util::read_file(file_name, text);
//
////                    size_t sum_lce = 0;
////                    size_t max_i = 0;
////                    size_t max_j = 0;
////                    size_t max_lce = 0;
////                    size_t sum = 0;
////                    for (size_t i = 0; i < text.size(); ++i) {
////                        for (size_t j = i + 1; j < text.size(); ++j) {
//                            const auto startTime = recomp::timer::now();
//                            size_t lce = recomp::lce_query::lce_query_naive(text, i, j);
//                            const auto endTime = recomp::timer::now();
//                            const auto timeSpan = endTime - startTime;
//                            std::cout << "RESULT algo=naive lce=" << lce << " i=" << i << " j=" << j << " time="
//                                      << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
//                                      << std::endl;
////                            sum_lce += lce;
////                            sum++;
////                            if (max_lce < lce) {
////                                max_lce = lce;
////                                max_i = i;
////                                max_j = j;
////                            }
////                        }
////                    }
////                    std::cout << "RESULT algo=naive_lce max_lce=" << max_lce << " max_i=" << max_i << " max_j="
////                              << max_j << " lces=" << sum_lce << " sum=" << sum << std::endl;
//                } else if (algo == "rmq") {
//                    // TODO(Chris): RMQ on lcp array
//                    // compute first A, then H, then ISA and then RMQ data structure on H
//                    // LCE(i,j) = RMQ_H(ISA[i], ISA[j])
////                    sdsl::csa_wt<> csa;
//                    sdsl::csa_sada<> csa;
//                    sdsl::construct(csa, file_name, 1);
////                    sdsl::lcp
////                    sdsl::construct()
//                    sdsl::rmq_succinct_sada<> rmq;
////                    rmq = sdsl::rmq_succinct_sada();
//                    const auto startTime = recomp::timer::now();
//                    auto lce = 0;
//                    const auto endTime = recomp::timer::now();
//                    const auto timeSpan = endTime - startTime;
//                    std::cout << "RESULT algo=rmq lce=" << lce << " i=" << i << " j=" << j << " time="
//                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
//                } else {
//                    std::cout << "No such algorithm '" << algo << "'." << std::endl;
//                    return -1;
//                }
//            }
//        }
//    }

    return 0;
}
