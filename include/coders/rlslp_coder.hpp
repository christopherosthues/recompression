#pragma once

#include "io/bitistream.hpp"
#include "defs.hpp"
#include "coders/coder.hpp"
#include "io/bitostream.hpp"
#include "rlslp.hpp"
#include "coders/rlslp_rule_sorter.hpp"
#include "coders/delta_coder.hpp"

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

                sort_rlslp_rules(rlslp);

                ostream.write_int<uint8_t>(bits, 6);
                ostream.write_int<size_t>(rlslp.size(), bits);
                ostream.write_int<terminal_count_t>(rlslp.terminals, bits);
                ostream.write_int<variable_t>(rlslp.root, bits);
                ostream.write_int<variable_t>(rlslp.blocks, bits);

                variable_t delta = 0;
                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    ostream.write_unary(rlslp[i].first() - delta);
                    delta = rlslp[i].first();
                }

//                delta = 0;
//                variable_t max_val = 0;
//                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
//                    ostream.write_unary(rlslp[i].first() - delta);
//                    delta = rlslp[i].first();
//                    if (max_val < rlslp[i].second()) {
//                        max_val = rlslp[i].second();
//                    }
//                }

                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    ostream.write_int<variable_t>(rlslp[i].second(), bits);
                }

                variable_t max_val = 0;
                variable_t max_len = 0;
                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                    if (max_val < rlslp[i].first()) {
                        max_val = rlslp[i].first();
                    }
                    if (max_len < rlslp[i].second()) {
                        max_len = rlslp[i].second();
                    }
                }

                auto block_bits = util::bits_for(max_val);
                auto len_bits = util::bits_for(max_len);
                ostream.write_int<uint8_t>(block_bits, 6);
                ostream.write_int<uint8_t>(len_bits, 6);
                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                    ostream.write_int<variable_t>(rlslp[i].first(), block_bits);
                    ostream.write_int<variable_t>(rlslp[i].second(), len_bits);
                }

//                bits = util::bits_for(max_val);
//                ostream.write_int<uint8_t>(bits, 6);
//                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
//                    ostream.write_int<variable_t>(rlslp[i].second(), bits);
//                }
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

                variable_t delta = 0;
                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    delta = istream.read_unary<variable_t>() + delta;
                    rlslp[i] = non_terminal<variable_t, terminal_count_t>{delta, 0};
                }

//                delta = 0;
//                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
//                    delta = istream.read_unary<variable_t>() + delta;
//                    rlslp[i] = non_terminal<variable_t, terminal_count_t>{delta, 0};
//                }

                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    rlslp[i].second() = istream.read_int<variable_t>(bits);
                }

//                bits = istream.read_int<uint8_t>(6);
//                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
//                    rlslp[i].second() = istream.read_int<variable_t>(bits);
//                }
                auto block_bits = istream.read_int<uint8_t>(6);
                auto len_bits = istream.read_int<uint8_t>(6);
                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                    variable_t first = istream.read_int<variable_t>(block_bits);
                    variable_t second = istream.read_int<variable_t>(len_bits);
                    rlslp[i] = non_terminal<variable_t, terminal_count_t>(first, second);
                }

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
