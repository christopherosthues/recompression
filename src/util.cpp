
//#include <fstream>
//
//#include <glog/logging.h>

#include "util.hpp"

//void recomp::util::read_file(const std::string& file_name, recomp::text_t& text) {
//    LOG(INFO) << "Reading file: " << file_name;
//    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
//    auto file_size = ifs.tellg();
//    if (file_size < 0) {
//        return;
//    }
//    ifs.seekg(0, std::ios::beg);
//
//    text.resize(static_cast<size_t>(file_size), '\0');
//    std::vector<char> in(file_size);
//    ifs.read((char*)in.data(), file_size);
//    for (size_t i = 0; i < in.size(); ++i) {
//        text[i] = static_cast<variable_t>(in[i]);
//    }
//    ifs.close();
//    LOG(INFO) << "Read " << file_size << " bytes";
//    LOG(INFO) << "Finished reading file";
//}
