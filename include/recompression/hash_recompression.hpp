
#pragma once

#include <chrono>
#include <climits>
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression.hpp"
#include "defs.hpp"
#include "rlslp.hpp"

namespace recomp {

/**
 * @brief This class is a sequential implementation of the recompression using unordered_maps.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class hash_recompression : public recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename recompression<variable_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t>::bv_t bv_t;
    typedef std::array<std::unordered_map<std::pair<variable_t, variable_t>, size_t, pair_hash>, 2> adj_list_comp_t;
    typedef std::array<std::vector<std::tuple<variable_t, variable_t, size_t>>, 2> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    inline hash_recompression() {
        this->name = "hash";
    }

    inline hash_recompression(std::string& dataset) : recompression<variable_t>(dataset) {
        this->name = "hash";
    }

    inline virtual void recomp(text_t& text,
                               rlslp<variable_t>& rlslp,
                               const size_t& alphabet_size,
                               const size_t cores) override {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        size_t text_size = text.size();
#endif
        rlslp.terminals = alphabet_size;
        bv_t bv;

        while (text.size() > 1) {
            bcomp(text, rlslp, bv);
            this->level++;

            if (text.size() > 1) {
                pcomp(text, rlslp, bv);
                this->level++;
            }
        }

        if (text.size() > 0) {
            rlslp.root = static_cast<variable_t>(text[0]);
            rlslp.is_empty = false;
            this->rename_rlslp(rlslp, bv);
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=" << this->name << "_recompression dataset=" << this->dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << this->level
                  << " size=" << text_size << std::endl;
#endif
    }

    using recompression<variable_t>::recomp;


 private:
    /**
     * @brief Detects all blocks in the text and replaces them with new non-terminals.
     *
     * @param text[in,out] The text
     * @param rlslp[in,out] The rlslp
     * @param bv[in,out] The bitvector to indicate which rules derive blocks
     */
    inline void bcomp(text_t& text, rlslp<variable_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_bcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level;
        const auto startTimeBlocks = recomp::timer::now();
#endif
        variable_t next_nt = rlslp.terminals + rlslp.non_terminals.size();
        variable_t block_len = 1;
        size_t block_count = 0;
        size_t new_text_size = text.size();
        std::unordered_map<std::pair<variable_t, variable_t>, variable_t, pair_hash> blocks;
        size_t copy_i = 0;
        bool copy = false;

        std::deque<non_terminal<variable_t>> new_rules;
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
                auto block = std::make_pair(text[i - 1], block_len);
                auto found = blocks.find(block);
                variable_t new_nt = 0;
                if (found == blocks.end()) {
                    new_nt = next_nt;

                    auto len = block_len;
                    if (text[i - 1] >= rlslp.terminals) {
                        len *= rlslp[text[i - 1] - rlslp.terminals].len;
                    }
                    new_rules.emplace_back(text[i - 1], block_len, len);

                    blocks[block] = next_nt++;
                } else {
                    new_nt = (*found).second;
                }
                text[copy_i - 1] = new_nt;
                block_count++;  // Only for statistics here
                block_len = 1;
            }
            if (copy) {
                text[copy_i] = text[i];
            }
        }
        size_t size = rlslp.size();
        rlslp.resize(size + new_rules.size());
        for (size_t i = 0; i < new_rules.size(); ++i) {
            rlslp[size + i] = new_rules[i];
        }
        if (copy_i < new_text_size) {
            text[copy_i] = text[text.size() - 1];
        }
#ifdef BENCH
        const auto endTimeBlocks = recomp::timer::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count())
                  << " elements=" << blocks.size() << " blocks=" << block_count;
        const auto startTimeIP = recomp::timer::now();
#endif
        if (blocks.size() > 0) {
            rlslp.blocks += blocks.size();
            bv.resize(rlslp.size(), true);
        }

        text.resize(new_text_size);
#ifdef BENCH
        const auto endTimeIP = recomp::timer::now();
        const auto timeSpanIP = endTimeIP - startTimeIP;
        std::cout << " resize_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanIP).count());
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }

    /**
     * @brief Computes the adjacency list of the text.
     *
     * @param text[in] The text
     * @param adj[out] The adjacency list
     * @param part[out] The partition initializes with zeroes
     */
    inline void compute_adj_list(const text_t& text,
                                 adj_list_t& adj,
                                 partition_t& part) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        adj_list_comp_t adj_list;
        // Compute adjacency graph of the symbols in the current text
        part[text[0]] = false;
        for (size_t i = 1; i < text.size(); ++i) {
            part[text[i]] = false;
            if (text[i - 1] > text[i]) {
                auto key = std::make_pair(text[i - 1], text[i]);
                if (adj_list[0].find(key) == adj_list[0].end()) {
                    adj_list[0][key] = 1;
                } else {
                    (adj_list[0][key])++;
                }
            } else {
                auto key = std::make_pair(text[i], text[i - 1]);
                if (adj_list[1].find(key) == adj_list[1].end()) {
                    adj_list[1][key] = 1;
                } else {
                    (adj_list[1][key])++;
                }
            }
        }

        for (size_t i = 0; i < 2; ++i) {
            for (const auto& ad : adj_list[i]) {
                adj[i].push_back(std::make_tuple(ad.first.first, ad.first.second, ad.second));
            }
        }
#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        const auto startTimeAdj = recomp::timer::now();
#endif
        ips4o::sort(adj[0].begin(), adj[0].end());
        ips4o::sort(adj[1].begin(), adj[1].end());
#ifdef BENCH
        const auto endTimeAdj = recomp::timer::now();
        const auto timeSpanAdj = endTimeAdj - startTimeAdj;
        std::cout << " sort_adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAdj).count();
