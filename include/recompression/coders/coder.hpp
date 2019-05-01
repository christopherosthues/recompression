#pragma once

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {
namespace coder {

class Encoder {
 public:
    template<typename variable_t = var_t>
    inline void encode(rlslp<variable_t>& rlslp) {}
};

class Decoder {
 public:
    template<typename variable_t = var_t>
    inline rlslp<variable_t> decode() {}
};

}  // namespace coder
}  // namespace recomp
