
#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

#include <tlx/simple_vector.hpp>

namespace recomp {

using timer = std::chrono::steady_clock;

template<typename T>
using ui_vector = tlx::SimpleVector<T, tlx::SimpleVectorMode::NoInitButDestroy>;

template<typename T>
using i_vector = tlx::SimpleVector<T, tlx::SimpleVectorMode::Normal>;

typedef std::uint32_t var_t;
typedef std::uint32_t term_t;

struct pair_hash {
    template<typename P1, typename P2>
    size_t operator()(const std::pair<P1, P2>& pair) const {
        std::hash<P1> h1;  // (pair.first);
        std::hash<P2> h2;  // (pair.second);
        return h1(pair.first) ^ (h2(pair.second) << 1);
    }
};

}  // namespace recomp