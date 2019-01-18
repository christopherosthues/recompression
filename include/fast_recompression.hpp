
#pragma once

#include <chrono>
#include <climits>
#include <iostream>
#include <map>
#include <vector>

#include <glog/logging.h>

#include "defs.hpp"
#include "rlslp.hpp"

namespace recomp {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression_fast {
 public:
    typedef std::vector<variable_t> text_t;
//    typedef std::tuple<variable_t, variable_t, bool> multiset_t;
    typedef std::vector<std::map<variable_t, std::pair<size_t, size_t>>> multiset_t;

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
    void recomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, const terminal_count_t& alphabet_size) {
        DLOG(INFO) << "recomp input - text size: " << text.size() << " - alphabet size: "
                   << std::to_string(alphabet_size);
        const auto startTime = std::chrono::system_clock::now();
//        variable_t alphabet_size = 0;
        std::vector<variable_t> mapping;
        rlslp.terminals = alphabet_size;

        terminal_count_t alpha_size = alphabet_size;

        replace_letters(text, /*rlslp,*/ alpha_size, mapping);

        while (text.size() > 1) {
//            std::cout << "BComp" << std::endl;
            bcomp(text, rlslp, alpha_size, mapping);
//            std::cout << "Alpha" << std::endl;
            compute_alphabet(text, alpha_size, mapping);
            if (text.size() > 1) {
//                std::cout << "PComp" << std::endl;
                pcomp(text, rlslp, alpha_size, mapping);
//                std::cout << "Alpha" << std::endl;
                compute_alphabet(text, alpha_size, mapping);
            }
        }

//        bool not_finished = text.size() > 1;
//        rlslp.terminals = alphabet_size;
//
//        while (not_finished) {
//            bcomp(text, rlslp);
//            not_finished = text.size() > 1;
//
//            if (not_finished) {
//                pcomp(text, rlslp);
//                not_finished = text.size() > 1;
//            }
//        }
//
        if (!rlslp.empty()) {
            rlslp.root = static_cast<variable_t>(rlslp.size() - 1);
        }

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for recomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void recomp(text_t& text, rlslp <variable_t, terminal_count_t>& rlslp) {
        recomp(text, rlslp, recomp::CHAR_ALPHABET);
    }

 private:
    const variable_t DELETED = UINT_MAX;

    /**
     * @brief Replaces all letters in the text with new non-terminals.
     *
     * @param text[in] The text to compress
     * @param alphabet_size[in,out] The size of the alphabet used in the text (from 0 to alphabet size - 1)
     * @param mapping[out] The mapping of the symbols in the text to the non-terminal
     */
    void replace_letters(text_t &t,
                         variable_t& alphabet_size,
                         std::vector<variable_t> &mapping) {
        const auto startTime = std::chrono::system_clock::now();
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

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        //std::cout << "Time for replace letters: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
    }

    /**
     * @brief Detects all blocks in the text and replaces them with new non-terminals.
     *
     * @param text[in,out] The text
     * @param alphabet_size[in,out] The size of the alphabet used in the text
     * @param mapping[in,out] The mapping of the symbols in the text to the non-terminal
     */
    void bcomp(text_t& text,
               rlslp<variable_t,
               terminal_count_t>& rlslp,
               variable_t& alphabet_size,
               std::vector<variable_t>& mapping) {
        //std::cout << "Text size (Input BComp): " << text_size << std::endl;
        std::cout << " text=" << text.size();// << " alphabet=" << alphabet.size();
        const auto startTime = std::chrono::system_clock::now();

        const auto startTimeBlock = std::chrono::system_clock::now();
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
                block_count++;
                block_len = 1;
            }
            if (copy) {
                text[copy_i] = text[i];
            }
        }
        if (copy_i < new_text_size) {
            text[copy_i] = text[text.size() - 1];
        }
        const auto endTimeBlock = std::chrono::system_clock::now();
        const auto timeSpanBlock = endTimeBlock - startTimeBlock;
        std::cout << " find_blocks=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlock).count()) << " hits=" << block_count;
        //std::cout << "Time for finding blocks: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlock).count() << "[ms]" << std::endl;

