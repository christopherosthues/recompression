#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp_support_sada.hpp>
#include <sdsl/rmq_support.hpp>

#include <prezzalce/lce.hpp>
#include <prezzalce/util.hpp>

#include <tlx/cmdline_parser.hpp>

#include "recompression.hpp"


int main(int argc, char *argv[]) {
    tlx::CmdlineParser cmd;
    cmd.set_description("Benchmark for runtime experiments of random lce queries");
    cmd.set_author("Christopher Osthues <osthues.christopher@web.de>");

    std::string path;
    cmd.add_param_string("path", path, "The path to the directory containing the files");

    std::string filenames;
    cmd.add_param_string("filenames", filenames,
                         "The files. Multiple files are seperated with spaces and are enclosed by \"\". Example: \"file1 file2 file3\"");

    std::string algorithms;
    cmd.add_param_string("algorithms", algorithms,
                         "The algorithms to benchmark [\"recomp (parallel_lp) | naive | prezza | rmq\"]");

    size_t repeats;
    cmd.add_param_bytes("repeats", repeats, "The number of repeats to process");

    size_t begin;
    cmd.add_param_bytes("begin", begin, "The number of queries to begin with");

    size_t end;
    cmd.add_param_bytes("end", end, "The number of queries to end with");

    size_t steps;
    cmd.add_param_bytes("steps", steps, "The steps");

    std::string z;
    cmd.add_param_string("zeroes", z, "Read file with zero symbol (z) or without (w)");

    std::string coder = "";
    cmd.add_string('c', "coder", coder, "The coder to read the rlslp from file (plain | fixed | sorted)");

    std::string rlslp_path = "";
    cmd.add_string('r', "rlslp_path", rlslp_path, "The path to directory containing the rlslp file");

    size_t prefix = 0;
    cmd.add_bytes('p', "prefix", prefix, "The prefix of the files in bytes to read in");

    if (!cmd.process(argc, argv)) {
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(filenames, " ", files);

    std::vector<std::string> algos;
    recomp::util::split(algorithms, " ", algos);

    std::srand(0);

    for (size_t k = 0; k < files.size(); ++k) {
        std::string file_name = path + files[k];

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

        size_t file_size = recomp::util::file_size_in_bytes(file_name);
        if (prefix > 0) {
            file_size = std::min(file_size, prefix);
        }

        lce::lceDataStructure prezza;
        lce::buildLCEDataStructure(&prezza, file_name);

        recomp::rlslp<recomp::var_t> rlslp;
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
                typedef recomp::parallel::parallel_lp_recompression<recomp::var_t>::text_t text_t;
                text_t text;
                recomp::util::read_file(file_name, text, prefix);

                recomp::parallel::parallel_lp_recompression<recomp::var_t> recompression;
                recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
            }
            std::cout << "Loaded" << std::endl;
        }
        if (coder.empty()) {
            typedef recomp::parallel::parallel_lp_recompression<recomp::var_t>::text_t text_t;
            text_t text;
            recomp::util::read_file(file_name, text, prefix);

            recomp::parallel::parallel_lp_recompression<recomp::var_t> recompression;
            recompression.recomp(text, rlslp, recomp::CHAR_ALPHABET, 4);
        }

        std::string plain_text;
        recomp::util::read_text_file(file_name, plain_text, prefix);

        sdsl::lcp_dac<> lcp;
        sdsl::construct(lcp, file_name, 1);

        sdsl::csa_sada<> sa;
        sdsl::construct(sa, file_name, 1);

        sdsl::rmq_succinct_sct<> rmq(&lcp);

        for (size_t accesses = begin; accesses <= end; accesses += steps) {
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
