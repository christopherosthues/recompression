#include <algorithm>
#include <iostream>
#include <vector>

#include <tlx/cmdline_parser.hpp>

#include "recompression.hpp"


int main(int argc, char *argv[]) {
    tlx::CmdlineParser cmd;
    cmd.set_description("Reads the given files, removes the zero symbol and writes it to <filename>_wz.");
    cmd.set_author("Christopher Osthues <osthues.christopher@web.de>");

    std::string path;
    cmd.add_param_string("path", path, "The path to the directory containing the files");

    std::string filenames;
    cmd.add_param_string("filenames", filenames,
                         "The files. Multiple files are seperated with spaces and are enclosed by \"\". Example: \"file1 file2 file3\"");

    size_t prefix = 0;
    cmd.add_bytes('p', "prefix", prefix, "The prefix of the files in bytes to read in");

    if (!cmd.process(argc, argv)) {
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(filenames, " ", files);

    for (size_t k = 0; k < files.size(); ++k) {
        std::string file_name = path + files[k];

        std::string text;
        recomp::util::read_text_file_without_zeroes(file_name, text, prefix);

        std::ofstream ostream(file_name + "_wz");
        ostream << text;
        ostream.close();
    }

    return 0;
}
