
#pragma once

namespace recomp {
namespace coder {

class Encoder {
 public:
    template<typename value_t, typename out_t>
    inline out_t encode(value_t v) {}
};

class Decoder {
 public:
    template<typename value_t, typename in_t>
    inline value_t decode(in_t in) {}
};

}  // namespace coder
}  // namespace recomp
