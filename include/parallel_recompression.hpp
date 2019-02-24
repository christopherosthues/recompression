
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class parallel_recompression : public recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename recompression<variable_t, terminal_count_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t, terminal_count_t>::bv_t bv_t;
//    typedef std::tuple<variable_t, variable_t, bool> adj_t;
    typedef size_t adj_t;
    typedef std::vector<adj_t> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;

    inline parallel_recompression() {
        this->name = "parallel";
    }

    inline parallel_recompression(std::string& dataset) : recomp::recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "parallel";
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
     * @param cores The number of cores/threads to use
     */
    inline virtual void recomp(text_t& text,
                               rlslp<variable_t, terminal_count_t>& rlslp,
                               const terminal_count_t& alphabet_size,
                               const size_t cores) override {
#ifdef BENCH_RECOMP
        const auto startTime = recomp::timer::now();
#endif
        this->cores = cores;
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

        if (!text.empty()) {
            rlslp.root = static_cast<variable_t>(text[0]);
            rlslp.is_empty = false;
            this->rename_rlslp(rlslp, bv);
        }

#ifdef BENCH_RECOMP
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=" << this->name << "_recompression dataset=" << this->dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << this->level
                  << " cores=" << this->cores << std::endl;
#endif
    }

    using recompression<variable_t, terminal_count_t>::recomp;


 private:
    const variable_t DELETED = UINT_MAX;

    inline void compact(text_t& text,
                        const std::vector<size_t>& compact_bounds,
                        const std::vector<size_t>& copy_bounds,
                        size_t count) {
#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = copy_bounds[copy_bounds.size() - 1];
        if (new_text_size > 1 && count > 0) {
            text_t new_text;
            new_text.reserve(new_text_size);
            new_text.resize(new_text_size);

#pragma omp parallel num_threads(this->cores)
            {
                auto thread_id = omp_get_thread_num();
                size_t copy_i = copy_bounds[thread_id];
                for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                    if (text[i] != DELETED) {
                        new_text[copy_i++] = text[i];
                    }
                }
            }

            text = std::move(new_text);
        } else if (new_text_size == 1) {
            text.resize(new_text_size);
            text.shrink_to_fit();
        }