#endif
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
     * @param partition[out] The partitioning of the symbols
     * @param part_l[out] Indicates which values are in the left partition set
     */
    inline void compute_partition(const text_t& text,
                                  partition_t& partition,
                                  bool& part_l) {
        adj_list_t adj;
        compute_adj_list(text, adj, partition);
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        int l_count = 0;
        int r_count = 0;
        size_t glob_i = 0;
        size_t l = 0;
        size_t r = 0;
        variable_t actual = 0;
        if (!adj[0].empty()) {
            actual = std::get<0>(adj[0][0]);
            if (!adj[1].empty()) {
                actual = std::min(actual, std::get<0>(adj[1][0]));
            }
        } else if (!adj[1].empty()) {
            actual = std::get<0>(adj[1][0]);
        }
        variable_t next = actual;
        size_t size = adj[0].size() + adj[1].size();
        while (glob_i < size) {
            while (l < adj[0].size() && std::get<0>(adj[0][l]) == actual) {
                if (partition[std::get<1>(adj[0][l])]) {
                    r_count += std::get<2>(adj[0][l]);
                } else {
                    l_count += std::get<2>(adj[0][l]);
                }
                l++;
                glob_i++;
            }
            if (l < adj[0].size()) {
                next = std::get<0>(adj[0][l]);
            }

            while (r < adj[1].size() && std::get<0>(adj[1][r]) == actual) {
                if (partition[std::get<1>(adj[1][r])]) {
                    r_count += std::get<2>(adj[1][r]);
                } else {
                    l_count += std::get<2>(adj[1][r]);
                }
                r++;
                glob_i++;
            }
            partition[actual] = l_count > r_count;
            l_count = 0;
            r_count = 0;

            if (r < adj[1].size() && l < adj[0].size()) {
                actual = std::min(next, std::get<0>(adj[1][r]));
            } else if (r < adj[1].size()) {
                actual = std::get<0>(adj[1][r]);
            } else {
                actual = next;
            }
        }
#ifdef BENCH
        const auto endTimeUndir = recomp::timer::now();
        const auto timeSpanUndir = endTimeUndir - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanUndir).count();
        const auto startTimeDir = recomp::timer::now();
#endif
        // Count pairs in the current text based on the pairs build by the partition
        // from left set to right set and vice versa
        int lr_count = 0;
        int rl_count = 0;
        for (size_t k = 0; k < text.size() - 1; ++k) {
            if (!partition[text[k]] && partition[text[k + 1]]) {
                lr_count++;
            } else if (partition[text[k]] && !partition[text[k + 1]]) {
                rl_count++;
            }
        }
        // If there are more pairs in the current text from right set to left set swap partition sets
        part_l = rl_count > lr_count;
#ifdef BENCH
        const auto endTimeDir = recomp::timer::now();
        const auto timeSpanDir = endTimeDir - startTimeDir;
        std::cout << " lr=" << lr_count << " rl=" << rl_count;
        std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanDir).count();
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
     * @param bv[in,out] The bitvector to indicate which rules derive blocks
     */
    inline void pcomp(text_t& text, rlslp<variable_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_pcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level;
#endif
        partition_t part;
        bool part_l = false;
        compute_partition(text, part, part_l);

#ifdef BENCH
        std::cout << " alphabet=" << part.size();
        const auto startTimePairs = recomp::timer::now();
#endif
        variable_t next_nt = rlslp.terminals + rlslp.non_terminals.size();
        size_t pair_count = 0;
        size_t new_text_size = text.size();
        std::unordered_map<std::pair<variable_t, variable_t>, variable_t, pair_hash> pairs;
        size_t copy_i = 0;
        bool copy = false;
        std::deque<non_terminal<variable_t>> new_rules;
        for (size_t i = 1; i < text.size(); ++i, ++copy_i) {
            if (part_l == part[text[i - 1]] && part_l != part[text[i]]) {
                copy = true;

                auto pair = std::make_pair(text[i - 1], text[i]);
                auto found = pairs.find(pair);
                variable_t new_nt = 0;
                if (found == pairs.end()) {
                    new_nt = next_nt;

                    size_t len = 0;
                    if (text[i - 1] >= static_cast<variable_t>(rlslp.terminals)) {
                        len = rlslp[text[i - 1] - rlslp.terminals].len;
                    } else {
                        len = 1;
                    }
                    if (text[i] >= static_cast<variable_t>(rlslp.terminals)) {
                        len += rlslp[text[i] - rlslp.terminals].len;
                    } else {
                        len += 1;
                    }
                    new_rules.emplace_back(text[i - 1], text[i], len);

                    pairs[pair] = next_nt++;
                } else {
                    new_nt = (*found).second;
                }
                text[copy_i] = new_nt;

                pair_count++;
                i++;
                new_text_size--;
            } else if (copy) {
                text[copy_i] = text[i - 1];
            }
        }
        size_t size = rlslp.size();
        rlslp.resize(size + new_rules.size());
        for (size_t i = 0; i < new_rules.size(); ++i) {
            rlslp[size + i] = new_rules[i];
        }
        if (copy_i < new_text_size) {
            text[copy_i] = text[text.size() - 1];
        }
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count())
                  << " elements=" << pairs.size() << " pairs=" << pair_count;
        const auto startTimeIP = recomp::timer::now();
#endif

        bv.resize(rlslp.size(), false);
        text.resize(new_text_size);

#ifdef BENCH
        const auto endTimeIP = recomp::timer::now();
        const auto timeSpanIP = endTimeIP - startTimeIP;
        std::cout << " resize_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanIP).count());
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }
};

}  // namespace recomp
