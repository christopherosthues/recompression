
#pragma once

#include <omp.h>

#ifdef BENCH
#include <iostream>
#endif

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "parallel_recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class parallel_rnd_recompression : public parallel_recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename recompression<variable_t, terminal_count_t>::bv_t bv_t;
    typedef typename parallel_recompression<variable_t, terminal_count_t>::adj_t adj_t;
    typedef typename parallel_recompression<variable_t, terminal_count_t>::adj_list_t adj_list_t;
    typedef ui_vector<std::uint8_t> partition_t;
    typedef size_t pair_position_t;

    inline parallel_rnd_recompression() {
        this->name = "parallel_rnd";
    }

    inline parallel_rnd_recompression(std::string& dataset) : parallel_recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "parallel_rnd";
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
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        size_t text_size = text.size();
#endif
        this->cores = cores;
        rlslp.terminals = alphabet_size;
        bv_t bv;

        while (text.size() > 1) {
            this->bcomp(text, rlslp, bv);
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

    using recompression<variable_t, terminal_count_t>::recomp;


 private:
    const variable_t DELETED = std::numeric_limits<variable_t>::max();

    inline void compact(text_t& text,
                        const std::vector<size_t>& compact_bounds,
                        const std::vector<size_t>& copy_bounds,
                        size_t count,
                        const ui_vector<variable_t>& mapping) {
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
                        if (text[i] >= mapping.size()) {
                            new_text[copy_i++] = text[i];
                        } else {
                            new_text[copy_i++] = mapping[text[i]];
                        }
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

            text = std::move(new_text);
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
     * @brief
     *
     * @param text
     * @param mapping
     */
    inline void compute_mapping(text_t& text,
                                rlslp<variable_t, terminal_count_t>& rlslp,
                                ui_vector<variable_t>& mapping) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

        std::vector<variable_t> mins;
#pragma omp parallel num_threads(this->cores)
        {
            auto n_threads = (size_t)omp_get_num_threads();
            auto thread_id = (size_t)omp_get_thread_num();
#pragma omp single
            {
                mins.resize(n_threads, std::numeric_limits<variable_t>::max());
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                if (text[i] < mins[thread_id]) {
                    mins[thread_id] = text[i];
                }
            }
        }
        variable_t min = std::numeric_limits<variable_t>::max();
        for (size_t i = 0; i < mins.size(); ++i) {
            if (mins[i] < min) {
                min = mins[i];
            }
        }
#ifdef BENCH
        const auto endTimeInAl = recomp::timer::now();
        const auto timeSpanInAl = endTimeInAl - startTime;
        std::cout << " minimum=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanInAl).count();
        const auto startTimeAlpha = recomp::timer::now();
#endif

        const auto max_letters = rlslp.size() + rlslp.terminals - min;
        ui_vector<variable_t> global_hist(max_letters);
        global_hist[0] = 0;
        i_vector<ui_vector<variable_t>> hists;
#pragma omp parallel num_threads(this->cores)
        {
            auto n_threads = (size_t)omp_get_num_threads();
            auto thread_id = (size_t)omp_get_thread_num();
#pragma omp single
            {
                hists.resize(n_threads);
                for (size_t i = 0; i < n_threads; ++i) {
                    hists[i].resize(max_letters);
                }
            }

            hists[thread_id].fill(0);

#pragma omp barrier
#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                hists[thread_id][text[i] - min] = 1;
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < global_hist.size(); ++i) {
                global_hist[i] = hists[0][i];
                for (size_t j = 1; j < n_threads; ++j) {
                    global_hist[i] |= hists[j][i];
                }
            }

#pragma omp single
            {
                for (size_t i = 0; i < n_threads; ++i) {
                    hists[i].resize(0);
                }
                hists.resize(0);
                mapping.resize(max_letters);
                size_t j = 0;
                if (global_hist[0] > 0) {
                    mapping[j++] = min;
                }
                for (size_t i = 1; i < global_hist.size(); ++i) {
                    if (global_hist[i] > 0) {
                        mapping[j++] = i + min;
                    }
                    global_hist[i] += global_hist[i - 1];
                }
                mapping.resize(j);
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                text[i] = global_hist[text[i] - min] - 1;
            }
        }
#ifdef BENCH
        const auto endAlphaTime = recomp::timer::now();
        const auto timeSpanAlpha = endAlphaTime - startTimeAlpha;
        std::cout << " mapping=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAlpha).count();
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
    inline void compute_partition(const text_t& text, partition_t& partition, bool& part_l) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        adj_list_t adj_list(text.size() - 1);
        this->compute_adj_list(text, adj_list);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> distribution(0, 1);
        partition[0] = 0;  // ensure, that minimum one symbol is in the left partition and one in the right
        partition[partition.size() - 1] = 1;
#pragma omp parallel num_threads(this->cores)
        {
#pragma omp for schedule(static)
            for (size_t i = 1; i < partition.size() - 1; ++i) {
                partition[i] = distribution(gen);
            }
        }

#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
        const auto startTimeCount = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
        std::vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, adj_list.size());
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < adj_list.size(); ++i) {
                bounds[thread_id] = i;
                i = adj_list.size();
            }

            variable_t last_i = 0;  // avoid more random access than necessary
            variable_t last_i1 = 0;
            size_t i = bounds[thread_id];
            if (i == 0) {
                last_i = text[adj_list[i]];
                last_i1 = text[adj_list[i] + 1];
                if (!partition[last_i] && partition[last_i1]) {
                    lr_count++;
                    prod_l++;
                } else if (partition[last_i] && !partition[last_i1]) {
                    rl_count++;
                    prod_r++;
                }
                i++;
            } else if (i < adj_list.size()) {
                last_i = text[adj_list[i - 1]];
                last_i1 = text[adj_list[i - 1] + 1];
            }

            for (; i < bounds[thread_id + 1]; ++i) {
                variable_t char_i = text[adj_list[i]];
                variable_t char_i1 = text[adj_list[i] + 1];
                if (!partition[char_i] && partition[char_i1]) {
                    lr_count++;
                    if (char_i != last_i || char_i1 != last_i1) {
                        prod_l++;
                    }
                } else if (partition[char_i] && !partition[char_i1]) {
                    rl_count++;
                    if (char_i != last_i || char_i1 != last_i1) {
                        prod_r++;
                    }
                }
                last_i = char_i;
                last_i1 = char_i1;
            }
        }
        part_l = rl_count > lr_count;
        if (rl_count == lr_count) {
            part_l = prod_r < prod_l;
        }
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
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
        ui_vector<variable_t> mapping;
        compute_mapping(text, rlslp, mapping);

        partition_t partition(mapping.size());

        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(text, partition, part_l);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
        std::cout << " alphabet=" << partition.size();
