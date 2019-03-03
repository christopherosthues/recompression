#pragma once

#include <io/bitistream.hpp>
#include "defs.hpp"
#include "coders/coder.hpp"
#include "io/bitostream.hpp"
#include "rlslp.hpp"

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
class RLSLPCoder {
 public:
    static const std::string k_extension;

    RLSLPCoder() = delete;

    class Encoder : coder::Encoder {
     protected:
        BitOStream ostream;

     public:
        using coder::Encoder::encode;

        inline Encoder(std::string& file_name) : ostream(file_name + k_extension) {}

        template<typename variable_t = var_t, typename terminal_count_t = term_t>
        inline void encode(rlslp<variable_t, terminal_count_t>& rlslp) {
            ostream.write_bit(rlslp.is_empty);

            if (!rlslp.is_empty) {
                auto bits = util::bits_for(rlslp.size() + rlslp.terminals);
                ostream.write_int<uint8_t>(bits, 6);
                ostream.write_int<size_t>(rlslp.size(), bits);
                ostream.write_int<terminal_count_t>(rlslp.terminals, bits);
                ostream.write_int<variable_t>(rlslp.root, bits);
                ostream.write_int<variable_t>(rlslp.blocks, bits);
            }

            ostream.close();
        }
    };

    class Decoder : public coder::Decoder {
     protected:
        BitIStream istream;

     public:
        using coder::Decoder::decode;

        inline Decoder(std::string& file_name) : istream(file_name + k_extension) {}

        template<typename variable_t = var_t, typename terminal_count_t = term_t>
        inline rlslp<variable_t, terminal_count_t> decode() {
            rlslp<variable_t, terminal_count_t> rlslp;
            bool empty = istream.read_bit();

            if (!empty) {
                auto bits = istream.read_int<uint8_t>(6);
                auto size = istream.read_int<size_t>(bits);
                rlslp.reserve(size);
                rlslp.resize(size);
                rlslp.terminals = istream.read_int<terminal_count_t>(bits);
                rlslp.root = istream.read_int<variable_t>(bits);
                rlslp.blocks = istream.read_int<variable_t>(bits);


                rlslp.compute_lengths();
            }

            rlslp.is_empty = empty;
            istream.close();
            return rlslp;
        }
    };
};

const std::string RLSLPCoder::k_extension = ".rlslp";

}  // namespace coder
}  // namespace recomp
