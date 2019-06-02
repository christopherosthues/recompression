#pragma once

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {
namespace coder {

/**
 * @brief A base class for all encoders that encodes a rlslp.
 */
class Encoder {
 public:
    /**
     * @brief Encodes a given rlslp and writes it to a file.
     *
     * @tparam variable_t The type of non-terminals
     * @param rlslp The rlslp
     */
    template<typename variable_t = var_t>
    inline void encode(rlslp<variable_t>& rlslp) {}
};

/**
 * @brief A base class for all decoders that decodes a rlslp.
 */
class Decoder {
 public:
    /**
     * @brief Decodes a rlslp from a file.
     *
     * @tparam variable_t The type of non-terminals
     * @return the decoded rlslp
     */
    template<typename variable_t = var_t>
    inline rlslp<variable_t> decode() {}
};

}  // namespace coder
}  // namespace recomp
