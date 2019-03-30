
#pragma once

#include <chrono>
#include <climits>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "recompression.hpp"
#include "defs.hpp"
#include "rlslp.hpp"

namespace recomp {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression_fast : public recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename recompression<variable_t, terminal_count_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t, terminal_count_t>::bv_t bv_t;
    typedef std::vector<std::map<variable_t, std::pair<size_t, size_t>>> adj_list_t;
    typedef std::vector<bool> partition_t;

    recompression_fast() {
        this->name = "fast_seq";
    }

    recompression_fast(std::string& dataset) : recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "fast_seq";
    }

    /**
     * @brief Builds the straight-line program generating the given text using the recompression technique.
     *
     * A straight-line program (SLP) is in particular a context free grammar in Chomsky normal form.
     * First all letters in the text are replaced by non-terminals which derive the letters. Then
     * the block compression bcomp and the pair compression pcomp alternately compress blocks and local
     * pairs in the texts resulting of the previous compression. This will be repeated until the text
     * contains only one letter.
     *
     * @param text The text
     */
    inline virtual void recomp(text_t& text,
                               rlslp<variable_t, terminal_count_t>& rlslp,
                               const terminal_count_t& alphabet_size,
                               const size_t cores) override {
#ifdef BENCH_RECOMP
        const auto startTime = recomp::timer::now();
        size_t text_size = text.size();
#endif
        std::vector<variable_t> mapping;
        rlslp.terminals = alphabet_size;
        bv_t bv;

        terminal_count_t alpha_size = alphabet_size;

        replace_letters(text, alpha_size, mapping);

        while (text.size() > 1) {
            bcomp(text, rlslp, bv, alpha_size, mapping);
            compute_alphabet(text, alpha_size, mapping);
            this->level++;

            if (text.size() > 1) {
                pcomp(text, rlslp, bv, alpha_size, mapping);
                compute_alphabet(text, alpha_size, mapping);
                this->level++;
            }
        }

        rlslp.resize(rlslp.size());

        if (text.size() > 0) {
            rlslp.root = static_cast<variable_t>(mapping[text[0]]);
            rlslp.is_empty = false;
            this->rename_rlslp(rlslp, bv);
        }

#ifdef BENCH_RECOMP
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=" << this->name << "_recompression dataset=" << this->dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << this->level
                  << " size=" << text_size << std::endl;
#endif
    }

    using recompression<variable_t, terminal_count_t>::recomp;


 private:
    /**
     * @brief Replaces all letters in the text with new non-terminals.
     *
     * @param text[in] The text to compress
     * @param alphabet_size[in,out] The size of the alphabet used in the text (from 0 to alphabet size - 1)
     * @param mapping[out] The mapping of the symbols in the text to the non-terminal
     */
    inline void replace_letters(text_t &t,
                                variable_t& alphabet_size,
                                std::vector<variable_t> &mapping) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        std::vector<bool> alpha(alphabet_size, false);
        for (size_t i = 0; i < t.size(); ++i) {
            alpha[t[i]] = true;
        }

        std::vector<size_t> count(alphabet_size, 0);

        alphabet_size = 0;

        bool first = true;
        for (size_t i = 0; i < alpha.size(); ++i) {
            if (alpha[i]) {
                alphabet_size++;

                if (i > 0 && !first) {
                    count[i] = count[i-1] + 1;
                } else if (first) {
                    first = false;
                }
                mapping.emplace_back(i);
            } else {
                if (i > 0) {
                    count[i] = count[i-1];
                }
            }
        }

        for (size_t i = 0; i < t.size(); ++i) {
            t[i] = count[t[i]];
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=replace_letters dataset=" << this->dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " effective_alphabet=" << alphabet_size << std::endl;
#endif
    }

    /**
     * @brief Detects all blocks in the text and replaces them with new non-terminals.
     *
     * @param text[in,out] The text
     * @param rlslp[in,out] The rlslp
     * @param alphabet_size[in,out] The size of the alphabet used in the text
     * @param mapping[in,out] The mapping of the symbols in the text to the non-terminal
     */
    inline void bcomp(text_t& text,
                      rlslp<variable_t, terminal_count_t>& rlslp,
                      bv_t& bv,
                      variable_t& alphabet_size,
                      std::vector<variable_t>& mapping) {
#ifdef BENCH
        std::cout << "RESULT algo=" << this->name << "_bcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " alphabet=" << alphabet_size;
        const auto startTime = recomp::timer::now();
#endif

#ifdef BENCH
        const auto startTimeBlocks = recomp::timer::now();
#endif
        variable_t next_nt = rlslp.terminals + rlslp.non_terminals.size();
        std::vector<bool> block_vec(alphabet_size);
        variable_t block_len = 1;
        size_t block_count = 0;
        size_t new_text_size = text.size();
        std::vector<std::map<variable_t, variable_t>> blocks(alphabet_size);
        std::vector<std::pair<variable_t, size_t>> positions;
        size_t copy_i = 0;
        bool copy = false;

        for (size_t i = 1; i < text.size(); ++i, ++copy_i) {
            while (i < text.size() && text[i - 1] == text[i]) {
                block_len++;
                i++;
                new_text_size--;
            }
            if (block_len > 1) {
                if (!copy) {
                    text[copy_i] = text[i - 1];
                    copy_i++;
                    copy = true;
                }
                positions.emplace_back(block_len, copy_i - 1);
                block_vec[text[i - 1]] = true;
                blocks[text[i-1]][block_len] = 1;
                block_count++;  // Only for statistics here
                block_len = 1;
            }
            if (copy && i < text.size()) {
                text[copy_i] = text[i];
            }
        }
        if (copy_i < new_text_size) {
            text[copy_i] = text[text.size() - 1];
        }
#ifdef BENCH
        const auto endTimeBlocks = recomp::timer::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count())
                  << " blocks=" << block_count;
