#pragma once

#ifdef BENCH
#include <iostream>
#endif

#include "recompression/io/bitistream.hpp"
#include "recompression/defs.hpp"
#include "coder.hpp"
#include "recompression/io/bitostream.hpp"
#include "recompression/rlslp.hpp"
#include "rlslp_rule_sorter.hpp"

namespace recomp {
namespace coder {

/**
 * @brief This class implements a coder that encodes and decodes the rules of the rlslp by first permuting and renaming
 * the variables such that the first symbols of each pairs are sorted in increasing order.
 */
class SortedRLSLPCoder {
 public:
    static const std::string k_extension;

    SortedRLSLPCoder() = delete;

    class Encoder : public coder::Encoder {
     protected:
        BitOStream ostream;

     public:
        inline Encoder(const std::string& file_name) : ostream(file_name + k_extension) {}

        template<typename variable_t = var_t>
        inline void encode(rlslp<variable_t>& rlslp) {
#ifdef BENCH
            const auto startTime = recomp::timer::now();
#endif
            ostream.write_bit(rlslp.is_empty);

            if (!rlslp.is_empty) {
                auto bits = util::bits_for(rlslp.size() + rlslp.terminals);

                sort_rlslp_rules(rlslp);

                ostream.write_int<uint8_t>(bits, 6);
                ostream.write_int<size_t>(rlslp.size(), bits);
                ostream.write_int<size_t>(rlslp.terminals, bits);
                ostream.write_int<variable_t>(rlslp.root, bits);
                ostream.write_int<variable_t>(rlslp.blocks, bits);

                variable_t delta = 0;
                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    ostream.write_unary(rlslp[i].first() - delta);
                    delta = rlslp[i].first();
                }

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
            }

            ostream.close();
#ifdef BENCH
            const auto endTime = recomp::timer::now();
            const auto timeSpan = endTime - startTime;
            std::string dataset = ostream.get_file_name();

            util::file_name_without_path(dataset);
            util::file_name_without_extension(dataset);
            util::replace_all(dataset, "_", "\\_");

            std::cout << "RESULT algo=enc_rlslp dataset=" << dataset
                      << " time=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                      << " productions=" << rlslp.size() << " blocks=" << (rlslp.size() - rlslp.blocks)
                      << " empty=" << rlslp.is_empty << std::endl;
#endif
        }
    };

    class Decoder : public coder::Decoder {
     protected:
        BitIStream istream;

     public:
        inline Decoder(const std::string& file_name) : istream(file_name + k_extension) {}

        template<typename variable_t = var_t>
        inline rlslp<variable_t> decode() {
#ifdef BENCH
            const auto startTime = recomp::timer::now();
#endif
            rlslp<variable_t> rlslp;
            bool empty = istream.read_bit();

            if (!empty) {
                auto bits = istream.read_int<uint8_t>(6);
                auto size = istream.read_int<size_t>(bits);
                rlslp.resize(size);
                rlslp.terminals = istream.read_int<size_t>(bits);
                rlslp.root = istream.read_int<variable_t>(bits);
                rlslp.blocks = istream.read_int<variable_t>(bits);

                variable_t delta = 0;
                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    delta = istream.read_unary<variable_t>() + delta;
                    rlslp[i] = non_terminal<variable_t>{delta, 0};
                }

                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    rlslp[i].second() = istream.read_int<variable_t>(bits);
                }

                auto block_bits = istream.read_int<uint8_t>(6);
                auto len_bits = istream.read_int<uint8_t>(6);
                for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                    variable_t first = istream.read_int<variable_t>(block_bits);
                    variable_t second = istream.read_int<variable_t>(len_bits);
                    rlslp[i] = non_terminal<variable_t>(first, second);
                }

                rlslp.compute_lengths();
            }

            rlslp.is_empty = empty;
            istream.close();
#ifdef BENCH
            const auto endTime = recomp::timer::now();
            const auto timeSpan = endTime - startTime;
            std::string dataset = istream.get_file_name();

            util::file_name_without_path(dataset);
            util::file_name_without_extension(dataset);
            util::replace_all(dataset, "_", "\\_");

            std::cout << "RESULT algo=dec_rlslp dataset=" << dataset
                      << " time=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                      << " productions=" << rlslp.size() << " blocks=" << (rlslp.size() - rlslp.blocks)
                      << " empty=" << rlslp.is_empty << std::endl;
#endif
            return rlslp;
        }
    };
};

const std::string SortedRLSLPCoder::k_extension = ".rlslp";

}  // namespace coder
}  // namespace recomp