        block_count = 0;

        const auto startTimeAss = std::chrono::system_clock::now();
        for (size_t i = 0; i < block_vec.size(); ++i) {
            if (block_vec[i]) {
                for (auto& block : blocks[i]) {
                    block.second = alphabet_size++;
                    block_count++;
                    rlslp.block_count++;
                    rlslp.non_terminals.emplace_back(mapping[i], block.first, rlslp.non_terminals[mapping[i]].len * block.first);
                    mapping.emplace_back(next_nt++);
                }
            }
        }
        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " block_rules=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
        //std::cout << "Time for block nts: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count() << "[ms]" << std::endl;


        const auto startTimeRep = std::chrono::system_clock::now();
//        text_size = new_text_size;
        for (const auto& pos : positions) {
            text[pos.second] = blocks[text[pos.second]][pos.first];
        }
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_blocks=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
        //std::cout << "Time for replacing blocks: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count() << "[ms]" << std::endl;
        text.resize(new_text_size);
        text.shrink_to_fit();

        /*if (block_count > 0) {
            rlslp.blocks.resize(rlslp.blocks.size() + block_count, true);
        }*/
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        //std::cout << "Time for bcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        //std::cout << "Text size (Ouput BComp): " << text_size << std::endl;
        std::cout << " comp_text=" << text.size();
    }

    /**
     * @brief Computes the multiset representing the adjacency graph of the symbols in the text.
     *
     * @param text[in] The text
     * @return The multiset
     */
    inline void compute_multiset(const text_t& text,
                                 std::vector<std::map<variable_t, std::pair<size_t, size_t>>>& multiset) {
        const auto startTime = std::chrono::system_clock::now();
        // Compute adjacency graph of the symbols in the current text
        for (size_t i = 1; i < text.size(); ++i) {
            if (text[i-1] > text[i]) {
                auto found = multiset[text[i-1]].find(text[i]);
                if (found == multiset[text[i-1]].end()) {
                    multiset[text[i-1]][text[i]].first = 1;
                    multiset[text[i-1]][text[i]].second = 0;
                } else {
                    multiset[text[i-1]][text[i]].first++;
                }
            } else {
                auto found = multiset[text[i]].find(text[i-1]);
                if (found == multiset[text[i]].end()) {
                    multiset[text[i]][text[i-1]].first = 0;
                    multiset[text[i]][text[i-1]].second = 1;
                } else {
                    multiset[text[i]][text[i-1]].second++;
                }
            }
            /*std::pair<variable_t, variable_t> adj;
            if (text[i-1] > text[i]) {
                adj.first = text[i-1];
                adj.second = text[i];
            } else {
                adj.first = text[i];
                adj.second = text[i-1];
            }
            auto found = multiset.find(adj);
            if (found == multiset.end()) {
                if (text[i-1] > text[i]) {
                    multiset[adj].first = 1;
                    multiset[adj].second = 0;
                } else {
                    multiset[adj].first = 0;
                    multiset[adj].second = 1;
                }
            } else {
                if (text[i-1] > text[i]) {
                    (*found).second.first += 1;
                } else {
                    (*found).second.second += 1;
                }
            }*/
        }

        /*std::cout << "Multiset: " << std::endl;
        for (size_t i = 0; i < multiset.size(); ++i) {
            for (const auto& sec : multiset[i]) {
                std::cout << i << "," << sec.first << ": " << sec.second.first << "," << sec.second.second << std::endl;
            }
        }*/

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " multiset=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        //std::cout << "Time for multiset: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
    }

    /**
     * @brief Computes the partition based on the adjacency graph of the text given by the multiset.
     *
     * @param multiset[i] Multiset representing the adjacency graph of the text
     * @param alphabet_size[in] The size of the alphabet used in the text
     *
     * @return The partition of the symbols in the alphabet to maximize the number of pairs to be compressed
     */
    inline void compute_partition(const std::vector<std::map<variable_t, std::pair<size_t, size_t>>>& multiset,
                                  std::vector<bool>& partition,
                                  const variable_t& alphabet_size) {
        const auto startTime = std::chrono::system_clock::now();
        //int lr_count = 0;
        //int rl_count = 0;
        variable_t c = 0;
        int left = 0, right = 0;
        for (size_t i = 0; i < multiset.size(); ++i) {
            while (c < alphabet_size && i > c) {
                partition[c] = left > right;
                c++;
                left = 0;
                right = 0;
            }
            for (const auto& sec : multiset[i]) {
                if (partition[sec.first]) {
                    right += sec.second.first + sec.second.second;
                } else {
                    left += sec.second.first + sec.second.second;
                }
            }
        }

        while (c < alphabet_size) {
            partition[c] = left > right;
            c++;
            left = 0;
            right = 0;
        }
        /*for (const auto& adj : multiset) {
            auto first = adj.first;
            while (c < alphabet_size && first.first > c) {
                partition[c] = left > right;
                c++;
                left = 0;
                right = 0;
            }
            if (partition[first.second]) {
                right += adj.second.first + adj.second.second;
            } else {
                left += adj.second.first + adj.second.second;
            }
        }

        while (c < alphabet_size) {
            partition[c] = left > right;
            c++;
            left = 0;
            right = 0;
        }*/

        /*std::cout << "Partition: " << std::endl;
        for (size_t i = 0; i < partition.size(); ++i) {
            std::cout << partition[i];
        }
        std::cout << std::endl;*/

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        //std::cout << "Time for computing partition: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
    }

    /**
     * @brief Counts the number of pairs.
     *
     * Counts the number of pairs build by symbols from the left set with symbols from the right set of the
     * partition and vice versa. If the number of pairs build by symbols from the right set with symbols of
     * the left set is greater than the number of symbol pairs from left to right then the sets of the
     * partition are swapped.
     *
     * @param multiset[in] Multiset representing the adjacency graph of the text
     * @param partition[in,out] The partition of the letters in the current alphabet represented by a bitvector
     */
    inline void count_pairs(const std::vector<std::map<variable_t, std::pair<size_t, size_t>>>& multiset,
                            std::vector<bool>& partition) {
        const auto startTime = std::chrono::system_clock::now();
        int lr_count = 0;
        int rl_count = 0;
        for (size_t i = 0; i < multiset.size(); ++i) {
            for (const auto& sec : multiset[i]) {
                if (!partition[i] && partition[sec.first]) {
                    lr_count += sec.second.first;
                    rl_count += sec.second.second;
                } else {
                    rl_count += sec.second.first;
                    lr_count += sec.second.second;
                }
            }
        }
        /*for (const auto &adj : multiset) {
            if (!partition[adj.first.first] && partition[adj.first.second]) {  // (c,b,0) -> cb in text
                lr_count += adj.second.first;
                rl_count += adj.second.second;
            } else {
                rl_count += adj.second.first;
                lr_count += adj.second.second;
            }
        }*/

        // If there are more pairs in the current text from right set to left set swap partition sets
        if (rl_count > lr_count) {
            partition.flip();
        }

        /*std::cout << "Partition after count pairs: " << std::endl;
        for (size_t i = 0; i < partition.size(); ++i) {
            std::cout << partition[i];
        }
        std::cout << std::endl;*/

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        //std::cout << "Time for count pairs: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
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
                          std::vector<bool>& partition) {
        //const auto startTime = std::chrono::system_clock::now();
        multiset_t multiset(alphabet_size);
        compute_multiset(text, multiset);

        const auto startTime = std::chrono::system_clock::now();
        compute_partition(multiset, partition, alphabet_size);

        // Count pairs in the current text based on the pairs build by the partition
        // from left set to right set and vice versa
        count_pairs(multiset, partition);

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        //std::cout << "Time for partition: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
    }

    /**
     * @brief Detects all pairs in the text and replaces them with new non-terminals.
     *
     * @param text[in,out] The text
     * @param alphabet_size[in,out] The size of the alphabet used in the text
     * @param mapping[in,out] The mapping of the symbols in the text to the non-terminal
     */
    void pcomp(text_t& text,
               rlslp<variable_t, terminal_count_t>& rlslp,
               variable_t& alphabet_size,
               std::vector<variable_t>& mapping) {
        //std::cout << "Text size (Input PComp): " << text_size << std::endl;
        std::cout << " text=" << text.size() << " alphabet=" << alphabet_size;
        const auto startTime = std::chrono::system_clock::now();
        std::vector<bool> part(alphabet_size, false);
        partition(text, alphabet_size, part);

        const auto startTimePairs = std::chrono::system_clock::now();
        variable_t next_nt = rlslp.terminals + rlslp.non_terminals.size();
        size_t pair_count = 0;
        size_t new_text_size = text.size();
        std::vector<std::map<variable_t, std::vector<variable_t>>> pairs(alphabet_size);
        size_t copy_i = 0;
        bool copy = false;
        //std::cout << "Alphabet: " << alphabet_size << std::endl;
        /*if (text_size == 2) {
            for (const auto& b : part) {
                std::cout << b;
            }
            std::cout << std::endl;
            for (size_t i = 0; i < text_size; ++i) {
                std::cout << t[i] << " ";
            }
            std::cout << std::endl;
        }*/
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
        const auto endTimePairs = std::chrono::system_clock::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count()) << " elements=" << pair_count << " hits=" << pair_c;
        //std::cout << "All pairs computed" << std::endl;