#endif
        ui_vector<pair_position_t> positions;

        ui_vector<size_t> bounds;
        std::vector<size_t> pair_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(this->cores) reduction(+:pair_count)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                size_t end_text = text.size() - 1;
                compact_bounds.reserve(n_threads + 1);
                compact_bounds.resize(n_threads + 1, end_text);
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

            size_t i = compact_bounds[thread_id];
#pragma omp barrier
            for (; i < compact_bounds[thread_id + 1]; ++i) {
                if (part_l == partition[text[i]] && part_l != partition[text[i + 1]]) {
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
                    pair_overlaps[thread_id] = (partition[text[cb - 1]] == part_l && partition[text[cb]] != part_l) ? 1
                                                                                                                    : 0;
                }
                pair_counts[thread_id] = cb + pair_overlaps[thread_id] - bounds[thread_id];
            }
        }
        {
            auto discard = std::move(partition);
        }
        pair_overlaps.resize(0);
        pair_overlaps.shrink_to_fit();
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
//        parallel::partitioned_radix_sort(sort_pairs);
#ifdef BENCH
        const auto endTimeSort = recomp::timer::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count());
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
                distinct_pairs.reserve(n_threads + 1);
                distinct_pairs.resize(n_threads + 1, 0);
            }

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
                for (size_t j = 1; j < distinct_pairs.size(); ++j) {
                    distinct_pairs[j] += distinct_pairs[j - 1];
                }

                auto pc = distinct_pairs[n_threads];
                auto rlslp_size = nt_count + pc;
                rlslp.reserve(rlslp_size);
                rlslp.resize(rlslp_size);
                bv.resize(rlslp_size, false);
            }

            i = assign_bounds[thread_id];
            auto last_var = next_nt + distinct_pairs[thread_id] - 1;
            variable_t last_char1 = 0;
            variable_t last_char2 = 0;
            if (i > 0 && i < assign_bounds[thread_id + 1]) {
                last_char1 = mapping[text[positions[i - 1]]];
                last_char2 = mapping[text[positions[i - 1] + 1]];
            }

#pragma omp barrier
            size_t j = 0;
            if (thread_id == 0) {
                last_char1 = mapping[text[positions[i]]];
                last_char2 = mapping[text[positions[i] + 1]];
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
                auto char_i1 = mapping[text[positions[i]]];
                auto char_i2 = mapping[text[positions[i] + 1]];
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
        positions.resize(0);
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                  << " productions=" << distinct_pairs[distinct_pairs.size() - 1] << " elements=" << pair_count;
#endif

        compact(text, compact_bounds, pair_counts, pair_count, mapping);

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
