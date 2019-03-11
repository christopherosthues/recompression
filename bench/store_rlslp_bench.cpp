#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "recompression.hpp"

int main(int argc, char *argv[]) {
    if (argc < 8) {
        std::cerr << "./store_rlslp_bench [path] [file_name(s)] [parallel | parallel_lock | parallel_lp | parallel_ls | parallel_gr | fast | hash | parallel_rnd] [plain | wlz | sorted | sorted_dr] [to_path] [cores] [repeats] [prefix]" << std::endl;
        return -1;
    }

    size_t cores = (size_t)recomp::util::str_to_int(argv[6]);
    std::cout << "Using " << cores << " threads" << std::endl;
    if (cores <= 0) {
        return -1;
    }

    size_t repeats = (size_t)recomp::util::str_to_int(argv[7]);
    std::cout << "Using " << repeats << " repeats" << std::endl;
    if (repeats <= 0) {
        return -1;
    }

    size_t prefix = 0;
    if (argc > 8) {
        prefix = (size_t)recomp::util::str_to_int(argv[8]);
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> algos;
    recomp::util::split(argv[3], " ", algos);

    std::string coder(argv[4]);

    std::string to_path(argv[5]);

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

                std::unique_ptr<recomp::recompression<recomp::var_t, recomp::term_t>> recomp = recomp::create_recompression(algo, dataset);
                if (!recomp) {
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
                std::cout << "Time for " << algo << " recompression: "
                          << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
                std::cout << "Time for " << algo << " recompression: "
                          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]"
                          << std::endl;

                std::string res = rlslp.derive_text();
                std::string c_text;
                recomp::util::read_text_file(file_name, c_text, prefix);
                if (res == c_text) {
                    std::cout << "Correct extract" << std::endl;
                } else {
                    std::cout << "Failure extract" << std::endl;
                }

                std::string coder_file = to_path + files[j];
                recomp::rlslp<recomp::var_t, recomp::term_t> in_rlslp;
                recomp::coder::encode(coder, coder_file, rlslp);
                in_rlslp = recomp::coder::decode(coder, coder_file);

                if (in_rlslp.is_empty) {
                    return -1;
                }
//                if (coder == "plain") {
//                    recomp::coder::PlainRLSLPCoder::Encoder enc{coder_file};
//                    enc.encode(rlslp);
//
//                    recomp::coder::PlainRLSLPCoder::Decoder dec{coder_file};
//                    in_rlslp = dec.decode();
//                } else if (coder == "plain_fixed") {
//                    recomp::coder::PlainFixedRLSLPCoder::Encoder enc{coder_file};
//                    enc.encode(rlslp);
//
//                    recomp::coder::PlainFixedRLSLPCoder::Decoder dec{coder_file};
//                    in_rlslp = dec.decode();
//                } else if (coder == "sorted") {
//                    recomp::coder::SortedRLSLPCoder::Encoder enc{coder_file};
//                    enc.encode(rlslp);
//
//                    recomp::coder::SortedRLSLPCoder::Decoder dec{coder_file};
//                    in_rlslp = dec.decode();
//                } else if (coder == "sorted_dr") {
//                    recomp::coder::SortedRLSLPDRCoder::Encoder enc{coder_file};
//                    enc.encode(rlslp);
//
//                    recomp::coder::SortedRLSLPDRCoder::Decoder dec{coder_file};
//                    in_rlslp = dec.decode();
//                } else {
//                    std::cout << "Unkown coder '" << coder << "'." << std::endl;
//                    return -1;
//                }

                std::string extension = recomp::coder::get_coder_extension(coder);
                std::ifstream in_enc(coder_file + extension, std::ios::binary | std::ios::ate);
                std::cout << "RESULT algo=" << recomp->name << "_recompression dataset=" << dataset << " coder="
                          << coder << " size=" << recomp::util::file_size_in_bytes(file_name, prefix) << " enc_size="
                          << in_enc.tellg() << " productions=" << rlslp.size() << std::endl;
                in_enc.close();

                if (rlslp == in_rlslp && rlslp.derive_text() == in_rlslp.derive_text()) {
                    std::cout << "Correct store" << std::endl;
                } else {
                    std::cout << "Failure store" << std::endl;
                }

                if (coder == "sorted" || coder == "sorted_dr") {
                    recomp::sort_rlslp_rules(rlslp);

                    if (rlslp.blocks > 0) {
                        for (size_t m = 0; m < rlslp.blocks - 1; ++m) {
                            if (rlslp[m].first() > rlslp[m + 1].first()) {
                                std::cout << "Pairs not sorted" << std::endl;
                                std::cout << rlslp[m].first() << ", " << rlslp[m + 1].first() << std::endl;
                            }
                        }
                    }

                    if (rlslp.size() > 0) {
                        for (size_t m = rlslp.blocks; m < rlslp.size() - 1; ++m) {
                            if (rlslp[m].first() > rlslp[m + 1].first()) {
                                std::cout << "Blocks not sorted" << std::endl;
                                std::cout << rlslp[m].first() << ", " << rlslp[m + 1].first() << std::endl;
                            }
                        }
                    }

                    if (rlslp.derive_text() == c_text) {
                        std::cout << "Correct store" << std::endl;
                    } else {
                        std::cout << "Failure store" << std::endl;
                    }
                }
            }
        }
    }
}
