
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

//std::string recomp::util::text_vector_to_string(const recomp::text_t& text) {
//    std::stringstream text_stream;
//    for (const auto& c : text) {
//        text_stream << c << " ";
//    }
//    return text_stream.str();
//}
//
//std::string recomp::util::blocks_to_string(const std::unordered_map<recomp::block_t, recomp::variable_t, recomp::pair_hash>& blocks) {
//    std::stringstream block_stream;
//    for (const auto& block : blocks) {
//        block_stream << "(" << block.first.first << "," << block.first.second << ") ";
//    }
//    return block_stream.str();
//}
//
//std::string recomp::util::vector_blocks_to_string(const std::vector<recomp::block_t>& sort_blocks) {
//    std::stringstream stream;
//    for (const auto& block : sort_blocks) {
//        stream << "(" << block.first << "," << block.second << ") ";
//    }
//    return stream.str();
//}
//
//std::string recomp::util::positions_to_string(const std::vector<recomp::position_t>& positions) {
//    std::stringstream pos_stream;
//    for (const auto& pos : positions) {
//        pos_stream << pos.second << " ";
//    }
//    return pos_stream.str();
//}
