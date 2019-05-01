#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <tlx/cmdline_parser.hpp>

#include "recompression.hpp"

int main(int argc, char *argv[]) {
    tlx::CmdlineParser cmd;
    cmd.set_description("Generates the rlslp from the recompression and stores it using the given coder.");
    cmd.set_author("Christopher Osthues <osthues.christopher@web.de>");

    std::string path;
    cmd.add_param_string("path", path, "The path to the directory containing the files");

    std::string filenames;
    cmd.add_param_string("filenames", filenames,
                         "The files. Multiple files are seperated with spaces and are enclosed by \"\". Example: \"file1 file2 file3\"");

    std::string algorithms;
    cmd.add_param_string("algorithms", algorithms,
                         "The algorithms to benchmark [\"parallel | parallel_ls | parallel_lock | parallel_lp | parallel_order_ls | parallel_order_gr | fast | hash | parallel_rnd\"]");

    std::string coder;
    cmd.add_param_string("coder", coder, "The coder to store the rlslp to file (plain | fixed | sorted)");

    std::string to_path;
    cmd.add_param_string("to_path", to_path, "The path to the directory to store the rlslp file to");

    size_t cores;
    cmd.add_param_bytes("cores", cores, "The number of cores to use");

    std::string z;
    cmd.add_param_string("zeroes", z, "Read file with zero symbol (z) or without (w)");

    size_t prefix = 0;
    cmd.add_bytes('p', "prefix", prefix, "The prefix of the files in bytes to read in");

    if (!cmd.process(argc, argv)) {
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(filenames, " ", files);

    std::vector<std::string> algos;
    recomp::util::split(algorithms, " ", algos);

    for (size_t j = 0; j < files.size(); ++j) {
        for (size_t i = 0; i < algos.size(); ++i) {
            std::string algo = algos[i];
            std::cout << "Using algo " << algo << std::endl;

            std::string file_name = path;
            file_name += files[j];

            size_t pos = file_name.find_last_of('/');
            std::string dataset;
            if (pos != std::string::npos) {
                dataset = file_name.substr(pos + 1);
            } else {
                dataset = file_name;
            }

            recomp::util::replace_all(dataset, "_", "\\_");

            std::unique_ptr<recomp::recompression<recomp::var_t>> recomp = recomp::create_recompression(algo, dataset);
            if (!recomp) {
                std::cerr << "No such algo " << algo << std::endl;
                return -1;
            }

            typedef recomp::recompression<recomp::var_t>::text_t text_t;
            text_t text;
            if (z == "z") {
                recomp::util::read_file(file_name, text, prefix);
            } else {
                recomp::util::read_file_without_zeroes(file_name, text, prefix);
            }

            recomp::rlslp<recomp::var_t> rlslp;

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
            if (z == "z") {
                recomp::util::read_text_file(file_name, c_text, prefix);
            } else {
                recomp::util::read_text_file_without_zeroes(file_name, c_text, prefix);
            }
            if (res == c_text) {
                std::cout << "Correct extract" << std::endl;
            } else {
                std::cout << "Failure extract" << std::endl;
            }

            std::string coder_file = to_path + files[j];
            if (z == "w") {
                coder_file += "_wz";
            }
            recomp::rlslp<recomp::var_t> in_rlslp;
            recomp::coder::encode(coder, coder_file, rlslp);
            in_rlslp = recomp::coder::decode(coder, coder_file);

            if (in_rlslp.is_empty) {
                return -1;
            }

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
