
#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"

namespace recomp {
namespace util {

//void read_file(const std::string& file_name, text_t& text);

template<typename text_t>
std::string text_vector_to_string(const text_t& text) {
    std::stringstream text_stream;
    for (const auto& c : text) {
        text_stream << c << " ";
    }
    return text_stream.str();
}

template<typename block_t, typename variable_t>
std::string blocks_to_string(const std::unordered_map<block_t, variable_t, pair_hash>& blocks) {
    std::stringstream block_stream;
    for (const auto& block : blocks) {
        block_stream << "(" << block.first.first << "," << block.first.second << ") ";
    }
    return block_stream.str();
}

template<typename block_t>
std::string vector_blocks_to_string(const std::vector<block_t>& sort_blocks) {
    std::stringstream stream;
    for (const auto& block : sort_blocks) {
        stream << "(" << block.first << "," << block.second << ") ";
    }
    return stream.str();
}

template<typename position_t>
std::string positions_to_string(const std::vector<position_t>& positions) {
    std::stringstream pos_stream;
    for (const auto& pos : positions) {
        pos_stream << pos.second << " ";
    }
    return pos_stream.str();
}

}  // namespace util
}  // namespace recomp
