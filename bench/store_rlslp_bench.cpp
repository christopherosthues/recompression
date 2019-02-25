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

#include "sequential_recompression.hpp"
#include "fast_recompression.hpp"
#include "recompression_hash.hpp"

#include "coders/plain_rlslp_coder.hpp"
#include "coders/plain_rlslp_wlz_coder.hpp"

int main(int argc, char *argv[]) {
    if (argc < 8) {
        std::cerr << "./store_rlslp_bench [path] [file_name(s)] [sequential | parallel | full_parallel | parallel_ls | parallel_gr | fast | hash] [plain | wlz | sorted] [to_path] [cores] [repeats]" << std::endl;
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

                std::unique_ptr<recomp::recompression<recomp::var_t, recomp::term_t>> recomp;

                if (algo == "sequential") {
                    recomp = std::make_unique<recomp::sequential_recompression<recomp::var_t, recomp::term_t>>(dataset);
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
                recomp::util::read_file<text_t>(file_name, text);

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
//                rlslp.resize(0);
//                rlslp.shrink_to_fit();
                std::string c_text;
                recomp::util::read_text_file(file_name, c_text);
                if (res == c_text) {
                    std::cout << "Correct extract" << std::endl;
                } else {
                    std::cout << "Failure extract" << std::endl;
                }

                if (coder == "plain") {
                    std::string coder_file = to_path + files[j] + ".plain";
                    recomp::coder::PlainRLSLPCoder::Encoder enc{coder_file};
                    enc.encode(rlslp);

                    recomp::coder::PlainRLSLPCoder::Decoder dec{coder_file};
                    recomp::rlslp<recomp::var_t, recomp::term_t> in_rlslp = dec.decode();

                    std::ifstream in_enc(coder_file, std::ios::binary | std::ios::ate);
                    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
                    std::cout << "RESULT algo=" << recomp->name << "_recompression dataset=" << dataset << " coder="
                              << coder << " size=" << in.tellg() << " enc_size=" << in_enc.tellg()
                              << " productions=" << rlslp.size() << std::endl;
                    in.close();
                    in_enc.close();

                    if (rlslp == in_rlslp && rlslp.derive_text() == in_rlslp.derive_text()) {
                        std::cout << "Correct store" << std::endl;
                    } else {
                        std::cout << "Failure store" << std::endl;
                    }
                } else if (coder == "wlz") {
                    std::string coder_file = to_path + files[j] + ".wlz";
                    recomp::coder::PlainRLSLPWLZCoder::Encoder enc{coder_file};
                    enc.encode(rlslp);

                    recomp::coder::PlainRLSLPWLZCoder::Decoder dec{coder_file};
                    recomp::rlslp<recomp::var_t, recomp::term_t> in_rlslp = dec.decode();

                    std::ifstream in_enc(coder_file, std::ios::binary | std::ios::ate);
                    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
                    std::cout << "RESULT algo=" << recomp->name << "_recompression dataset=" << dataset << " coder="
                              << coder << " size=" << in.tellg() << " enc_size=" << in_enc.tellg()
                              << " productions=" << rlslp.size() << std::endl;
                    in.close();
                    in_enc.close();

                    if (rlslp == in_rlslp && rlslp.derive_text() == in_rlslp.derive_text()) {
                        std::cout << "Correct store" << std::endl;
                    } else {
                        std::cout << "Failure store" << std::endl;
                    }
                } else if (coder == "sorted") {
                    std::cout << "not supported jet" << std::endl;
                } else {
                    std::cout << "Coder '" << coder << "' unkown." << std::endl;
                }

            }
        }
    }
}
