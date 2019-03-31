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
    cmd.set_description("Benchmark for runtime strong scaling experiments");
    cmd.set_author("Christopher Osthues <osthues.christopher@web.de>");

    std::string path;
    cmd.add_param_string("path", path, "The path to the directory containing the files");

    std::string filenames;
    cmd.add_param_string("filenames", filenames,
                         "The files. Multiple files are seperated with spaces and are enclosed by \"\". Example: \"file1 file2 file3\"");

    std::string algorithms;
    cmd.add_param_string("algorithms", algorithms,
                         "The algorithms to benchmark [\"parallel | full_parallel | parallel_lock | parallel_lp | parallel_ls | parallel_gr | parallel_rnd\"]");

    size_t cores;
    cmd.add_param_bytes("cores", cores, "The maximal number of cores");

    size_t repeats;
    cmd.add_param_bytes("repeats", repeats, "The number of repeats to process");

    size_t begin;
    cmd.add_param_bytes("begin", begin, "The number of cores to use at the beginning");

    size_t steps;
    cmd.add_param_bytes("steps", steps, "The steps");

    size_t file_size;
    cmd.add_param_bytes("file_size", file_size, "The file size to begin with");

    if (!cmd.process(argc, argv)) {
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(filenames, " ", files);

    std::vector<std::string> algos;
    recomp::util::split(algorithms, " ", algos);

    for (size_t j = 0; j < files.size(); ++j) {
        for (size_t step = 1; step <= cores; step *= 2) {
            for (size_t repeat = 0; repeat < repeats; ++repeat) {
                for (size_t i = 0; i < algos.size(); ++i) {
                    std::cout << "Iteration: " << repeat << std::endl;
                    std::string algo = algos[i];
                    std::cout << "Using algo " << algo << std::endl;
                    std::cout << "Using " << step << " cores" << std::endl;

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

                    std::unique_ptr<recomp::recompression<recomp::var_t, recomp::term_t>> recomp = recomp::create_recompression(algo, dataset);
                    if (!recomp) {
                        std::cerr << "No such algo " << algo << std::endl;
                        return -1;
                    }

                    typedef recomp::recompression<recomp::var_t, recomp::term_t>::text_t text_t;
                    text_t text;
                    recomp::util::read_file_fill(file_name, text, file_size);

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
                    recomp::util::read_text_file_fill(file_name, c_text, file_size);
                    if (res == c_text) {
                        std::cout << "Correct" << std::endl;
                    } else {
                        std::cout << "Failure" << std::endl;
                    }
                }
            }
            file_size *= 2;
        }
    }
}
