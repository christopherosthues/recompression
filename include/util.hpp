
#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"

namespace recomp {
namespace util {

/**
 * @brief Faster loading of a file but with more memory consumption.
 *
 * @tparam text_t The type of text
 * @param[in] file_name The file name
 * @param[out] text The read text
 */
template<typename text_t>
void read_file_fast(const std::string& file_name, text_t& text) {
    LOG(INFO) << "Reading file: " << file_name;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    auto file_size = ifs.tellg();
    if (file_size < 0) {
        LOG(ERROR) << "Failed to read file " << file_name;
        return;
    }
    ifs.seekg(0, std::ios::beg);

    text.resize(static_cast<size_t>(file_size), '\0');
    std::vector<char> in(file_size);
    ifs.read(reinterpret_cast<char*>(in.data()), file_size);
    for (size_t i = 0; i < in.size(); ++i) {
        text[i] = static_cast<typename text_t::value_type>(in[i]);
    }
    ifs.close();
    LOG(INFO) << "Read " << file_size << " bytes";
    LOG(INFO) << "Finished reading file";
}

/**
 * @brief Reads in a file to the specified data.
 *
 * @tparam text_t The type of text
 * @param file_name The file name
 * @param text[out] The read text
 */
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

    text.resize(static_cast<size_t>(file_size), 0);

    char c;
    auto index = 0;
    while (ifs.get(c)) {
        text[index++] = static_cast<typename text_t::value_type>((unsigned char)c);
    }
    ifs.close();

    LOG(INFO) << "Read " << file_size << " bytes";
    LOG(INFO) << "Finished reading file";
}

/**
 * @brief Reads a file to a string.
 *
 * @param file_name The file name
 * @param text[out] The read text
 */
void read_text_file(const std::string& file_name, std::string& text) {
    LOG(INFO) << "Reading file: " << file_name;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    auto file_size = ifs.tellg();
    if (file_size < 0) {
        LOG(ERROR) << "Failed to read file " << file_name;
        return;
    }
    ifs.seekg(0, std::ios::beg);

    // std::vector<char> bytes(((size_t)file_size)+1);
    text.resize(static_cast<size_t>(file_size), '\0');
    ifs.read((char*)text.data(), file_size);
    // bytes.data()[file_size] = '$';
    ifs.close();
    LOG(INFO) << "Read " << file_size << " bytes";
    LOG(INFO) << "Finished reading file";
    // return std::string(bytes.data(), file_size);
//    std::ifstream ifs(file_name.c_str());
//    std::istream_iterator<char> input(ifs);
//    std::copy(input, std::istream_iterator<char>(), std::back_inserter(text));
}

/**
 * @brief Returns a string representation of the text.
 *
 * @tparam text_t The type of text
 * @param text The text
 * @return The string representation of the text
 */
template<typename text_t>
std::string text_vector_to_string(const text_t& text) {
    std::stringstream text_stream;
    for (const auto& c : text) {
        text_stream << c << " ";
    }
    return text_stream.str();
}

/**
 * @brief Returns a string representation an unordered map of blocks.
 *
 * @tparam block_t The type of blocks
 * @tparam variable_t The type of non-terminals
 * @param blocks The blocks
 * @return The string representation
 */
template<typename block_t, typename variable_t>
std::string blocks_to_string(const std::unordered_map<block_t, variable_t, pair_hash>& blocks) {
    std::stringstream block_stream;
    for (const auto& block : blocks) {
        block_stream << "(" << block.first.first << ", " << block.first.second << ") ";
    }
    return block_stream.str();
}

/**
 * @brief Returns a string representation a vector of blocks.
 *
 * @tparam block_t The type of blocks
 * @param blocks The blocks
 * @return The string representation
 */
template<typename block_t>
std::string vector_blocks_to_string(const std::vector<block_t>& blocks) {
    std::stringstream stream;
    for (const auto& block : blocks) {
        stream << "(" << block.first << ", " << block.second << ") ";
    }
    return stream.str();
}

/**
 * @brief Returns a string representation of the positions vector of blocks.
 *
 * @tparam position_t The type of block positions
 * @param positions A vector of block positions
 * @return The string representation
 */
template<typename position_t>
std::string block_positions_to_string(const std::vector<position_t>& positions) {
    std::stringstream pos_stream;
    for (const auto& pos : positions) {
        pos_stream << pos.second << " ";
    }
    return pos_stream.str();
}

/**
 * @brief Returns a string representation of the positions vector of pairs.
 *
 * @tparam pair_position_t The type of pair positions
 * @param positions A vector of pair positions
 * @return The string representation
 */
template<typename pair_position_t>
std::string pair_positions_to_string(const std::vector<pair_position_t>& positions) {
    std::stringstream pos_stream;
    for (const auto& pos : positions) {
        pos_stream << pos << " ";
    }
    return pos_stream.str();
}

/**
 * @brief Returns a string representation of a partition.
 *
 * @tparam partition_t The type of partition
 * @param partition The partition
 * @return The string representation
 */
template<typename partition_t>
std::string partition_to_string(const partition_t& partition) {
    std::stringstream sstream;
    for (const auto& par : partition) {
        sstream << "(" << par.first << ": " << par.second << ") ";
    }
    return sstream.str();
}

/**
 * @brief Returns a string representation of a multiset.
 *
 * @tparam multiset_t The type of multiset
 * @param multiset The multiset
 * @return The string representation
 */
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

/**
 * @brief Checks whether the elements in the given vector are sorted increasingly.
 *
 * @tparam vector_t The type of vector
 * @param vector The vector
 * @return @code{True} if the vector is sorted, otherwise @code{false}
 */
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
