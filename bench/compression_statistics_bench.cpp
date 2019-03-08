#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "recompression.hpp"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "./bench_compression_statistics [path] [file_name(s)] [enc_path] [plain | plain_fixed | sorted | sorted_dr | bzip2-1 | bzip2-9 | gzip-1 | gzip-9]"
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
//            if (coder == "plain") {
//                coder_file += file_name + recomp::coder::PlainRLSLPCoder::k_extension;
//            } else if (coder == "wlz") {
//                coder_file += file_name + recomp::coder::PlainFixedRLSLPCoder::k_extension;
//            } else if (coder == "sorted") {
//                coder_file += file_name + recomp::coder::SortedRLSLPCoder::k_extension;
//            } else if (coder == "sorted_dr") {
//                coder_file += file_name + recomp::coder::SortedRLSLPDRCoder::k_extension;
//            } else
            if (coder == "gzip-1") {
                coder_file += file_name + ".1.gz";
            } else if (coder == "gzip-9") {
                coder_file += file_name + ".9.gz";
            } else if (coder == "bzip2-1") {
                coder_file += file_name + ".1.bz2";
            } else if (coder == "bzip2-9") {
                coder_file += file_name + ".9.bz2";
            } else {
                std::string extension = recomp::coder::get_coder_extension(coder);
                if (extension == "unknown") {
                    std::cout << "Unkown coder '" << coder << "'." << std::endl;
                    return -1;
                } else {
                    coder_file = file_name + extension;
                }
            }

            std::ifstream in_enc(coder_file, std::ios::binary | std::ios::ate);
            std::ifstream in(path + file_name, std::ios::binary | std::ios::ate);

            std::string out_coder = coder;
            recomp::util::replace_all(out_coder, "-", "_");

            std::cout << "RESULT dataset=" << dataset << " coder=" << out_coder << " size=" << in.tellg() << " enc_size="
                      << in_enc.tellg() << std::endl;
            in.close();
            in_enc.close();
        }
    }
}
