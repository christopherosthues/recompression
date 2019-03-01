#pragma once

#include <climits>

#include "defs.hpp"
#include "rlslp.hpp"
#include "coders/coder.hpp"
#include "io/bitostream.hpp"
#include "io/bitistream.hpp"

namespace recomp {
namespace coder {

class PlainRLSLPWLZCoder {
 public:
    static const std::string k_extension;

    PlainRLSLPWLZCoder() = delete;

    class Encoder : coder::Encoder {
     protected:
        BitOStream ostream;

     public:
        using coder::Encoder::encode;

//        inline Encoder(std::ofstream& ostream) : ostream(ostream) {}
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

                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    ostream.write_int<variable_t>(rlslp.non_terminals[i].first(), bits);
                    ostream.write_int<variable_t>(rlslp.non_terminals[i].second(), bits);
                }

                variable_t max_val = 0;
                variable_t max_len = 0;
                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                    if (max_val < rlslp.non_terminals[i].first()) {
                        max_val = rlslp.non_terminals[i].first();
                    }
                    if (max_len < rlslp.non_terminals[i].second()) {
                        max_len = rlslp.non_terminals[i].second();
                    }
                }

                auto block_bits = util::bits_for(max_val);
                auto len_bits = util::bits_for(max_len);
                ostream.write_int<uint8_t>(block_bits, 6);
                ostream.write_int<uint8_t>(len_bits, 6);
                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                    ostream.write_int<variable_t>(rlslp.non_terminals[i].first(), block_bits);
                    ostream.write_int<variable_t>(rlslp.non_terminals[i].second(), len_bits);
                }
            }

            ostream.close();
        }
    };

    class Decoder : public coder::Decoder {
     protected:
        BitIStream istream;

     public:
        using coder::Decoder::decode;

//        inline Decoder(BitIStream& istream) : istream(istream) {}
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

                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    variable_t first = istream.read_int<variable_t>(bits);
                    variable_t second = istream.read_int<variable_t>(bits);
                    rlslp[i] = non_terminal<variable_t, terminal_count_t>(first, second);
                }
                auto block_bits = istream.read_int<uint8_t>(6);
                auto len_bits = istream.read_int<uint8_t>(6);
                if (block_bits > 0) {
                    for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                        variable_t first = istream.read_int<variable_t>(block_bits);
                        variable_t second = istream.read_int<variable_t>(len_bits);
                        rlslp[i] = non_terminal<variable_t, terminal_count_t>(first, second);
                    }
                }

                rlslp.compute_lengths();
            }
            rlslp.is_empty = empty;
            istream.close();
            return rlslp;
        }
    };
};

const std::string PlainRLSLPWLZCoder::k_extension = ".wlz";

}  // namespace coder
}  // namespace recomp