//        text_size = new_text_size;

        const auto startTimeAss = std::chrono::system_clock::now();
//        size_t alpha_size = alphabet_size;
        for (size_t i = 0; i < alphabet_size;/*alpha_size*/ ++i) {
            if (!part[i]) {
                for (const auto& pair : pairs[i]) {
                    for (const auto& pos : pair.second) {
//                        text[pos] = alphabet_size;
                        text[pos] = next_nt;
                    }
                    rlslp.non_terminals.emplace_back(mapping[i], mapping[pair.first], rlslp.non_terminals[mapping[i]].len + rlslp.non_terminals[mapping[pair.first]].len);  // TODO(Chris): adjust
                    next_nt++;
                }
            }
        }
        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " pair_rules=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
        text.resize(new_text_size);
        text.shrink_to_fit();

        //std::cout << "Replaced all pairs" << std::endl;

        /*if (pair_count > 0) {
            rlslp.blocks.resize(rlslp.blocks.size() + pair_count, false);
        }*/

        // Introduce new non-terminals for all found pairs
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " comp_text=" << text.size();
        //std::cout << "Time for pcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;

        //std::cout << "Text size (Ouput PComp): " << text_size << std::endl;
    }

    /**
     * @brief Updates the alphabet used in the current text.
     *
     * @param text[in,out] The current text
     * @param alphabet_size[in,out] The size of the alphabet used in the text
     * @param mapping[in,out] The mapping of the symbols in the text to the non-terminal
     */
    void compute_alphabet(text_t& text, variable_t &alphabet_size, std::vector<variable_t> &mapping) {
        const auto startTime = std::chrono::system_clock::now();
        // Create bitvector for all letters to determine which are used in the current text
        std::vector<bool> used(alphabet_size, 0);
        std::vector<size_t> ranks(alphabet_size, 0);
        for (size_t i = 0; i < text.size(); ++i) {
            used[text[i]] = true;
        }
        // Build rank structure over the bitvector
        ranks[0] = 0;//(used[0]? 1 : 0);
        for (size_t i = 1; i < ranks.size(); ++i) {
            if (used[i-1]) {
                ranks[i] = ranks[i-1] + 1;
            } else {
                ranks[i] = ranks[i-1];
            }
        }

        alphabet_size = 0;
        auto m = 0;
        auto index = 0;
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
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        //std::cout << "Time for alphabet: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
    }
};

}  // namespace recomp
