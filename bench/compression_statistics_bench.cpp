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
    if (argc < 5) {
        std::cerr << "./bench_compression_statistics [path] [file_name(s)] [enc_path] [plain | wlz | sorted | bzip2-1 | bzip2-9 | gzip-1 | gzip-9]"
                  << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    std::vector<std::string> coders;
    recomp::util::split(argv[4], " ", coders);

    std::string path(argv[1]);
    std::string coder_path(argv[3]);

    for (size_t j = 0; j < files.size(); ++j) {
        for (const auto& coder : coders) {
            std::cout << "Using coder " << coder << std::endl;

            std::string file_name = files[j];

            size_t pos = file_name.find_last_of('/');
            std::string dataset;
            if (pos != std::string::npos) {
                dataset = file_name.substr(pos + 1);
            } else {
                dataset = file_name;
            }

            recomp::util::replace_all(dataset, "_", "\\_");

            std::string coder_file = coder_path;
            if (coder == "plain") {
                coder_file += file_name + ".plain";
            } else if (coder == "wlz") {
                coder_file += file_name + ".wlz";
            } else if (coder == "sorted") {
                std::cout << "not supported jet" << std::endl;
            } else if (coder == "gzip-1") {
                coder_file += file_name + ".1.gz";
            } else if (coder == "gzip-9") {
                coder_file += file_name + ".9.gz";
            } else if (coder == "bzip2-1") {
                coder_file += file_name + ".1.bz2";
            } else if (coder == "bzip2-9") {
                coder_file += file_name + ".9.bz2";
            } else {
                std::cout << "Unkown coder '" << coder << "'." << std::endl;
            }

            std::ifstream in_enc(coder_file, std::ios::binary | std::ios::ate);
            std::ifstream in(path + file_name, std::ios::binary | std::ios::ate);
            std::cout << "RESULT dataset=" << dataset << " coder=" << coder << " size=" << in.tellg() << " enc_size="
                      << in_enc.tellg() << std::endl;
            in.close();
            in_enc.close();
        }
    }
}
