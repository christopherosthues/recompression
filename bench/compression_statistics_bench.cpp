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
    cmd.set_author("Christopher Osthues");

    std::string path;
    cmd.add_param_string("path", path, "The path to the directory containing the files");

    std::string filenames;
    cmd.add_param_string("filenames", filenames,
                         "The files. Multiple files are separated with spaces and are enclosed by \"\". Example: \"file1 file2 file3\"");

    std::string coder_path;
    cmd.add_param_string("coder_path", coder_path,
                         "The path to the files");

    std::string coders_str;
    cmd.add_param_string("coders", coders_str, "The coders [\"plain | fixed | sorted | bzip2-1 | bzip2-9 | gzip-1 | gzip-9\"]");

    if (!cmd.process(argc, argv)) {
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(filenames, " ", files);

    std::vector<std::string> coders;
    recomp::util::split(coders_str, " ", coders);

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
                    coder_file = coder_path + file_name + extension;
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
