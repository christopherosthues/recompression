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

int str_to_int(std::string s) {
    std::istringstream ss(s);
    int n;
    if (!(ss >> n)) {
        std::cerr << "Invalid number: " << s;
        return -1;
    } else if (!ss.eof()) {
        std::cerr << "Trailing characters after number: " << s;
        return -1;
    }
    return n;
}

void replace_all(std::string& s, std::string replace, std::string replace_with) {
    size_t pos = s.find(replace);

    while(pos != std::string::npos) {
        s.replace(pos, replace.size(), replace_with);
        pos = s.find(replace, pos + replace_with.size());
    }
}

void split(std::string s, std::string delimiter, std::vector<std::string>& split) {
    size_t pos = 0, end;
    size_t delim_len = delimiter.size();

    while ((end = s.find(delimiter, pos)) != std::string::npos) {
        split.push_back(s.substr(pos, end - pos));
        pos = end + delim_len;
    }

    split.push_back(s.substr(pos));
}


int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cerr << "./recompression_all_bench [path] [file_name(s)] [sequential | parallel | full_parallel | parallel_ls | parallel_gr | fast | hash] [cores] [repeats]" << std::endl;
        return -1;
    }

    int cores = str_to_int(argv[4]);
    std::cout << "Using " << cores << " threads" << std::endl;
    if (cores <= 0) {
        return -1;
    }

    int repeats = str_to_int(argv[5]);
    std::cout << "Using " << repeats << " repeats" << std::endl;
    if (repeats <= 0) {
        return -1;
    }

    std::vector<std::string> files;
    split(argv[2], " ", files);

    std::vector<std::string> algos;
    split(argv[3], " ", algos);

    for (size_t j = 0; j < files.size(); ++j) {
        for (size_t i = 0; i < algos.size(); ++i) {
            for (size_t repeat = 0; repeat < repeats; ++repeat) {
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

                replace_all(dataset, "_", "\\_");

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

                const auto startTime = std::chrono::system_clock::now();

                recomp->recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
                const auto endTime = std::chrono::system_clock::now();
                const auto timeSpan = endTime - startTime;
                std::cout << "Time for " << algo << " recompression: "
                          << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
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
    }
}
