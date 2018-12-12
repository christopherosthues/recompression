
#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "parallel_recompression.hpp"
#include "rlslp.hpp"

namespace recomp {
namespace util {

void read_file(const std::string& file_name, text_t& text);

std::string text_vector_to_string(const text_t& text);

std::string blocks_to_string(const std::unordered_map<block_t, variable_t, pair_hash>& blocks);

std::string vector_blocks_to_string(const std::vector<block_t>& sort_blocks);

std::string positions_to_string(const std::vector<position_t>& positions);

}  // namespace util
}  // namespace recomp
