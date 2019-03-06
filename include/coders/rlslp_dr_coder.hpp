#pragma once

#include <vector>

#include "io/bitistream.hpp"
#include "defs.hpp"
#include "coders/coder.hpp"
#include "io/bitostream.hpp"
#include "rlslp.hpp"
#include "coders/rlslp_rule_sorter.hpp"

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
class RLSLPDRCoder {
 public:
    static const std::string k_extension;

    RLSLPDRCoder() = delete;

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

//                std::cout << "Begin rule sort" << std::endl;

                sort_rlslp_rules(rlslp);

//                std::cout << "rules sorted" << std::endl;

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

//                std::cout << "Begin" << std::endl;
                int64_t delta_dr = 0;
//                int64_t max = 0;
                std::vector<bool> neg(rlslp.blocks);
                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    int64_t diff = rlslp[i].second() - delta_dr;
                    neg[i] = diff < 0;
                    if (diff < 0) {
                        diff = -diff;
                    }
                    ostream.write_elias_gamma_code<int64_t>(diff);
//                    std::cout << "Writing pair " << diff << std::endl;
//                    max = std::max(max, diff);
//                    ostream.write_unary(diff);
                    delta_dr = rlslp[i].second();
                }
//                std::cout << "Max pair: " << max << std::endl;
                if (rlslp.blocks > 0) {
                    ostream.write_bitvector_compressed(neg);
//                    for (size_t i = 0; i < neg.size(); ++i) {
//                        std::cout << neg[i] << ", ";
//                    }
//                    std::cout << std::endl;
                }

                if (rlslp.size() != rlslp.blocks) {
                    delta_dr = 0;
                    size_t number = 0;
                    neg.resize(rlslp.size() - rlslp.blocks);
                    neg.shrink_to_fit();
//                    max = 0;
                    for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                        int64_t diff = rlslp[i].first() - delta_dr;
                        neg[i - rlslp.blocks] = diff < 0;
                        if (diff < 0) {
                            diff = -diff;
                            number++;
                        }
//                        max = std::max(max, diff);
//                        std::cout << "Writing block first " << diff << std::endl;
                        ostream.write_elias_gamma_code<int64_t>(diff);
//                        ostream.write_unary(diff);
                        delta_dr = rlslp[i].first();
                    }
//                    std::cout << "Max block first: " << max << std::endl;
                    if (number > 0) {
                        ostream.write_bit(true);
                        ostream.write_bitvector_compressed(neg);
                    } else {
                        ostream.write_bit(false);
                    }

//                    for (size_t i = 0; i < neg.size(); ++i) {
//                        std::cout << neg[i] << ", ";
//                    }
//                    std::cout << std::endl;

                    delta_dr = 0;
                    number = 0;
//                    max = 0;
                    for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                        int64_t diff = rlslp[i].second() - delta_dr;
                        neg[i - rlslp.blocks] = diff < 0;
                        if (diff < 0) {
                            diff = -diff;
                            number++;
                        }
//                        std::cout << "Writing block sec " << diff << std::endl;
//                        max = std::max(max, diff);
//                        ostream.write_unary(diff);
                        ostream.write_elias_gamma_code<int64_t>(diff);
                        delta_dr = rlslp[i].second();
                    }
//                    std::cout << "Max block second: " << max << std::endl;
                    if (number > 0) {
                        ostream.write_bit(true);
                        ostream.write_bitvector_compressed(neg);
                    } else {
                        ostream.write_bit(false);
                    }
//                    for (size_t i = 0; i < neg.size(); ++i) {
//                        std::cout << neg[i] << ", ";
//                    }
//                    std::cout << std::endl;
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
                    delta += istream.read_unary<variable_t>();
                    rlslp[i] = non_terminal<variable_t, terminal_count_t>{delta, 0};
                }

                std::vector<int64_t> diffs(rlslp.blocks);
                for (size_t i = 0; i < rlslp.blocks; ++i) {
//                    diffs[i] = istream.read_unary<int64_t>();
                    diffs[i] = istream.read_elias_gamma_code<int64_t>();
                }
                std::vector<bool> neg;
                if (rlslp.blocks > 0) {
                    neg = istream.read_bitvector_compressed();
                }

                variable_t delta_dr = 0;
                for (size_t i = 0; i < rlslp.blocks; ++i) {
                    delta_dr += (variable_t)(neg[i]? -diffs[i] : diffs[i]);
                    rlslp[i].second() = delta_dr;
                }

                if (rlslp.size() != rlslp.blocks) {
                    delta_dr = 0;
                    diffs.resize(rlslp.size() - rlslp.blocks);
                    diffs.shrink_to_fit();
                    for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
//                        diffs[i - rlslp.blocks] = istream.read_unary<int64_t>();
                        diffs[i - rlslp.blocks] = istream.read_elias_gamma_code<int64_t>();
                    }
                    bool num = istream.read_bit();
                    if (num) {
                        neg = istream.read_bitvector_compressed();

                        for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                            delta_dr += (variable_t) (neg[i - rlslp.blocks] ? -diffs[i - rlslp.blocks] : diffs[i - rlslp.blocks]);
                            rlslp[i].first() = delta_dr;
                        }
                    } else {
                        delta = 0;
                        for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                            delta += (variable_t) diffs[i - rlslp.blocks];
                            rlslp[i].first() = delta;
                        }
                    }

                    delta_dr = 0;
                    for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                        diffs[i - rlslp.blocks] = istream.read_elias_gamma_code<int64_t>();
//                        diffs[i - rlslp.blocks] = istream.read_unary<int64_t>();
                    }
                    num = istream.read_bit();
                    if (num) {
                        neg = istream.read_bitvector_compressed();

                        for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                            delta_dr += (variable_t) (neg[i - rlslp.blocks] ? -diffs[i - rlslp.blocks] : diffs[i - rlslp.blocks]);
                            rlslp[i].second() = delta_dr;
                        }
                    } else {
                        delta = 0;
                        for (size_t i = rlslp.blocks; i < rlslp.size(); ++i) {
                            delta += (variable_t) diffs[i - rlslp.blocks];
                            rlslp[i].second() = delta;
                        }
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

const std::string RLSLPDRCoder::k_extension = ".rlslp_dr";

}  // namespace coder
}  // namespace recomp