#endif

        block_count = 0;

#ifdef BENCH
        const auto startTimeAss = recomp::timer::now();
#endif
        for (size_t i = 0; i < block_vec.size(); ++i) {
            if (block_vec[i]) {
                for (auto& block : blocks[i]) {
                    block.second = alphabet_size++;
                    block_count++;  // Here needed to count the number of different blocks
                    variable_t len = block.first;
                    if (mapping[i] >= rlslp.terminals) {
                        len *= rlslp[mapping[i] - rlslp.terminals].len;
                    }
                    rlslp.non_terminals.emplace_back(mapping[i], block.first, len);
                    mapping.emplace_back(next_nt++);
                }
            }
        }
#ifdef BENCH
        const auto endTimeAss = recomp::timer::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " elements=" << block_count << " block_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif

        if (block_count > 0) {
            rlslp.blocks += block_count;
            bv.resize(rlslp.size(), true);
        }

#ifdef BENCH
        const auto startTimeRep = recomp::timer::now();
#endif
        for (const auto& pos : positions) {
            text[pos.second] = blocks[text[pos.second]][pos.first];
        }
#ifdef BENCH
        const auto endTimeRep = recomp::timer::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif
        text.resize(new_text_size);
//        text.shrink_to_fit();

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }

    /**
     * @brief Computes the multiset representing the adjacency graph of the symbols in the text.
     *
     * @param text[in] The text
     * @return The multiset
     */
    inline void compute_adj_list(const text_t& text, adj_list_t& adj_list) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        // Compute adjacency graph of the symbols in the current text
        for (size_t i = 1; i < text.size(); ++i) {
            if (text[i-1] > text[i]) {
                auto found = adj_list[text[i-1]].find(text[i]);
                if (found == adj_list[text[i-1]].end()) {
                    adj_list[text[i-1]][text[i]].first = 1;
                    adj_list[text[i-1]][text[i]].second = 0;
                } else {
                    adj_list[text[i-1]][text[i]].first++;
                }
            } else {
                auto found = adj_list[text[i]].find(text[i-1]);
                if (found == adj_list[text[i]].end()) {
                    adj_list[text[i]][text[i-1]].first = 0;
                    adj_list[text[i]][text[i-1]].second = 1;
                } else {
                    adj_list[text[i]][text[i-1]].second++;
                }
            }
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
    }

    /**
     * @brief Computes the partition based on the adjacency graph of the text given by the multiset.
     *
     * @param adj_list[i] Multiset representing the adjacency graph of the text
     * @param alphabet_size[in] The size of the alphabet used in the text
     *
     * @return The partition of the symbols in the alphabet to maximize the number of pairs to be compressed
     */
    inline void compute_partition(const adj_list_t& adj_list, partition_t& partition) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        int l_count = 0;
        int r_count = 0;
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (!adj_list[i].empty()) {
                for (const auto& mult : adj_list[i]) {
                    if (partition[mult.first]) {
                        r_count += mult.second.first + mult.second.second;
                    } else {
                        l_count += mult.second.first + mult.second.second;
                    }
                }
                partition[i] = l_count > r_count;
                l_count = 0;
                r_count = 0;
            }
        }
