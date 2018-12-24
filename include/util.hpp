
#pragma once

#include <array>
#include <fstream>
#include <sstream>
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
        block_stream << "(" << block.first.first << ", " << block.first.second << ") ";
    }
    return block_stream.str();
}

template<typename block_t>
std::string vector_blocks_to_string(const std::vector<block_t>& sort_blocks) {
    std::stringstream stream;
    for (const auto& block : sort_blocks) {
        stream << "(" << block.first << ", " << block.second << ") ";
    }
    return stream.str();
}

template<typename position_t>
std::string block_positions_to_string(const std::vector<position_t>& positions) {
    std::stringstream pos_stream;
    for (const auto& pos : positions) {
        pos_stream << pos.second << " ";
    }
    return pos_stream.str();
}

template<typename pair_position_t>
std::string pair_positions_to_string(const std::vector<pair_position_t>& positions) {
    std::stringstream pos_stream;
    for (const auto& pos : positions) {
        pos_stream << pos << " ";
    }
    return pos_stream.str();
}

template<typename partition_t>
std::string partition_to_string(const partition_t& partition) {
    std::stringstream sstream;
    for (const auto& par : partition) {
        sstream << "(" << par.first << ": " << par.second << ") ";
    }
    return sstream.str();
}

template<typename multiset_t>
std::string multiset_to_string(const multiset_t& multiset) {
    std::stringstream sstream;
    for (const auto& tup : multiset) {
        sstream << "(" << std::get<0>(tup) << "," << std::get<1>(tup) << "," << std::get<2>(tup) << ") ";
    }
    return sstream.str();
}

template<size_t n>
std::string array_to_string(const std::array<size_t, n>& positions) {
    std::stringstream sstream;
    for (const auto& pos : positions) {
        sstream << pos << " ";
    }
    return sstream.str();
}

template<typename vector_t>
bool is_sorted(std::vector<vector_t> vector) {
    for (size_t i = 1; i < vector.size(); ++i) {
        if (vector[i-1] > vector[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace util
}  // namespace recomp
