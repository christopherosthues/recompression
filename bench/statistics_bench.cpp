
#include <string>
#include <unordered_set>

#include "defs.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "./bench_statistics [path] [file(s)]" << std::endl;
        return -1;
    }

    std::vector<std::string> files;
    recomp::util::split(argv[2], " ", files);

    for (size_t i = 0; i < files.size(); ++i) {
        std::string file_name(argv[1]);
        file_name += files[i];

        std::string text;
        recomp::util::read_text_file(file_name, text);
        
        std::unordered_set<char> alphabet;

        size_t l = 0, r = 0, e = 0;
        for (size_t j = 0; j < text.size() - 1; ++j) {
            alphabet.insert(text[j]);
            if (text[j] < text[j + 1]) {
                l++;
            } else if (text[j] > text[j + 1]) {
                r++;
            } else {
                e++;
            }
        }
        if (text.size() > 0) {
            alphabet.insert(text[text.size() - 1]);
        }

        size_t pos = file_name.find_last_of('/');
        std::string dataset;
        if (pos != std::string::npos) {
            dataset = file_name.substr(pos + 1);
        } else {
            dataset = file_name;
        }

        recomp::util::replace_all(dataset, "_", "\\_");

        std::cout << "RESULT dataset=" << dataset << " size=" << text.size() << " alphabet=" << alphabet.size()
                  << " l=" << l << " r=" << r << " e=" << e << std::endl;
    }

    return 0;
}
