#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <prezzalce/lce.hpp>
#include <prezzalce/util.hpp>
#include <recompression/coders/sorted_rlslp_coder.hpp>

#include "recompression.hpp"

int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cerr << "./bench_random_access [path] [file_name(s)] [recomp (parallel_lp) | naive | prezza] [repeats] [accesses] [coder (fixed | plain | sorted)] [rlslp_path] [prefix]" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    size_t repeats = (size_t)recomp::util::str_to_int(argv[4]);

    std::srand(0);

    size_t accesses = (size_t)recomp::util::str_to_int(argv[5]);
    std::vector<size_t> indices(accesses);

    std::string coder;
    if (argc > 6) {
        coder = std::string(argv[6]);
    }
    std::string rlslp_path;
    if (argc > 7) {
        rlslp_path = std::string(argv[7]);
    }

    size_t prefix = 0;
    if (argc > 8) {
        prefix = (size_t)recomp::util::str_to_int(argv[8]);
    }

    std::vector<std::vector<std::string>> access(algos.size());
    for (size_t i = 0; i < algos.size(); ++i) {
        access[i] = std::vector<std::string>(accesses);
    }
    for (size_t k = 0; k < files.size(); ++k) {
        std::string file_name = argv[1] + files[k];
        size_t file_size = recomp::util::file_size_in_bytes(file_name);
        if (prefix > 0) {
            file_size = std::min(file_size, prefix);
        }

        for (size_t i = 0; i < accesses; ++i) {
            indices[i] = recomp::util::random_number(file_size);
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
                std::cout << "Unknown coder '" + coder + "'. Generating rlslp with parallel_lp_recompression." << std::endl;
                typedef recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
                text_t text;
                recomp::util::read_file(file_name, text, prefix);

                recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t> recompression;
                recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
            }
//            if (coder == "plain") {
//                std::cout << "plain" << std::endl;
//                std::cout << coder_file << std::endl;
//                recomp::coder::PlainRLSLPCoder::Decoder dec{coder_file};
//                rlslp = dec.decode();
//            } else if (coder == "fixed") {
//                std::cout << "fixed" << std::endl;
//                std::cout << coder_file << std::endl;
//                recomp::coder::PlainFixedRLSLPCoder::Decoder dec{coder_file};
//                rlslp = dec.decode();
//            } else if (coder == "sorted") {
//                std::cout << "sorted" << std::endl;
//                std::cout << coder_file << std::endl;
//                recomp::coder::SortedRLSLPCoder::Decoder dec{coder_file};
//                rlslp = dec.decode();
//            } else {
//                std::cout << "Unknown coder '" + coder + "'. Generating rlslp with parallel_lp_recompression." << std::endl;
//                coder = "";
//            }
            std::cout << "Loaded" << std::endl;
        }
        if (coder.empty()) {
            typedef recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
            text_t text;
            recomp::util::read_file(file_name, text, prefix);

            recomp::parallel::parallel_lp_recompression<recomp::var_t, recomp::term_t> recompression;
            recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
        }

        std::string plain_text;
        recomp::util::read_text_file(file_name, plain_text, prefix);

        for (size_t repeat = 0; repeat < repeats; ++repeat) {
            for (size_t l = 0; l < algos.size(); ++l) {
                std::cout << "Iteration: " << repeat << std::endl;
                std::string algo = algos[l];
                std::cout << "Using algo " << algo << std::endl;

                if (algo == "recomp") {
                    const auto startTime = recomp::timer::now();
                    for (size_t i = 0; i < accesses; ++i) {
                        access[l][i] = rlslp.extract(indices[i], 1);
                    }
                    const auto endTime = recomp::timer::now();
                    const auto timeSpan = endTime - startTime;
                    std::cout << "RESULT algo=recompression dataset=" << dataset << " accesses=" << accesses << " time="
                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
                } else if (algo == "naive") {
                    const auto startTime = recomp::timer::now();
                    for (size_t i = 0; i < accesses; ++i) {
                        access[l][i] = plain_text[indices[i]];
                    }
                    const auto endTime = recomp::timer::now();
                    const auto timeSpan = endTime - startTime;
                    std::cout << "RESULT algo=naive dataset=" << dataset << " accesses=" << accesses << " time="
                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
                } else if (algo == "prezza") {
                    const auto startTime = recomp::timer::now();
                    for (size_t i = 0; i < accesses; ++i) {
                        access[l][i] = lce::getChar(&prezza, indices[i]);
                    }
                    const auto endTime = recomp::timer::now();
                    const auto timeSpan = endTime - startTime;
                    std::cout << "RESULT algo=prezza dataset=" << dataset << " accesses=" << accesses << " time="
                              << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
                } else {
                    std::cout << "No such algorithm '" << algo << "'." << std::endl;
                    return -1;
                }
            }
            for (size_t l = 0; l < algos.size() - 1; ++l) {
                for (size_t i = 0; i < access.size(); ++i) {
                    if (access[l][i] != access[l+ 1][i]) {
                        std::cout << "Failure: " << algos[l] << " " << access[l][i] << " != " << access[l + 1][i] << " " << algos[l + 1] << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}
