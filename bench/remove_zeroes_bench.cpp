#include <algorithm>
#include <iostream>
#include <vector>

#include "recompression.hpp"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "./bench_lce_query [path] [file_name(s)] [prefix]" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    size_t prefix = 0;
    if (argc > 3) {
        prefix = (size_t)recomp::util::str_to_int(argv[3]);
    }

    for (size_t k = 0; k < files.size(); ++k) {
        std::string file_name = argv[1] + files[k];

        std::string text;
        recomp::util::read_text_file_without_zeroes(file_name, text, prefix);

        std::ofstream ostream(file_name + "_wz");
        ostream << text;
        ostream.close();
    }

    return 0;
}