#ifdef BENCH
        const auto endTimeCompact = recomp::timer::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif
    }

    /**
     * @brief Replaces all block in the text with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    inline void bcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_bcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " cores=" << this->cores;
#endif

        size_t block_count = 0;

#ifdef BENCH
        const auto startTimeBlocks = recomp::timer::now();
#endif
        std::vector<position_t> positions;

        std::vector<size_t> bounds;
        std::vector<size_t> block_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> block_overlaps;
#pragma omp parallel num_threads(this->cores) reduction(+:block_count)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            variable_t block_len = 1;

#pragma omp single
            {
#ifdef BENCH
                std::cout << " used_cores=" << n_threads;
#endif
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                compact_bounds.reserve(n_threads + 1);
                compact_bounds.resize(n_threads + 1, text.size());
//                compact_bounds[n_threads] = text.size();
                block_counts.reserve(n_threads + 1);
                block_counts.resize(n_threads + 1, 0);
                block_overlaps.reserve(n_threads + 1);
                block_overlaps.resize(n_threads + 1, 0);
            }
            std::vector<position_t> t_positions;
            bool add = false;

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                compact_bounds[thread_id] = i;
                if (i == 0) {
                    add = thread_id == 0;
                } else {
                    add = text[i - 1] != text[i];
                }
                i = text.size();
            }

//            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
//                compact_bounds[thread_id] = text.size();
//            }
//
#pragma omp barrier
            size_t i = compact_bounds[thread_id];
            if (i > 0 && i < compact_bounds[thread_id + 1]) {
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
                    block_len++;
                    i++;
                }
                if (!add) {
                    block_overlaps[thread_id] = block_len;
                    block_len = 1;
                }
                if (block_len > 1) {
                    t_positions.emplace_back(block_len, i - block_len + 1);
                    block_count++;
                    block_counts[thread_id + 1] += block_len - 1;
                    block_len = 1;
                }
                i++;
            }

            for (; i < compact_bounds[thread_id + 1]; ++i) {
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
                    block_len++;
                    i++;
                }
                if (block_len > 1) {
                    t_positions.emplace_back(block_len, i - block_len + 1);
                    block_count++;
                    block_counts[thread_id + 1] += block_len - 1;
                    block_len = 1;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                for (size_t j = 1; j < n_threads + 1; ++j) {
                    bounds[j] += bounds[j - 1];
                    block_counts[j] += block_counts[j - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            block_counts[thread_id] = compact_bounds[thread_id] + block_overlaps[thread_id] - block_counts[thread_id];
//            if (compact_bounds[thread_id] > 0 && compact_bounds[thread_id] < text.size()) {
//                if (compact_bounds[thread_id] + block_overlaps[thread_id] > block_counts[thread_id]) {
//                    block_counts[thread_id] = compact_bounds[thread_id] + block_overlaps[thread_id] - block_counts[thread_id];
//                } else {
//                    block_counts[thread_id] = 0;
//                }
//            }
        }
        block_counts[block_counts.size() - 1] =
                compact_bounds[block_counts.size() - 1] + block_overlaps[block_counts.size() - 1] -
                block_counts[block_counts.size() - 1];
//        std::cout << std::endl << "overlaps: ";
//        for (size_t i = 0; i < block_overlaps.size(); ++i) {
//            std::cout << block_overlaps[i] << ", ";
//        }
//        std::cout << std::endl << "compact_bounds: ";
//        for (size_t i = 0; i < compact_bounds.size(); ++i) {
//            std::cout << compact_bounds[i] << ", ";
//        }
//        std::cout << std::endl << "block_counts: ";
//        for (size_t i = 0; i < block_counts.size(); ++i) {
//            std::cout << block_counts[i] << ", ";
//        }
//        std::cout << std::endl;
        block_overlaps.resize(0);
        block_overlaps.shrink_to_fit();
#ifdef BENCH
        const auto endTimeBlocks = recomp::timer::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());
#endif

        if (!positions.empty()) {
#ifdef BENCH
            const auto startTimeSort = recomp::timer::now();
#endif
            auto sort_cond = [&](const position_t& i, const position_t& j) {
                auto char_i = text[i.second];
                auto char_j = text[j.second];
                if (char_i == char_j) {
                    return i.first < j.first;
                } else {
                    return char_i < char_j;
                }
            };
            ips4o::parallel::sort(positions.begin(), positions.end(), sort_cond, this->cores);
#ifdef BENCH
            const auto endTimeSort = recomp::timer::now();
            const auto timeSpanSort = endTimeSort - startTimeSort;
            std::cout << " sort="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count());
#endif


#ifdef BENCH
            const auto startTimeRules = recomp::timer::now();
#endif
            auto nt_count = rlslp.non_terminals.size();
            auto next_nt = rlslp.terminals + nt_count;

            std::vector<size_t> assign_bounds;
            std::vector<size_t> distinct_blocks;
#pragma omp parallel num_threads(this->cores)
            {
                auto thread_id = omp_get_thread_num();
                auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
                {
                    assign_bounds.reserve(n_threads + 1);
                    assign_bounds.resize(n_threads + 1, positions.size());
//                    assign_bounds[n_threads] = positions.size();
                    distinct_blocks.reserve(n_threads + 1);
                    distinct_blocks.resize(n_threads + 1, 0);
                }

#pragma omp for schedule(static)
                for (size_t i = 0; i < positions.size(); ++i) {
                    assign_bounds[thread_id] = i;
                    i = positions.size();
                }

//                if (thread_id > 0 && assign_bounds[thread_id] == 0) {
//                    assign_bounds[thread_id] = positions.size();
//                }

                size_t i = assign_bounds[thread_id];
                if (i == 0) {
                    distinct_blocks[thread_id + 1]++;
                    i++;
                }

#pragma omp barrier
                for (; i < assign_bounds[thread_id + 1]; ++i) {
                    if (positions[i].first != positions[i - 1].first ||
                        text[positions[i].second] != text[positions[i - 1].second]) {
                        distinct_blocks[thread_id + 1]++;
                    }
                }

#pragma omp barrier
#pragma omp single
                {
                    for (size_t j = 1; j < distinct_blocks.size(); ++j) {
                        distinct_blocks[j] += distinct_blocks[j - 1];
                    }

                    auto bc = distinct_blocks[n_threads];
                    auto rlslp_size = nt_count + bc;
                    rlslp.reserve(rlslp_size);
                    rlslp.resize(rlslp_size);
                    rlslp.blocks += bc;
                    bv.resize(rlslp_size, true);

//                    std::cout << "bc: " << bc << std::endl;
//                    std::cout << "rlslp: " << rlslp_size << std::endl;
////                std::cout << "distinct pair finised" << std::endl;
//                    std::cout << std::endl << "distinct: ";
//                    for (size_t j = 0; j < distinct_blocks.size(); ++j) {
//                        std::cout << distinct_blocks[j] << ", ";
//                    }
//                    std::cout << std::endl << "assign_bounds: ";
//                    for (size_t j = 0; j < assign_bounds.size(); ++j) {
//                        std::cout << assign_bounds[j] << ", ";
//                    }
//                    std::cout << std::endl;
                }

                i = assign_bounds[thread_id];
                auto last_var = next_nt + distinct_blocks[thread_id] - 1;
                variable_t last_char = 0;
                if (i > 0 && i < assign_bounds[thread_id + 1]) {
                    last_char = text[positions[i - 1].second];
                }

#pragma omp barrier
                size_t j = 0;
                if (thread_id == 0) {
                    last_char = text[positions[i].second];
                    auto b_len = positions[i].first;
                    auto len = b_len;
                    if (last_char >= rlslp.terminals) {
                        len *= rlslp[last_char - rlslp.terminals].len;
                    }
                    rlslp[nt_count + distinct_blocks[thread_id] + j] = non_terminal<variable_t, terminal_count_t>(last_char, b_len, len);
                    j++;
                    last_var++;
                    text[positions[i].second] = last_var;
                    for (size_t k = 1; k < b_len; ++k) {
                        text[positions[i].second + k] = DELETED;
                    }
                    i++;
                }

                for (; i < assign_bounds[thread_id + 1]; ++i) {
                    auto char_i = text[positions[i].second];
                    auto b_len = positions[i].first;
                    if (char_i == last_char && b_len == positions[i - 1].first) {
                        text[positions[i].second] = last_var;
                    } else {
                        auto len = b_len;
                        if (char_i >= rlslp.terminals) {
                            len *= rlslp[char_i - rlslp.terminals].len;
                        }
                        rlslp[nt_count + distinct_blocks[thread_id] + j] = non_terminal<variable_t, terminal_count_t>(char_i, b_len, len);
                        j++;
                        last_var++;
                        text[positions[i].second] = last_var;
                        last_char = char_i;
                    }
                    for (size_t k = 1; k < b_len; ++k) {
                        text[positions[i].second + k] = DELETED;
                    }
                }
            }
#ifdef BENCH
            const auto endTimeRules = recomp::timer::now();
            const auto timeSpanRules = endTimeRules - startTimeRules;
            std::cout << " rules="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                      << " productions=" << distinct_blocks[distinct_blocks.size() - 1] << " elements=" << block_count;
#endif

            compact(text, compact_bounds, block_counts, block_count);
#ifdef BENCH
        } else {
            std::cout << " sort=0 rules=0 elements=0 blocks=0 compact_text=0";
#endif
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }

    /**
     * @brief Computes and sorts the adjacency list of the text in ascending order.
     *
     * The adjacency list is stored as a list of text positions.
     *
     * @param text The text
     * @param adj_list[out] The adjacency list (represented as text positions)
     */
    inline void compute_adj_list(const text_t& text, adj_list_t& adj_list) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            adj_list[i] = i;
