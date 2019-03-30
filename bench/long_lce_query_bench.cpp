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
        std::cerr << "./bench_long_lce_query [path] [file_name(s)] [recomp (parallel_lp) | naive | prezza | rmq] [repeats] [begin] [end] [step] [w | z] [coder (fixed | plain | sorted)] [rlslp_path] [prefix]" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    size_t repeats = (size_t)recomp::util::str_to_int(argv[4]);

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

        size_t pos = file_name.find_last_of('/');
        std::string dataset;
        if (pos != std::string::npos) {
            dataset = file_name.substr(pos + 1);
        } else {
            dataset = file_name;
        }

        recomp::util::replace_all(dataset, "_", "\\_");

        if (z == "w") {
            file_name += "_wz";
        }

        lce::lceDataStructure prezza;
        lce::buildLCEDataStructure(&prezza, file_name);

        recomp::rlslp<recomp::var_t, recomp::term_t> rlslp;
        if (!coder.empty()) {
            std::string coder_file;
            if (!rlslp_path.empty()) {
                coder_file = rlslp_path + files[k];
                if (z == "w") {
                    coder_file += "_wz";
                }
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
//                if (z == "z") {
                    recomp::util::read_file(file_name, text, prefix);
//                } else {
//                    recomp::util::read_file_without_zeroes(file_name, text, prefix);
//                }

                recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t> recompression;
                recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
            }
            std::cout << "Loaded" << std::endl;
        }
        if (coder.empty()) {
            typedef recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
            text_t text;
//            if (z == "z") {
                recomp::util::read_file(file_name, text, prefix);
//            } else {
//                recomp::util::read_file_without_zeroes(file_name, text, prefix);
//            }

            recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t> recompression;
            recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
        }

        std::string plain_text;
//        if (z == "z") {
            recomp::util::read_text_file(file_name, plain_text, prefix);
//        } else {
//            recomp::util::read_text_file_without_zeroes(file_name, plain_text, prefix);
//        }

        sdsl::lcp_dac<> lcp;
        sdsl::construct(lcp, file_name, 1);

        sdsl::csa_sada<> sa;
        sdsl::construct(sa, file_name, 1);

        sdsl::rmq_succinct_sct<> rmq(&lcp);

        for (size_t accesses = begin; accesses <= end; accesses += step) {
            std::vector<size_t> indices(accesses + accesses);
            std::vector<size_t> idx(plain_text.size());
            for (size_t n = 0; n < idx.size(); ++n) {
                idx[n] = n;
            }

            auto sort_cond = [&](const size_t i, const size_t j) {
//                return rmq(j, j) < rmq(i, i);
//                return lcp[rmq(j, j)] < lcp[rmq(i, i)];
                return lcp[j] < lcp[i];
            };

            ips4o::parallel::sort(idx.begin(), idx.end(), sort_cond, 4);

//            for (size_t kl = 0; kl < accesses; ++kl) {
//                std::cout << lcp[idx[kl]] << std::endl;
//            }

//            for (size_t kl = 0; kl < 20; kl++) {
//                std::cout << idx[kl] << std::endl;
//            }

//            size_t max = 0;
//            for (size_t i = 0; i < plain_text.size(); ++i) {
//                auto rmq_val = rmq(i, i);
//                if (rmq_val > max) {
//                    max = rmq_val;
//                }
//            }
//            std::cout << "Max lce: " << max << std::endl;

            for (size_t i = 0, n = 0; i < indices.size(); i += 2) {
                indices[i] = sa[idx[n]];
                if (idx[n] == 0) {
                    indices[i + 1] = indices[i];
                } else {
                    indices[i + 1] = sa[idx[n] - 1];
                }
//                std::cout << indices[i] << ", " << indices[i + 1] << std::endl;
                n++;
                if (n >= idx.size()) {
                    n = 0;
                }
//                for (size_t kl = 0; kl < accesses; kl+=2) {
//                    std::cout << indices[i] << ", " << indices[i + 1] << std::endl;
//                    std::cout << lcp[sa.isa[indices[i]]] << std::endl;
//                }
            }
            idx.resize(0);
            idx.shrink_to_fit();

            for (size_t repeat = 0; repeat < repeats; ++repeat) {
                std::vector<size_t> lces(algos.size(), 0);
                for (size_t l = 0; l < algos.size(); ++l) {
                    std::cout << "Iteration: " << repeat << std::endl;
                    std::string algo = algos[l];
                    std::cout << "Using algo " << algo << std::endl;

                    if (algo == "recomp") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < indices.size(); i+=2) {
                            lces[l] += recomp::lce_query::lce_query(rlslp, indices[i], indices[i + 1]);
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
                        for (size_t i = 0; i < indices.size(); i+=2) {
                            lces[l] += recomp::lce_query::lce_query_naive(plain_text, indices[i], indices[i + 1]);
                        }
                        const auto endTime = recomp::timer::now();
                        const auto timeSpan = endTime - startTime;
                        std::cout << "RESULT algo=naive dataset=" << dataset << " queries=" << accesses << " lce="
                                  << lces[l] << " time="
                                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                  << std::endl;
                    } else if (algo == "prezza") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < indices.size(); i+=2) {
                            lces[l] += lce::fastlce(&prezza, indices[i], indices[i + 1]);
                        }
                        const auto endTime = recomp::timer::now();
                        const auto timeSpan = endTime - startTime;
                        std::cout << "RESULT algo=prezza dataset=" << dataset << " queries=" << accesses << " lce="
                                  << lces[l] << " time="
                                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                                  << std::endl;
                    } else if (algo == "rmq") {
                        const auto startTime = recomp::timer::now();
                        for (size_t i = 0; i < indices.size(); i+=2) {
                            auto isa_i = sa.isa[indices[i]];
                            auto isa_i1 = sa.isa[indices[i + 1]];
                            auto left = std::min(isa_i, isa_i1);
                            auto right = std::max(isa_i, isa_i1);
                            if (left == right) {
                                lces[l] += plain_text.size() - indices[i];
                            } else {
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
}
