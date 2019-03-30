#pragma once

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {
namespace coder {

class Encoder {
 public:
    template<typename variable_t = var_t, typename terminal_count_t = term_t>
    inline void encode(rlslp<variable_t, terminal_count_t>& rlslp) {}
};

class Decoder {
 public:
    template<typename variable_t = var_t, typename terminal_count_t = term_t>
    inline rlslp<variable_t, terminal_count_t> decode() {}
};

}  // namespace coder
}  // namespace recomp
