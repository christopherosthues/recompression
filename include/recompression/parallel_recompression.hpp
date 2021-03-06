
#pragma once

#include <omp.h>

#ifdef BENCH
#include <iostream>
#endif

#include <algorithm>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

#include "graph.hpp"

namespace recomp {

namespace parallel {

/**
 * @brief This class is a parallel implementation of the recompression computing a sequential undirected maximum cut.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class parallel_recompression : public recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename recompression<variable_t>::bv_t bv_t;
    typedef size_t adj_t;
    typedef ui_vector<adj_t> adj_list_t;
    typedef ui_vector<bool> partition_t;

    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;

    inline parallel_recompression() {
        this->name = "parallel";
    }

    inline parallel_recompression(std::string& dataset) : recomp::recompression<variable_t>(dataset) {
        this->name = "parallel";
    }

    using recompression<variable_t>::recomp;

    inline virtual void recomp(text_t& text,
                               rlslp<variable_t>& rlslp,
                               const size_t& alphabet_size,
                               const size_t cores) override {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        size_t text_size = text.size();
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
                  << " cores=" << this->cores << " size=" << text_size << std::endl;
#endif
    }


 protected:
    const variable_t DELETED = std::numeric_limits<variable_t>::max();

    /**
     * @brief Compacts the text
     *
     * @param text[in,out] The text
     * @param compact_bounds[in] The bounds for the cores to copy from
     * @param copy_bounds[in] The bounds for the cores to copy to
     * @param count[in] The number of found blocks/pairs
     */
    inline void compact(text_t& text,
                        const ui_vector<size_t>& compact_bounds,
                        const ui_vector<size_t>& copy_bounds,
                        size_t count) {
#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = copy_bounds[copy_bounds.size() - 1];
        if (new_text_size > 1 && count > 0) {
#ifdef BENCH
            const auto startTimeNewText = recomp::timer::now();
#endif
            text_t new_text(new_text_size);
#ifdef BENCH
            const auto endTimeNT = recomp::timer::now();
            const auto timeSpanNT = endTimeNT - startTimeNewText;
            std::cout << " init_new_text="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanNT).count());
            const auto startTimeCopy = recomp::timer::now();
#endif

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
#ifdef BENCH
            const auto endTimeCopy = recomp::timer::now();
            const auto timeSpanCopy = endTimeCopy - startTimeCopy;
            std::cout << " copy="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
            const auto startTimeMove = recomp::timer::now();
#endif

            std::swap(text, new_text);
#ifdef BENCH
            const auto endTimeMove = recomp::timer::now();
            const auto timeSpanMove = endTimeMove - startTimeMove;
            std::cout << " move="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMove).count());
#endif
        } else if (new_text_size == 1) {
            text.resize(new_text_size);
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
     * @param bv[in,out] The bitvector to indicate which rules derive blocks
     */
    inline void bcomp(text_t& text, rlslp<variable_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_bcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " cores=" << this->cores;
        const auto startTimeBlocks = recomp::timer::now();
#endif
        size_t block_count = 0;
        ui_vector<position_t> positions;

        ui_vector<size_t> bounds;
        ui_vector<size_t> block_counts;
        ui_vector<size_t> compact_bounds;
        ui_vector<size_t> block_overlaps;
#pragma omp parallel num_threads(this->cores) reduction(+:block_count)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            variable_t block_len = 1;

#pragma omp single
            {
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                compact_bounds.resize(n_threads + 1);
                compact_bounds[n_threads] = text.size();
                block_counts.resize(n_threads + 1);
                block_counts[n_threads] = 0;
                block_overlaps.resize(n_threads + 1);
                block_overlaps[n_threads] = 0;
            }
            compact_bounds[thread_id] = text.size();
            block_counts[thread_id] = 0;
            block_overlaps[thread_id] = 0;
            std::deque<position_t> t_positions;
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
        }
        block_counts[block_counts.size() - 1] =
                compact_bounds[block_counts.size() - 1] + block_overlaps[block_counts.size() - 1] -
                block_counts[block_counts.size() - 1];
        block_overlaps.resize(1);
#ifdef BENCH
        const auto endTimeBlocks = recomp::timer::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());
