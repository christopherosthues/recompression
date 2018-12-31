
#pragma once

#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"

namespace recomp {
namespace util {

template<typename text_t>
std::string text_vector_to_string(const text_t& text) {
    std::stringstream text_stream;
    for (const auto& c : text) {
        text_stream << c << " ";
    }
    return text_stream.str();
}

/**
 * @brief Faster loading of a file but with more memory consumption.
 * @tparam text_t The type of text
 * @param[in] file_name The file name
 * @param[out] text The text to be read in
 */
template<typename text_t>
void read_file_fast(const std::string& file_name, text_t& text) {
    LOG(INFO) << "Reading file: " << file_name;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    auto file_size = ifs.tellg();
    if (file_size < 0) {
        return;
    }
    ifs.seekg(0, std::ios::beg);

    text.resize(static_cast<size_t>(file_size), '\0');
    std::vector<char> in(file_size);
    ifs.read((char*)in.data(), file_size);
    for (size_t i = 0; i < in.size(); ++i) {
        text[i] = static_cast<typename text_t::value_type>(in[i]);
    }
    ifs.close();
    LOG(INFO) << "Read " << file_size << " bytes";
    LOG(INFO) << "Finished reading file";
//    LOG(INFO) << "Text fast: " << text_vector_to_string(text);
}

template<typename text_t>
void read_file(const std::string& file_name, text_t& text) {
    LOG(INFO) << "Reading file: " << file_name;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    auto file_size = ifs.tellg();
    if (file_size < 0) {
        LOG(ERROR) << "Failed to read file " << file_name;
        return;
    }
    ifs.seekg(0, std::ios::beg);

    text.resize(static_cast<size_t>(file_size), '\0');

    char c;
    auto index = 0;
    while (ifs.get(c)) {
        text[index++] = static_cast<typename text_t::value_type>(c);
    }
    ifs.close();

    LOG(INFO) << "Read " << file_size << " bytes";
    LOG(INFO) << "Finished reading file";
//    LOG(INFO) << "Text: " << text_vector_to_string(text);

//    std::ifstream ifs(file_name.c_str());
//    std::istream_iterator<recomp::var_t> input(ifs);
//    std::copy(input, std::istream_iterator<recomp::var_t>(), std::back_inserter(text));
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
