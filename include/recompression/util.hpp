
#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "defs.hpp"

namespace recomp {

namespace util {

size_t file_size_in_bytes(std::string& file_name, const size_t prefix_size = 0) {
    std::ifstream in(file_name, std::ios::binary | std::ios::ate);
    size_t size = in.tellg();
    if (prefix_size > 0) {
        size = std::min(size, prefix_size);
    }
    in.close();
    return size;
}

/**
 * @brief Generates (pseudo) random number from 0 (inclusive) to max (exclusive).
 *
 * @param max_val The maximum value (exclusive)
 * @return A random number
 */
inline size_t random_number(size_t max_val) {
    return ((((size_t)std::rand()) << 32) + std::rand()) % max_val;
}

/**
 * @brief Faster loading of a file but with more memory consumption.
 *
 * @tparam text_t The type of text
 * @param[in] file_name The file name
 * @param[out] text The read text
 */
template<typename text_t>
void read_file_fast(const std::string& file_name, ui_vector<text_t>& text, const size_t prefix_size = 0) {
    std::cout << "Reading file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::end);
    uint64_t file_size = ifs.tellg();
    if (prefix_size > 0) {
        file_size = std::min(file_size, prefix_size);
    }

    ifs.seekg(0, std::ios::beg);
    text.resize(file_size);
//    ifs.read(reinterpret_cast<char*>(text.data()), file_size);
    std::vector<char> in(file_size);
    ifs.read(reinterpret_cast<char*>(in.data()), file_size);
    for (size_t i = 0; i < in.size(); ++i) {
        text[i] = static_cast<text_t>(in[i]);
    }
    ifs.close();
    std::cout << "Read " << file_size << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Reads in a file to the specified data.
 *
 * @tparam text_t The type of text
 * @param file_name The file name
 * @param text[out] The read text
 */
template<typename text_t>
void read_file_fill(const std::string& file_name, ui_vector<text_t>& text, const size_t prefix_size) {
    std::cout << "Reading file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::beg);

    text.resize(prefix_size);

    char c;
    size_t index = 0;
    while (ifs.get(c) && index < prefix_size) {
        text[index++] = static_cast<text_t>((unsigned char)c);
    }
    ifs.close();
    
    size_t idx = 0;
    while (index < prefix_size) {
        text[index++] = text[idx++];
    }

    std::cout << "Read " << prefix_size << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Reads in a file to the specified data.
 *
 * @tparam text_t The type of text
 * @param file_name The file name
 * @param text[out] The read text
 */
template<typename text_t>
void read_file(const std::string& file_name, ui_vector<text_t>& text, const size_t prefix_size = 0) {
    std::cout << "Reading file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    uint64_t file_size = ifs.tellg();
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::beg);

    if (prefix_size > 0) {
        file_size = std::min(file_size, prefix_size);
    }

    text.resize(file_size);

    char c;
    size_t index = 0;
    while (ifs.get(c) && index < file_size) {
        text[index++] = static_cast<text_t>((unsigned char)c);
    }
    ifs.close();

    std::cout << "Read " << file_size << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Reads in a file to the specified data.
 *
 * @tparam text_t The type of text
 * @param file_name The file name
 * @param text[out] The read text
 */
template<typename text_t>
void read_file_without_zeroes(const std::string& file_name, ui_vector<text_t>& text, const size_t prefix_size = 0) {
    std::cout << "Reading file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    uint64_t file_size = ifs.tellg();
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::beg);

    if (prefix_size > 0) {
        file_size = std::min(file_size, prefix_size);
    }

    text.resize(file_size);

    char c;
    size_t index = 0;
    size_t idx = 0;
    while (ifs.get(c) && index < file_size) {
        if (static_cast<text_t>((unsigned char)c) != 0) {
            text[idx++] = static_cast<text_t>((unsigned char)c);
        }
        index++;
    }
    ifs.close();
    text.resize(idx);

    std::cout << "Read " << text.size() << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Reads a file to a string.
 *
 * @param file_name The file name
 * @param text[out] The read text
 */
void read_text_file(const std::string& file_name, std::string& text, const size_t prefix_size = 0) {
    std::cout << "Reading text file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    uint64_t file_size = ifs.tellg();
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::beg);

    if (prefix_size > 0) {
        file_size = std::min(file_size, prefix_size);
    }

    text.resize(file_size, '\0');
    ifs.read((char*)text.data(), file_size);
    ifs.close();

    std::cout << "Read " << file_size << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Reads a file to a string.
 *
 * @param file_name The file name
 * @param text[out] The read text
 */
void read_text_file_fill(const std::string& file_name, std::string& text, size_t prefix_size = 0) {
    std::cout << "Reading text file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    uint64_t file_size = ifs.tellg();
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::beg);

    if (prefix_size > 0) {
        file_size = std::min(file_size, prefix_size);
    }

    text.resize(prefix_size, '\0');
    ifs.read((char*)text.data(), file_size);
    ifs.close();
    
    size_t index = file_size;
    size_t idx = 0;
    while (index < prefix_size) {
        text[index++] = text[idx++];
    }

    std::cout << "Read " << prefix_size << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Reads a file to a string.
 *
 * @param file_name The file name
 * @param text[out] The read text
 */
void read_text_file_without_zeroes(const std::string& file_name, std::string& text, const size_t prefix_size = 0) {
    std::cout << "Reading text file: " << file_name << std::endl;
    std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    uint64_t file_size = ifs.tellg();
    if (!ifs) {
        std::cerr << "Failed to read file " << file_name << std::endl;
        exit(1);
    }
    ifs.seekg(0, std::ios::beg);

    if (prefix_size > 0) {
        file_size = std::min(file_size, prefix_size);
    }

    text.resize(file_size, '\0');
    ifs.read((char*)text.data(), file_size);
    ifs.close();
    size_t j = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] != 0) {
            text[j++] = text[i];
        }
    }
    text.resize(j);
    text.shrink_to_fit();
    std::cout << "Read " << text.size() << " bytes" << std::endl;
    std::cout << "Finished reading file" << std::endl;
}

/**
 * @brief Returns a string representation of the text.
 *
 * @tparam text_t The type of text
 * @param text The text
 * @return The string representation of the text
 */
template<typename text_t>
std::string text_vector_to_string(const std::vector<text_t>& text) {
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
    std::map<var_t, bool> part;
    for (const auto& par : partition) {
        part[par.first] = par.second;
    }
    for (const auto& par : part) {
        sstream << "(" << par.first << ": " << par.second << ") " << std::endl;
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

void replace_all(std::string& s, std::string replace, std::string replace_with) {
    size_t pos = s.find(replace);

    while(pos != std::string::npos) {
        s.replace(pos, replace.size(), replace_with);
        pos = s.find(replace, pos + replace_with.size());
    }
}

void split(std::string s, std::string delimiter, std::vector<std::string>& split) {
    size_t pos = 0, end;
    size_t delim_len = delimiter.size();

    while ((end = s.find(delimiter, pos)) != std::string::npos) {
        split.push_back(s.substr(pos, end - pos));
        pos = end + delim_len;
    }

    split.push_back(s.substr(pos));
}

void file_name_without_path(std::string& file_name) {
    size_t pos = file_name.find_last_of('/');
    if (pos != std::string::npos) {
        file_name = file_name.substr(pos + 1);
    }
}

void file_name_without_extension(std::string& file_name) {
    size_t pos = file_name.find_last_of('.');
    if (pos != std::string::npos) {
        file_name = file_name.substr(0, pos);
    }
}

int str_to_int(std::string s) {
    std::istringstream ss(s);
    int n;
    if (!(ss >> n)) {
        std::cerr << "Invalid number: " << s;
        return -1;
    } else if (!ss.eof()) {
        std::cerr << "Trailing characters after number: " << s;
        return -1;
    }
    return n;
}

void check_graph(std::string file_name) {
    std::ifstream in_file(file_name, std::ifstream::in | std::ios::binary | std::ifstream::ate);
    std::string part_str;
    in_file.seekg(0, std::ios::beg);

    std::unordered_map<int, std::unordered_map<int, int>> adj;
    std::getline(in_file, part_str);

    int v = 1;
    while (std::getline(in_file, part_str)) {
        std::vector<std::string> split;
        util::split(part_str, " ", split);
        for (size_t i = 0; i < split.size(); i += 2) {
            auto u = str_to_int(split[i]);
            auto wgt = str_to_int(split[i + 1]);
            adj[v][u+1] = wgt;
        }
        v++;
    }

    for (const auto& ad : adj) {
        for (const auto& a : ad.second) {
            if (adj[a.first][ad.first] != a.second) {
                std::cout << "Graph not correct" << std::endl;
            }
        }
    }
    in_file.close();
}

template<typename T>
inline ui_vector<T> create_ui_vector(std::vector<T> vec) {
    ui_vector<T> ui_vec(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        ui_vec[i] = vec[i];
    }
    return ui_vec;
}

inline constexpr uint8_t bits_for(size_t value) {
    return value == 0 ? (uint8_t)1 : 64 - __builtin_clzll(value);
}

}  // namespace util
}  // namespace recomp