#endif

        if (positions.size() > 0) {
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
            const auto startTimeRules = recomp::timer::now();
#endif
            auto nt_count = rlslp.non_terminals.size();
            auto next_nt = rlslp.terminals + nt_count;

            ui_vector<size_t> assign_bounds;
            ui_vector<size_t> distinct_blocks;
            typename recomp::rlslp<variable_t>::production_t productions;
#pragma omp parallel num_threads(this->cores)
            {
                auto thread_id = omp_get_thread_num();
                auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
                {
                    assign_bounds.resize(n_threads + 1);
                    assign_bounds[n_threads] = positions.size();
                    distinct_blocks.resize(n_threads + 1);
                    distinct_blocks[0] = 0;
                }
                assign_bounds[thread_id] = positions.size();
                distinct_blocks[thread_id + 1] = 0;

#pragma omp for schedule(static)
                for (size_t i = 0; i < positions.size(); ++i) {
                    assign_bounds[thread_id] = i;
                    i = positions.size();
                }

                size_t i = assign_bounds[thread_id];
                if (i == 0) {
                    distinct_blocks[thread_id + 1]++;
                    i++;
                }

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
                    productions.resize(rlslp_size);
                    rlslp.blocks += bc;
                    bv.resize(rlslp_size, true);
                }

#pragma omp for schedule(static)
                for (size_t k = 0; k < rlslp.size(); ++k) {
                    productions[k] = rlslp[k];
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
                    productions[nt_count + distinct_blocks[thread_id] + j] = non_terminal<variable_t>(last_char, b_len,
                                                                                                      len);
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
                        productions[nt_count + distinct_blocks[thread_id] + j] = non_terminal<variable_t>(char_i, b_len,
                                                                                                          len);
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
            std::swap(rlslp.non_terminals, productions);
            productions.resize(1);
            positions.resize(1);
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
                std::cout << " sort=0 rules=0 elements=0 blocks=0 init_new_text=0 copy=0 move=0 compact_text=0";
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
     * @param text[in] The text
     * @param adj_list[out] The adjacency list (represented as text positions)
     */
    virtual void compute_adj_list(const text_t& text, adj_list_t& adj_list) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            adj_list[i] = i;
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        const auto startTimeMult = recomp::timer::now();
#endif
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
     * @brief Computes a directed maximum cut based on the given undirected cut represented by the partition.
     *
     * @param text[in] The text
     * @param partition[in,out] The partition
     * @param adj_list[in] The adjacency list
     * @param part_l[out] Indicates which partition set is the first one (@code{false} if symbol with value false
     *                    are in Sigma_l, otherwise all symbols with value true are in Sigma_l)
     * @param minimum[in] The smallest symbol in the text
     */
    virtual void directed_cut(const text_t& text,
                              partition_t& partition,
                              adj_list_t& adj_list,
                              bool& part_l,
                              variable_t minimum) {
#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
#endif
        adj_list.resize(1);

        int lr_count = 0;
        int rl_count = 0;
#pragma omp parallel for num_threads(this->cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
        for (size_t i = 0; i < text.size() - 1; ++i) {
            if (!partition[text[i] - minimum] && partition[text[i + 1] - minimum]) {
                lr_count++;
            } else if (partition[text[i] - minimum] && !partition[text[i + 1] - minimum]) {
                rl_count++;
            }
        }
        part_l = rl_count > lr_count;
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif
    }

    /**
     * @brief Computes a partitioning (Sigma_l, Sigma_r) of the symbols in the text.
     *
     * @param text[in] The text
     * @param partition[out] The partition
     * @param part_l[out] Indicates which partition set is the first one (@code{false} if symbol with value false
     *                    are in Sigma_l, otherwise all symbols with value true are in Sigma_l)
     * @param minimum[in] The smallest symbol in the text
     */
    inline void compute_partition(const text_t& text, partition_t& partition, bool& part_l, variable_t minimum) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        adj_list_t adj_list(text.size() - 1);
#ifdef BENCH
        const auto endTimeAdjInit = recomp::timer::now();
        const auto timeSpanAdjInit = endTimeAdjInit - startTime;
        std::cout << " init_adj_vec=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAdjInit).count();
#endif
        compute_adj_list(text, adj_list);
#ifdef BENCH
        const auto startTimeInitPar = recomp::timer::now();
#endif
#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < partition.size(); ++i) {
            partition[i] = false;
        }
#ifdef BENCH
        const auto endTimeInitPar = recomp::timer::now();
        const auto timeSpanInitPar = endTimeInitPar - startTimeInitPar;
        std::cout << " init_partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanInitPar).count();
        const auto startTimePar = recomp::timer::now();
#endif
        int l_count = 0;
        int r_count = 0;
        variable_t val = 0;
        if (text[adj_list[0]] > text[adj_list[0] + 1]) {
            val = text[adj_list[0]];
            partition[text[adj_list[0] + 1] - minimum] = false;
        } else {
            val = text[adj_list[0] + 1];
            partition[text[adj_list[0]] - minimum] = false;
        }

        l_count++;
        for (size_t i = 1; i < adj_list.size(); ++i) {
            auto text_i = text[adj_list[i]];
            auto text_i1 = text[adj_list[i] + 1];
            if (text_i1 > text_i) {
                std::swap(text_i, text_i1);
            }

            if (val < text_i) {
                partition[val - minimum] = l_count > r_count;
                l_count = 0;
                r_count = 0;
                val = text_i;
            }
            if (partition[text_i1 - minimum]) {
                r_count++;
            } else {
                l_count++;
            }
        }
        partition[val - minimum] = l_count > r_count;

#ifdef GRAPH_STATS
        compute_graph_stats(text, adj_list);
#endif

#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTimePar;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
        directed_cut(text, partition, adj_list, part_l, minimum);

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
     * @param bv[in,out] The bitvector to indicate which rules derive blocks
     */
    inline void pcomp(text_t& text, rlslp<variable_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_pcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " cores=" << this->cores;
#endif
        variable_t minimum = std::numeric_limits<variable_t>::max();
#pragma omp parallel for schedule(static) num_threads(this->cores) reduction(min:minimum)
        for (size_t i = 0; i < text.size(); ++i) {
            minimum = std::min(minimum, text[i]);
        }
#ifdef BENCH
        const auto endTimeInAl = recomp::timer::now();
        const auto timeSpanInAl = endTimeInAl - startTime;
        std::cout << " minimum=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanInAl).count();
#endif

        const auto max_letters = rlslp.size() + rlslp.terminals - minimum;
        partition_t partition(max_letters);

        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(text, partition, part_l, minimum);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
        std::cout << " alphabet=" << partition.size();
#endif
        ui_vector<pair_position_t> positions;

        ui_vector<size_t> bounds;
        ui_vector<size_t> pair_counts;
        ui_vector<size_t> compact_bounds;
        ui_vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(this->cores) reduction(+:pair_count)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            size_t end_text = text.size() - 1;

#pragma omp single
            {
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                compact_bounds.resize(n_threads + 1);
                compact_bounds[n_threads] = end_text;
                pair_counts.resize(n_threads + 1);
                pair_counts[n_threads] = 0;
                pair_overlaps.resize(n_threads + 1);
                pair_overlaps[n_threads] = 0;
            }
            compact_bounds[thread_id] = end_text;
            pair_counts[thread_id] = 0;
            pair_overlaps[thread_id] = 0;
            std::deque<pair_position_t> t_positions;

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                compact_bounds[thread_id] = i;
                i = text.size();
            }

            for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                if (part_l == partition[text[i] - minimum] && part_l != partition[text[i + 1] - minimum]) {
                    t_positions.emplace_back(i);
                    pair_count++;
                }
            }
            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                compact_bounds[n_threads] = text.size();
                for (size_t j = 1; j < n_threads + 1; ++j) {
                    bounds[j] += bounds[j - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);

                pair_counts[n_threads] = compact_bounds[n_threads] + pair_overlaps[n_threads] - bounds[n_threads];
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            if (compact_bounds[thread_id] == text.size() - 1) {
                compact_bounds[thread_id] = text.size();
            }

            size_t cb = compact_bounds[thread_id];
            if (cb > 0) {
                if (cb < text.size()) {
                    pair_overlaps[thread_id] = (partition[text[cb - 1] - minimum] == part_l &&
                                                partition[text[cb] - minimum] != part_l) ? 1
                                                                                         : 0;
                }
                pair_counts[thread_id] = cb + pair_overlaps[thread_id] - bounds[thread_id];
            }
        }
        partition.resize(1);
        pair_overlaps.resize(1);
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());
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
#ifdef BENCH
        const auto endTimeSort = recomp::timer::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count());
        const auto startTimeRules = recomp::timer::now();
