
#pragma once

#include "coders/coder.hpp"

namespace recomp {
namespace coder {
/**
 * @brief A class to delta encode and decode values.
 *
 * The value used for computing the delta is the last value that was delta encoded. If no such value is
 * given 0 is used for the first value to be encoded.
 *
 * @tparam value_t The type of values to delta-encode.
 */
template<typename value_t>
class DeltaCoder {
 public:
    DeltaCoder() = delete;

    class Encoder : coder::Encoder {
     protected:
        value_t last = 0;

     public:
        using coder::Encoder::encode;

        inline void reset() {
            last = 0;
        }

        template<typename out_t>
        inline out_t encode(value_t v) {
            auto out = v - last;
            last = v;
            return out;
        }
    };

    class Decoder : public coder::Decoder {
     protected:
        value_t last = 0;

     public:
        using coder::Decoder::decode;

        inline void reset() {
            last = 0;
        }

        template<typename in_t>
        inline value_t decode(in_t in) {
            last = in + last;
            return last;
        }
    };
};
}  // namespace coder
}  // namespace recomp
