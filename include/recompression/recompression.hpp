#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "defs.hpp"
#include "rlslp.hpp"

namespace recomp {

/**
 * This is the interface for all implementations of the recompression provided by this library.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class recompression {
 public:
    typedef ui_vector<variable_t> text_t;
    typedef std::vector<variable_t> alphabet_t;
    typedef std::vector<bool> bv_t;

    std::string name;
    std::string dataset = "data";
    size_t level = 0;
    size_t cores = 1;

    inline recompression() = default;

    /**
     *
     * @param dataset The name of the dataset
     */
    inline recompression(std::string& dataset) : dataset(dataset) {}

    /**
     * @brief Builds the straight-line program generating the given text using the recompression technique.
     *
     * A straight-line program (SLP) is in particular a context free grammar in Chomsky normal form.
     * First all letters in the text are replaced by non-terminals which derive the letters. Then
     * the block compression bcomp and the pair compression pcomp alternately compress blocks and local
     * pairs in the texts resulting of the previous compression. This will be repeated until the text
     * contains only one letter.
     *
     * @param text[in,out] The text
     * @param rlslp[out] The rlslp
     * @param alphabet_size The first non-terminal to generate
     * @param cores The number of cores to use
     */
    virtual void recomp(text_t& text,
                        rlslp<variable_t>& rlslp,
                        const size_t& alphabet_size,
                        const size_t cores) {};

    /**
     * @brief Builds the straight-line program generating the given text using the recompression technique.
     *
     * A straight-line program (SLP) is in particular a context free grammar in Chomsky normal form.
     * First all letters in the text are replaced by non-terminals which derive the letters. Then
     * the block compression bcomp and the pair compression pcomp alternately compress blocks and local
     * pairs in the texts resulting of the previous compression. This will be repeated until the text
     * contains only one letter.
     *
     * @param text[in,out] The text
     * @param rlslp[out] The rlslp
     * @param cores The number of cores to use
     */
    virtual void recomp(text_t& text,
                        rlslp <variable_t>& rlslp,
                        const size_t cores) {
        this->recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
    }

 protected:
    /**
     * @brief Moves all block rules to the end and renames the non-terminals according to their new position.
     *
     * @param rlslp The rlslp to rename
     * @param bv The bitvector indicating the block rules
     */
    void rename_rlslp(rlslp<variable_t>& rlslp, const bv_t& bv) {
#ifdef BENCH
        const auto startTimeRlslp = recomp::timer::now();
#endif
        if (rlslp.size() > 0) {
            rlslp.blocks = rlslp.size() - rlslp.blocks;

            if (!bv[rlslp.root - rlslp.terminals]) {
                rlslp.root = rlslp.blocks + rlslp.terminals - 1;
            }

            std::vector<variable_t> renamed(rlslp.size());
            std::vector<recomp::non_terminal<variable_t>> renamed_rules(rlslp.size() - rlslp.blocks);
            variable_t block_i = 0;
            variable_t pair_i = 0;
            for (size_t i = 0; i < rlslp.size(); ++i) {
                if (bv[i]) {
                    renamed_rules[block_i] = rlslp[i];
                    renamed[i] = block_i + rlslp.blocks + rlslp.terminals;
                    block_i++;
                } else {
                    rlslp[pair_i] = rlslp[i];
                    renamed[i] = pair_i + rlslp.terminals;
                    pair_i++;
                }
            }

#pragma omp parallel for schedule(static) num_threads(cores)
            for (size_t i = 0; i < rlslp.blocks; ++i) {
                if (!rlslp.is_terminal(rlslp[i].first())) {
                    rlslp[i].first() = renamed[rlslp[i].first() - rlslp.terminals];
                }
                if (!rlslp.is_terminal(rlslp[i].second())) {
                    rlslp[i].second() = renamed[rlslp[i].second() - rlslp.terminals];
                }
            }
#pragma omp parallel for schedule(static) num_threads(cores)
            for (size_t i = 0; i < renamed_rules.size(); ++i) {
                if (!rlslp.is_terminal(renamed_rules[i].first())) {
                    renamed_rules[i].first() = renamed[renamed_rules[i].first() - rlslp.terminals];
                }
                rlslp[i + rlslp.blocks] = renamed_rules[i];
            }
        }
#ifdef BENCH
        const auto endTimeRlslp = recomp::timer::now();
        const auto timeSpanRlslp = endTimeRlslp - startTimeRlslp;
        std::cout << "RESULT algo=" << this->name << "_rlslp dataset=" << this->dataset << " blocks="
                  << (rlslp.size() - rlslp.blocks) << " cores=" << this->cores << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRlslp).count() << std::endl;
#endif
    }
};

}  // namespace recomp
