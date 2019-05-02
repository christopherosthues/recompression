#pragma once

#include <climits>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/coders/coder.hpp"
#include "recompression/io/bitostream.hpp"
#include "recompression/io/bitistream.hpp"

namespace recomp {
namespace coder {

class PlainRLSLPCoder {
 public:
    static const std::string k_extension;

    PlainRLSLPCoder() = delete;

    class Encoder : public coder::Encoder {
     protected:
        BitOStream ostream;

     public:
        inline Encoder(const std::string& file_name) : ostream(file_name + k_extension) {}

        template<typename variable_t = var_t>
        inline void encode(rlslp<variable_t>& rlslp) {
            ostream.write_bit(rlslp.is_empty);

            if (!rlslp.is_empty) {
                ostream.write_int<size_t>(rlslp.size());
                ostream.write_int<size_t>(rlslp.terminals);
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
        inline Decoder(const std::string& file_name) : istream(file_name + k_extension) {}

        template<typename variable_t = var_t>
        inline rlslp<variable_t> decode() {
            rlslp<variable_t> rlslp;
            bool empty = istream.read_bit();

            if (!empty) {
                auto size = istream.read_int<size_t>();
                // rlslp.reserve(size);
                rlslp.resize(size, 1);
                rlslp.terminals = istream.read_int<size_t>();
                rlslp.root = istream.read_int<variable_t>();
                rlslp.blocks = istream.read_int<variable_t>();

                for (size_t i = 0; i < size; ++i) {
                    variable_t first = istream.read_int<variable_t>();
                    variable_t second = istream.read_int<variable_t>();
                    rlslp[i] = non_terminal<variable_t>(first, second);
                }

                rlslp.compute_lengths();
            }
            rlslp.is_empty = empty;
            istream.close();
            return rlslp;
        }
    };
};

const std::string PlainRLSLPCoder::k_extension = ".plain";

}  // namespace coder
}  // namespace recomp