//            if (text[i] > text[i + 1]) {
//                adj_list[i] = std::make_tuple(text[i], text[i + 1], false);
//            } else {
//                adj_list[i] = std::make_tuple(text[i + 1], text[i], true);
//            }
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif

#ifdef BENCH
        const auto startTimeMult = recomp::timer::now();
#endif
//        partitioned_radix_sort(adj_list);
//        ips4o::parallel::sort(adj_list.begin(), adj_list.end(), std::less<adj_t>(), cores);
        auto sort_adj = [&](size_t i, size_t j) {
            auto char_i = text[i];
            auto char_i1 = text[i + 1];
            auto char_j = text[j];
            auto char_j1 = text[j + 1];
            if (char_i > char_i1) {
                if (char_j > char_j1) {
                    bool less = char_i < char_j;
                    if (char_i == char_j) {
                        less = char_i1 < char_j1;
                    }
                    return less;
                } else {
                    bool less = char_i < char_j1;
                    if (char_i == char_j1) {
                        less = char_i1 < char_j;
                    }
                    return less;
                }
            } else {
                if (char_j > char_j1) {
                    bool less = char_i1 < char_j;
                    if (char_i1 == char_j) {
                        less = char_i < char_j1;
                    }
                    return less;
                } else {
                    bool less = char_i1 < char_j1;
                    if (char_i1 == char_j1) {
                        less = char_i < char_j;
                    }
                    return less;
                }
            }
        };
        ips4o::parallel::sort(adj_list.begin(), adj_list.end(), sort_adj, this->cores);
