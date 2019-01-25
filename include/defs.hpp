
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

namespace recomp {

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