#endif
        auto nt_count = rlslp.non_terminals.size();
        auto next_nt = rlslp.terminals + nt_count;

        ui_vector<size_t> assign_bounds;
        ui_vector<size_t> distinct_pairs;
        typename recomp::rlslp<variable_t>::production_t productions;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                assign_bounds.resize(n_threads + 1);
                assign_bounds[n_threads] = positions.size();
                distinct_pairs.resize(n_threads + 1);
                distinct_pairs[0] = 0;
            }
            assign_bounds[thread_id] = positions.size();
            distinct_pairs[thread_id + 1] = 0;

#pragma omp for schedule(static)
            for (size_t i = 0; i < positions.size(); ++i) {
                assign_bounds[thread_id] = i;
                i = positions.size();
            }

            size_t i = assign_bounds[thread_id];
            if (i == 0) {
                distinct_pairs[thread_id + 1]++;
                i++;
            }

            for (; i < assign_bounds[thread_id + 1]; ++i) {
                if (text[positions[i]] != text[positions[i - 1]] ||
                    text[positions[i] + 1] != text[positions[i - 1] + 1]) {
                    distinct_pairs[thread_id + 1]++;
                }
            }

#pragma omp barrier
#pragma omp single
            {
                for (size_t j = 1; j < distinct_pairs.size(); ++j) {
                    distinct_pairs[j] += distinct_pairs[j - 1];
                }

                auto pc = distinct_pairs[n_threads];
                auto rlslp_size = nt_count + pc;
                productions.resize(rlslp_size);
                bv.resize(rlslp_size, false);
            }
#pragma omp for schedule(static)
            for (size_t k = 0; k < rlslp.size(); ++k) {
                productions[k] = rlslp[k];
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
                productions[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(last_char1, last_char2,
                                                                                                 len);
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
                    productions[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(char_i1, char_i2,
                                                                                                     len);
                    j++;
                    last_var++;
                    text[positions[i]] = last_var;
                    last_char1 = char_i1;
                    last_char2 = char_i2;
                }
                text[positions[i] + 1] = DELETED;
            }
        }
        std::swap(rlslp.non_terminals, productions);
        productions.resize(1);
        positions.resize(1);
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                  << " productions=" << distinct_pairs[distinct_pairs.size() - 1] << " elements=" << pair_count;
#endif

        compact(text, compact_bounds, pair_counts, pair_count);

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }

    inline void compute_graph_stats(const text_t& text, adj_list_t& adj_list) {
        graph<variable_t> g{adj_list, text};
        g.density();
        g.components();
        g.bicomponents();
    }
};

}  // namespace parallel
}  // namespace recomp