#ifdef BENCH
        const auto endTimeMult = recomp::timer::now();
        const auto timeSpanMult = endTimeMult - startTimeMult;
        std::cout << " sort_adj_list="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMult).count());
#endif
    }

    /**
     * @brief Computes a partitioning (Sigma_l, Sigma_r) of the symbols in the text.
     *
     * @param text[in] The text
     * @param adj_list[in] The adjacency list of the text (text positions)
     * @param partition[out] The partition
     * @param part_l[out] Indicates which partition set is the first one (@code{false} if symbol with value false
     *                    are in Sigma_l, otherwise all symbols with value true are in Sigma_l)
     */
    inline void compute_partition(const text_t& text,
                                  const adj_list_t& adj_list,
                                  partition_t& partition,
                                  bool& part_l) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        int l_count = 0;
        int r_count = 0;
        variable_t val = 0;
        if (!adj_list.empty()) {
            if (text[adj_list[0]] > text[adj_list[0] + 1]) {
                val = text[adj_list[0]];
                partition[text[adj_list[0] + 1]] = false;
//                if (partition.find(val) == partition.end()) {
//                    partition[val] = false;
//                }
//                if (!partition[val]) {
//                    l_count++;
//                } else {
//                    r_count++;
//                }
            } else {
                val = text[adj_list[0] + 1];
                partition[text[adj_list[0]]] = false;
//                if (partition.find(val) == partition.end()) {
//                    partition[val] = false;
//                }
//                if (!partition[val]) {
//                    l_count++;
//                } else {
//                    r_count++;
//                }
            }

            l_count++;
        }
        for (size_t i = 1; i < adj_list.size(); ++i) {
            auto text_i = text[adj_list[i]];
            auto text_i1 = text[adj_list[i] + 1];
            if (text_i > text_i1) {
                if (val < text_i) {
                    partition[val] = l_count > r_count;
                    l_count = 0;
                    r_count = 0;
                    val = text_i;
                }
                if (partition.find(text_i1) == partition.end()) {
                    partition[text_i1] = false;
                }
                if (partition[text_i1]) {
                    r_count++;
                } else {
                    l_count++;
                }
            } else {
                if (val < text_i1) {
                    partition[val] = l_count > r_count;
                    l_count = 0;
                    r_count = 0;
                    val = text_i1;
                }
                if (partition.find(text_i) == partition.end()) {
                    partition[text_i] = false;
                }
                if (partition[text_i]) {
                    r_count++;
                } else {
                    l_count++;
                }
            }
        }
        partition[val] = l_count > r_count;
//        if (!adj_list.empty()) {
//            if (partition[std::get<0>(adj_list[0])]) {
//                r_count++;
//            } else {
//                l_count++;
//            }
//        }
//        for (size_t i = 1; i < adj_list.size(); ++i) {
//            if (std::get<0>(adj_list[i - 1]) < std::get<0>(adj_list[i])) {
////            LOG(INFO) << "Setting " << std::get<0>(adj_list[i - 1]) << " to " << (l_count > r_count) << " ; "
////                      << l_count << ", " << r_count;
//                partition[std::get<0>(adj_list[i - 1])] = l_count > r_count;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(adj_list[i])]) {
//                r_count++;
//            } else {
//                l_count++;
//            }
//        }
//        partition[std::get<0>(adj_list[adj_list.size() - 1])] = l_count > r_count;

#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif

#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
#pragma omp parallel for num_threads(this->cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
        for (size_t i = 0; i < text.size() - 1; ++i) {
            if (!partition[text[i]] && partition[text[i + 1]]) {
                lr_count++;
            } else if (partition[text[i]] && !partition[text[i + 1]]) {
                rl_count++;
            }
        }