#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
    }

    /**
     * @brief Counts the number of pairs.
     *
     * Counts the number of pairs build by symbols from the left set with symbols from the right set of the
     * partition and vice versa. If the number of pairs build by symbols from the right set with symbols of
     * the left set is greater than the number of symbol pairs from left to right then the sets of the
     * partition are swapped.
     *
     * @param adj_list[in] Multiset representing the adjacency graph of the text
     * @param partition[in,out] The partition of the letters in the current alphabet represented by a bitvector
     */
    inline void count_pairs(const adj_list_t& adj_list,
                            partition_t& partition) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
        for (size_t i = 0; i < adj_list.size(); ++i) {
            for (const auto& sec : adj_list[i]) {
                if (!partition[i] && partition[sec.first]) {
                    lr_count += sec.second.first;
                    rl_count += sec.second.second;
                } else if (partition[i] && !partition[sec.first]) {
                    rl_count += sec.second.first;
                    lr_count += sec.second.second;
                }
            }
        }
#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " lr=" << lr_count << " rl=" << rl_count;
        std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif

        // If there are more pairs in the current text from right set to left set swap partition sets
        if (rl_count > lr_count) {
            partition.flip();
        }
    }

    /**
     * @brief Determines a partition of the letters in the current alphabet.
     *
     * This is an implementation of a greedy 1/4 approximation algorithm.
     *
     * At first the adjacency graph of the text will be computed by creating the multiset
     * of the tuples (t[i], t[i+1], 0) for t[i] > t[i+1] and (t[i+1], t[i], 1) for
     * t[i+1] > t[i]. This multiset will be sorted with radix sort. Then for all increasingly
     * sorted letters c in the current alphabet the sums l and r of freq(c, b, cdot)
     * for all b in the left set of the partition and freq(c, b, cdot) for all b
     * in the right set of the partition will be computed. freq{c, bar{c}, cdot} counts all local
     * pairs of cb and bc in the text. If l is greater then r the letter will be added
     * to the right set of the partition else to the left set. At last the total pairs of the
     * partition from the left set to the right set and vice versa occurring in the text will be
     * counted. If there are more pairs from the right set to the left set occurring in the text
     * the sets will be swapped.
     *
     * @param text[in] The text
     * @param alphabet_size[in] The size of the alphabet used in the text
     *
     * @return The partition of the letters in the current alphabet represented by a bitvector
     */
    inline void partition(const text_t& text,
                          const variable_t& alphabet_size,
                          partition_t& partition) {
        adj_list_t adj_list(alphabet_size);
        compute_adj_list(text, adj_list);

#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        compute_partition(adj_list, partition);

        // Count pairs in the current text based on the pairs build by the partition
        // from left set to right set and vice versa
        count_pairs(adj_list, partition);

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
    }

    /**
     * @brief Detects all pairs in the text and replaces them with new non-terminals.
     *
     * @param text[in,out] The text
     * @param rlslp[in,out] The rlslp
     * @param alphabet_size[in,out] The size of the alphabet used in the text
     * @param mapping[in,out] The mapping of the symbols in the text to the non-terminal
     */
    inline void pcomp(text_t& text,
                      rlslp<variable_t, terminal_count_t>& rlslp,
                      bv_t& bv,
                      variable_t& alphabet_size,
                      std::vector<variable_t>& mapping) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_pcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " alphabet=" << alphabet_size;
#endif

        partition_t part(alphabet_size, false);
        partition(text, alphabet_size, part);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
