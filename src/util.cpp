
#include "util.hpp"

std::string recomp::util::text_vector_to_string(const recomp::text_t& text) {
    std::stringstream text_stream;
    for (const auto& c : text) {
        text_stream << c << " ";
    }
    return text_stream.str();
}

std::string recomp::util::blocks_to_string(const std::unordered_map<std::pair<recomp::variable_t, recomp::variable_t>, recomp::variable_t, recomp::pair_hash>& blocks) {
    std::stringstream block_stream;
    for (const auto& block : blocks) {
        block_stream << "(" << block.first.first << "," << block.first.second << ") ";
    }
    return block_stream.str();
}

std::string recomp::util::vector_blocks_to_string(const std::vector<std::pair<recomp::variable_t, recomp::variable_t>>& sort_blocks) {
    std::stringstream stream;
    for (const auto& block : sort_blocks) {
        stream << "(" << block.first << "," << block.second << ") ";
    }
    return stream.str();
}

std::string recomp::util::positions_to_string(const std::vector<std::pair<recomp::variable_t, size_t>>& positions) {
    std::stringstream pos_stream;
    for (const auto& pos : positions) {
        pos_stream << pos.second << " ";
    }
    return pos_stream.str();
}