//        for (size_t i = 0; i < adj_list.size(); ++i) {
//            if (std::get<0>(adj_list[i])) {
//                if (!partition[std::get<1>(adj_list[i])] &&
//                    partition[std::get<2>(adj_list[i])]) {  // cb in text and c in right set and b in left
//                    rl_count++;
//                } else if (partition[std::get<1>(adj_list[i])] &&
//                           !partition[std::get<2>(adj_list[i])]) {  // cb in text and c in left set and b in right
//                    lr_count++;
//                }
//            } else {
//                if (!partition[std::get<1>(adj_list[i])] &&
//                    partition[std::get<2>(adj_list[i])]) {  // bc in text and b in left set and c in right
//                    lr_count++;
//                } else if (partition[std::get<1>(adj_list[i])] &&
//                           !partition[std::get<2>(adj_list[i])]) {  // bc in text and b in right set and c in left
//                    rl_count++;
//                }
//            }
//        }
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif

//#ifdef BENCH
//        const auto startTimeProds = recomp::timer::now();
//#endif
//        int distinct_l = 0;
//        int distinct_r = 0;
//#pragma omp parallel for num_threads(this->cores) schedule(static) reduction(+:distinct_l) reduction(+:distinct_r)
//        for (size_t i = 0; i < adj_list.size(); ++i) {
//
//        }
//#ifdef BENCH
//        const auto endTimeProds = recomp::timer::now();
//        const auto timeSpanProds = endTimeProds - startTimeProds;
//        std::cout << " dist_l=" << distinct_l << " dist_r=" << distinct_r << " distinct_time="
//                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanProds).count();
//#endif

        part_l = rl_count > lr_count;

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
    }


    /**
     * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    inline void pcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_pcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " cores=" << this->cores;
#endif
        partition_t partition;
        adj_list_t adj_list(text.size() - 1);
        compute_adj_list(text, adj_list);

        size_t pair_count = 0;
        bool part_l = false;
//        std::cout << std::endl << "Begin partition" << std::endl;
        compute_partition(text, adj_list, partition, part_l);
//        std::cout << std::endl << "fin part" << std::endl;

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
        std::cout << " alphabet=" << partition.size();
#endif
        std::vector<pair_position_t> positions;

        std::vector<size_t> bounds;
        std::vector<size_t> pair_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(this->cores) reduction(+:pair_count)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
#ifdef BENCH
                std::cout << " used_cores=" << n_threads;
#endif
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                size_t end_text = text.size() - 1;
                compact_bounds.reserve(n_threads + 1);
                compact_bounds.resize(n_threads + 1, end_text);
//                compact_bounds[n_threads] = text.size() - 1;
                pair_counts.reserve(n_threads + 1);
                pair_counts.resize(n_threads + 1, 0);
                pair_overlaps.reserve(n_threads + 1);
                pair_overlaps.resize(n_threads + 1, 0);
            }
            std::vector<pair_position_t> t_positions;

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                compact_bounds[thread_id] = i;
                i = text.size();
            }
//            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
//                compact_bounds[thread_id] = text.size() - 1;
//            }
//#pragma omp barrier

//#pragma omp single
//            {
//                std::cout << "compact bounds finished" << std::endl;
//            }

            size_t i = compact_bounds[thread_id];
#pragma omp barrier
            for (; i < compact_bounds[thread_id + 1]; ++i) {
                if (part_l == partition[text[i]] && part_l != partition[text[i + 1]]) {
                    t_positions.emplace_back(i);
                    pair_count++;
//                    pair_counts[thread_id + 1]++;
                }
            }
//#pragma omp single
//            {
//                std::cout << "find finished" << std::endl;
//            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                compact_bounds[n_threads] = text.size();
                for (size_t j = 1; j < n_threads + 1; ++j) {
                    bounds[j] += bounds[j - 1];
//                    pair_counts[j] += pair_counts[j - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);

//                std::cout << std::endl << "bounds: ";
//                for (size_t j = 0; j < pair_counts.size(); ++j) {
//                    std::cout << bounds[j] << ", ";
//                }
                pair_counts[n_threads] = compact_bounds[n_threads] + pair_overlaps[n_threads] - bounds[n_threads];
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

//#pragma omp single
//            {
//                std::cout << "copy finished" << std::endl;
//            }

            if (compact_bounds[thread_id] == text.size() - 1) {
                compact_bounds[thread_id] = text.size();
            }
//#pragma omp single
//            {
//                std::cout << "adjust compact finished" << std::endl;
//            }

            size_t cb = compact_bounds[thread_id];
//            if (cb > 0 && cb < text.size()) {
//#pragma omp critical
//                {std::cout << "Checking " << text[cb - 1] << " at " << (cb - 1) << " and " << text[cb] << " at " << (cb) << " by " << thread_id << std::endl;}
            if (cb > 0) {
                if (cb < text.size()) {
                    pair_overlaps[thread_id] = (partition[text[cb - 1]] == part_l && partition[text[cb]] != part_l) ? 1
                                                                                                                    : 0;
                }
                pair_counts[thread_id] = cb + pair_overlaps[thread_id] - bounds[thread_id];
//                if (cb + pair_overlaps[thread_id] > pair_counts[thread_id]) {
//                    pair_counts[thread_id] = cb + pair_overlaps[thread_id] - pair_counts[thread_id];
//                } else {
//                    pair_counts[thread_id] = 0;
//                }
            }
        }
//        std::cout << std::endl << "overlaps: ";
//        for (size_t i = 0; i < pair_overlaps.size(); ++i) {
//            std::cout << pair_overlaps[i] << ", ";
//        }
//        std::cout << std::endl << "compact_bounds: ";
//        for (size_t i = 0; i < compact_bounds.size(); ++i) {
//            std::cout << compact_bounds[i] << ", ";
//        }
//        std::cout << std::endl << "pair_counts: ";
//        for (size_t i = 0; i < pair_counts.size(); ++i) {
//            std::cout << pair_counts[i] << ", ";
//        }
//        std::cout << std::endl;
        pair_overlaps.resize(0);
        pair_overlaps.shrink_to_fit();
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());
#endif

#ifdef BENCH
        const auto startTimeSort = recomp::timer::now();
#endif
        auto sort_cond = [&](const pair_position_t& i, const pair_position_t& j) {
            auto char_i = text[i];
            auto char_i1 = text[i + 1];
            auto char_j = text[j];
            auto char_j1 = text[j + 1];
            if (char_i == char_j) {
                return char_i1 < char_j1;
            } else {
                return char_i < char_j;
            }
        };
        ips4o::parallel::sort(positions.begin(), positions.end(), sort_cond, this->cores);
//        parallel::partitioned_radix_sort(sort_pairs);
#ifdef BENCH
        const auto endTimeSort = recomp::timer::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count());
#endif
//        std::cout << std::endl;

#ifdef BENCH
        const auto startTimeRules = recomp::timer::now();
#endif
        auto nt_count = rlslp.non_terminals.size();
        auto next_nt = rlslp.terminals + nt_count;

        std::vector<size_t> assign_bounds;
        std::vector<size_t> distinct_pairs;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                assign_bounds.reserve(n_threads + 1);
                assign_bounds.resize(n_threads + 1, positions.size());
//                assign_bounds[n_threads] = positions.size();
                distinct_pairs.reserve(n_threads + 1);
                distinct_pairs.resize(n_threads + 1, 0);
//                std::cout << "create assign" << std::endl;
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < positions.size(); ++i) {
                assign_bounds[thread_id] = i;
                i = positions.size();
            }

//            if (thread_id > 0 && assign_bounds[thread_id] == 0) {
//                assign_bounds[thread_id] = positions.size();
//            }

            size_t i = assign_bounds[thread_id];
            if (i == 0) {
                distinct_pairs[thread_id + 1]++;
                i++;
            }

//#pragma omp single
//            {
//                std::cout << "assign bounds finished" << std::endl;
//            }

#pragma omp barrier
            for (; i < assign_bounds[thread_id + 1]; ++i) {
                if (text[positions[i]] != text[positions[i - 1]] ||
                    text[positions[i] + 1] != text[positions[i - 1] + 1]) {
                    distinct_pairs[thread_id + 1]++;
                }
            }

#pragma omp barrier
#pragma omp single
            {
//                std::cout << "distinct pair in sections finised" << std::endl;
                for (size_t j = 1; j < distinct_pairs.size(); ++j) {
                    distinct_pairs[j] += distinct_pairs[j - 1];
                }

                auto pc = distinct_pairs[n_threads];
                auto rlslp_size = nt_count + pc;
                rlslp.reserve(rlslp_size);
                rlslp.resize(rlslp_size);
                bv.resize(rlslp_size, false);

//                std::cout << "pc: " << pc << std::endl;
//                std::cout << "rlslp: " << rlslp_size << std::endl;
////                std::cout << "distinct pair finised" << std::endl;
//                std::cout << std::endl << "distinct: ";
//                for (size_t j = 0; j < distinct_pairs.size(); ++j) {
//                    std::cout << distinct_pairs[j] << ", ";
//                }
//                std::cout << std::endl << "assign_bounds: ";
//                for (size_t j = 0; j < assign_bounds.size(); ++j) {
//                    std::cout << assign_bounds[j] << ", ";
//                }
//                std::cout << std::endl;
            }

            i = assign_bounds[thread_id];
            auto last_var = next_nt + distinct_pairs[thread_id] - 1;
            variable_t last_char1 = 0;
            variable_t last_char2 = 0;
            if (i > 0 && i < assign_bounds[thread_id + 1]) {
                last_char1 = text[positions[i - 1]];
                last_char2 = text[positions[i - 1] + 1];
            }

#pragma omp barrier
            size_t j = 0;
            if (thread_id == 0) {
                last_char1 = text[positions[i]];
                last_char2 = text[positions[i] + 1];
                size_t len = 0;
                if (last_char1 >= rlslp.terminals) {
                    len = rlslp[last_char1 - rlslp.terminals].len;
                } else {
                    len = 1;
                }
                if (last_char2 >= rlslp.terminals) {
                    len += rlslp[last_char2 - rlslp.terminals].len;
                } else {
                    len += 1;
                }
                rlslp[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t, terminal_count_t>(last_char1, last_char2, len);
                j++;
                last_var++;
                text[positions[i]] = last_var;
                text[positions[i] + 1] = DELETED;
                i++;
            }

            for (; i < assign_bounds[thread_id + 1]; ++i) {
                auto char_i1 = text[positions[i]];
                auto char_i2 = text[positions[i] + 1];
                if (char_i1 == last_char1 && char_i2 == last_char2) {
                    text[positions[i]] = last_var;
                } else {
                    size_t len = 0;
                    if (char_i1 >= rlslp.terminals) {
                        len = rlslp[char_i1 - rlslp.terminals].len;
                    } else {
                        len = 1;
                    }
                    if (char_i2 >= rlslp.terminals) {
                        len += rlslp[char_i2 - rlslp.terminals].len;
                    } else {
                        len += 1;
                    }
                    rlslp[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t, terminal_count_t>(char_i1, char_i2, len);
                    j++;
                    last_var++;
                    text[positions[i]] = last_var;
                    last_char1 = char_i1;
                    last_char2 = char_i2;
                }
                text[positions[i] + 1] = DELETED;
            }
        }
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                  << " productions=" << distinct_pairs[distinct_pairs.size() - 1] << " elements=" << pair_count;
#endif

        compact(text, compact_bounds, pair_counts, pair_count);
//#ifdef BENCH
//        const auto startTimeCompact = recomp::timer::now();
//#endif
//        size_t new_text_size = pair_counts[pair_counts.size() - 1];  // text.size() - pair_counts[pair_counts.size() - 1];
//        if (new_text_size > 1 && pair_count > 0) {
//            text_t new_text;
//            new_text.reserve(new_text_size);
//            new_text.resize(new_text_size);
//
//#pragma omp parallel num_threads(this->cores)
//            {
//                auto thread_id = omp_get_thread_num();
//                size_t copy_i = pair_counts[thread_id];
//                for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
//                    if (text[i] != DELETED) {
//                        new_text[copy_i++] = text[i];
//                    }
//                }
//            }
//
//            text = std::move(new_text);
//        } else if (new_text_size == 1) {
//            text.resize(new_text_size);
//            text.shrink_to_fit();
//        }
//#ifdef BENCH
//        const auto endTimeCompact = recomp::timer::now();
//        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
//        std::cout << " compact_text="
//                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
//#endif

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }
};

}  // namespace parallel
}  // namespace recomp
