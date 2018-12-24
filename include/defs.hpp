
#pragma once

#include <array>
#include <functional>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace recomp {

typedef std::int32_t var_t;
typedef std::uint32_t term_t;
//typedef std::int32_t variable_t;
//typedef std::uint32_t terminal_count_t;
//
//typedef std::vector<variable_t> text_t;
//typedef std::vector<variable_t> alphabet_t;
////typedef std::array<std::vector<std::pair<std::pair<variable_t, variable_t>, size_t>>, 2> multiset_t;
//typedef std::vector<std::tuple<variable_t, variable_t, bool>> multiset_t;
//typedef std::unordered_map<variable_t, bool> partition_t;
//
//typedef std::pair<variable_t, variable_t> block_t;
//typedef block_t pair_t;
//typedef std::pair<variable_t, size_t> position_t;
//typedef size_t pair_position_t;

struct pair_hash {
    template<typename P1, typename P2>
    size_t operator()(const std::pair<P1, P2>& pair) const {
        std::hash<P1> h1;  // (pair.first);
        std::hash<P2> h2;  // (pair.second);
        return h1(pair.first) ^ (h2(pair.second) << 1);
    }
};

}  // namespace recomp
