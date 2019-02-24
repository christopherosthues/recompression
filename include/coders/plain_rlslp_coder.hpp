#pragma once

#include <climits>

#include "defs.hpp"
#include "rlslp.hpp"
#include "coders/coder.hpp"
#include "io/bitostream.hpp"
#include "io/bitistream.hpp"

namespace recomp {
namespace coder {

class PlainRLSLPCoder {
 public:
    PlainRLSLPCoder() = delete;

    class Encoder : coder::Encoder {
     protected:
        BitOStream ostream;

     public:
        using coder::Encoder::encode;

//        inline Encoder(std::ofstream& ostream) : ostream(ostream) {}
        inline Encoder(std::string& file_name) : ostream(file_name) {}

        template<typename variable_t = var_t, typename terminal_count_t = term_t>
        inline void encode(rlslp<variable_t, terminal_count_t>& rlslp) {
            ostream.write_bit(rlslp.is_empty);

            if (!rlslp.is_empty) {
                ostream.write_int<size_t>(rlslp.size());
                ostream.write_int<terminal_count_t>(rlslp.terminals);
                ostream.write_int<variable_t>(rlslp.root);
                ostream.write_int<variable_t>(rlslp.blocks);

                for (const auto& rule : rlslp.non_terminals) {
                    ostream.write_int<variable_t>(rule.first());
                    ostream.write_int<variable_t>(rule.second());
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
        inline Decoder(std::string& file_name) : istream(file_name) {}

        template<typename variable_t = var_t, typename terminal_count_t = term_t>
        inline rlslp<variable_t, terminal_count_t> decode() {
            rlslp<variable_t, terminal_count_t> rlslp;
            bool empty = istream.read_bit();

            if (!empty) {
                auto size = istream.read_int<size_t>();
                rlslp.reserve(size);
                rlslp.resize(size);
                rlslp.terminals = istream.read_int<terminal_count_t>();
                rlslp.root = istream.read_int<variable_t>();
                rlslp.blocks = istream.read_int<variable_t>();

                for (size_t i = 0; i < size; ++i) {
                    variable_t first = istream.read_int<variable_t>();
                    variable_t second = istream.read_int<variable_t>();
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

}  // namespace coder
}  // namespace recomp