#endif
        variable_t next_nt = rlslp.terminals + rlslp.non_terminals.size();
        size_t pair_count = 0;
        size_t new_text_size = text.size();
        std::vector<std::map<variable_t, std::vector<variable_t>>> pairs(alphabet_size);
        size_t copy_i = 0;
        bool copy = false;
        size_t pair_c = 0;
        for (size_t i = 1; i < text.size(); ++i, ++copy_i) {
            if (!part[text[i-1]] && part[text[i]]) {
                copy = true;
                auto found = pairs[text[i - 1]].find(text[i]);
                if (found == pairs[text[i - 1]].end()) {
                    mapping.emplace_back(next_nt++);
                    pair_count++;
                }
                pair_c++;
                pairs[text[i - 1]][text[i]].push_back(copy_i);
                text[copy_i] = text[i - 1];
                i++;
                new_text_size--;
            } else if (copy) {
                text[copy_i] = text[i - 1];
            }
        }
        if (copy_i < new_text_size) {
            text[copy_i] = text[text.size() - 1];
        }
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count())
                  << " elements=" << pair_count << " pairs=" << pair_c;
#endif

#ifdef BENCH
        const auto startTimeAss = recomp::timer::now();
#endif
        size_t alpha_size = alphabet_size;
        for (size_t i = 0; i < alpha_size; ++i) {
            if (!part[i]) {
                for (const auto& pair : pairs[i]) {
                    for (const auto& pos : pair.second) {
                        text[pos] = alphabet_size;
                    }
                    alphabet_size++;
                    size_t len = 0;
                    if (mapping[i] >= static_cast<variable_t>(rlslp.terminals)) {
                        len = rlslp[mapping[i] - rlslp.terminals].len;
                    } else {
                        len = 1;
                    }
                    if (mapping[pair.first] >= static_cast<variable_t>(rlslp.terminals)) {
                        len += rlslp[mapping[pair.first] - rlslp.terminals].len;
                    } else {
                        len += 1;
                    }
                    rlslp.non_terminals.emplace_back(mapping[i], mapping[pair.first], len);
                }
            }
        }
//        rlslp.blocks.resize(rlslp.blocks.size() + pair_count, false);
        bv.resize(rlslp.size(), false);

#ifdef BENCH
        const auto endTimeAss = recomp::timer::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " pair_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif

        text.resize(new_text_size);
//        text.shrink_to_fit();

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }

    /**
     * @brief Updates the alphabet used in the current text.
     *
     * @param text[in,out] The current text
     * @param alphabet_size[in,out] The size of the alphabet used in the text
     * @param mapping[in,out] The mapping of the symbols in the text to the non-terminal
     */
    inline void compute_alphabet(text_t& text, variable_t &alphabet_size, std::vector<variable_t> &mapping) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_alphabet dataset=" << this->dataset << " level=" << this->level
                  << " alphabet=" << alphabet_size;
#endif
        // Create bitvector for all letters to determine which are used in the current text
        std::vector<bool> used(alphabet_size, 0);
        std::vector<size_t> ranks(alphabet_size, 0);
        for (size_t i = 0; i < text.size(); ++i) {
            used[text[i]] = true;
        }
        // Build rank structure over the bitvector
        ranks[0] = 0;  // (used[0]? 1 : 0);
        for (size_t i = 1; i < ranks.size(); ++i) {
            if (used[i-1]) {
                ranks[i] = ranks[i-1] + 1;
            } else {
                ranks[i] = ranks[i-1];
            }
        }

        alphabet_size = 0;
        auto m = 0;
        size_t index = 0;
        for (const auto& u : used) {
            // If letter is used in the current text add it to the new alphabet and set the rank of the letter
            if (u) {
                alphabet_size++;
                mapping[index++] = mapping[m];
            }
            ++m;
        }
        mapping.resize(index);
        mapping.shrink_to_fit();

        // overwrite text with ranks of symbols
        for (size_t i = 0; i < text.size(); ++i) {
            text[i] -= (text[i] - ranks[text[i]]);
        }
#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " effective_alphabet=" << alphabet_size << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << std::endl;
#endif
    }
};

}  // namespace recomp
