
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <tuple>
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
class full_parallel_recompression : public recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename recompression<variable_t, terminal_count_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t, terminal_count_t>::bv_t bv_t;
    typedef size_t adj_t;
    typedef std::vector<adj_t> adj_list_t;
    typedef std::vector<std::uint8_t> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;

    inline full_parallel_recompression() {
        this->name = "full_parallel";
    }

    inline full_parallel_recompression(std::string& dataset) : recomp::recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "full_parallel";
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

    inline void compact(text_t& text,
                        const std::vector<size_t>& compact_bounds,
                        const std::vector<size_t>& copy_bounds,
                        size_t count,
                        const std::vector<variable_t>& mapping) {
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
                        if (text[i] >= mapping.size()) {
                            new_text[copy_i++] = text[i];
                        } else {
                            new_text[copy_i++] = mapping[text[i]];
                        }
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
        }
        block_counts[block_counts.size() - 1] =
                compact_bounds[block_counts.size() - 1] + block_overlaps[block_counts.size() - 1] -
                block_counts[block_counts.size() - 1];
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
                    distinct_blocks.reserve(n_threads + 1);
                    distinct_blocks.resize(n_threads + 1, 0);
                }

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
            positions.resize(0);
            positions.shrink_to_fit();
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
     *
     * @param text
     * @param mapping
     */
    inline void compute_mapping(text_t& text, std::vector<variable_t>& mapping) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        std::vector<size_t> alpha(text.size());
#pragma omp parallel for num_threads(this->cores) schedule(static)
        for (size_t i = 0; i < text.size(); ++i) {
            alpha[i] = i;
        }
        auto sort_alpha = [&](size_t i, size_t j) {
            return text[i] < text[j];
        };
#ifdef BENCH
        const auto endAlphaTime = recomp::timer::now();
        const auto timeSpanAlpha = endAlphaTime - startTime;
        std::cout << " alpha=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAlpha).count();
        const auto startAlphaSortTime = recomp::timer::now();
#endif
        ips4o::parallel::sort(alpha.begin(), alpha.end(), sort_alpha, this->cores);
#ifdef BENCH
        const auto endAlphaSortTime = recomp::timer::now();
        const auto timeSpanAlphaSort = endAlphaSortTime - startAlphaSortTime;
        std::cout << " sort_alpha=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAlphaSort).count();
        const auto startMapTime = recomp::timer::now();
#endif

        std::vector<size_t> alpha_counts;
        std::vector<size_t> alpha_bounds;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                alpha_counts.reserve(n_threads + 1);
                alpha_counts.resize(n_threads + 1, 0);
                alpha_bounds.reserve(n_threads + 1);
                alpha_bounds.resize(n_threads + 1, alpha.size());
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < alpha.size(); ++i) {
                alpha_bounds[thread_id] = i;
                i = alpha.size();
            }

            size_t i = alpha_bounds[thread_id];
            if (i == 0) {
                alpha_counts[thread_id + 1]++;
                i++;
            }

            for (; i < alpha_bounds[thread_id + 1]; ++i) {
                if (text[alpha[i]] != text[alpha[i - 1]]) {
                    alpha_counts[thread_id + 1]++;
                }
            }

            i = alpha_bounds[thread_id];

#pragma omp barrier
#pragma omp single
            {
                for (size_t j = 1; j < alpha_counts.size(); ++j) {
                    alpha_counts[j] += alpha_counts[j - 1];
                }
                mapping.reserve(alpha_counts[n_threads]);
                mapping.resize(alpha_counts[n_threads]);
            }

            size_t j = alpha_counts[thread_id];
            variable_t old_val = 0;
            if (i > 0 && i < alpha_bounds[thread_id + 1]) {
                old_val = text[alpha[i - 1]];
            }

#pragma omp barrier
            if (i == 0) {
                old_val = text[alpha[i]];
                mapping[j] = old_val;
                text[alpha[i]] = j++;
                i++;
            }

            for (; i < alpha_bounds[thread_id + 1]; ++i) {
                variable_t val = text[alpha[i]];
                if (val != old_val) {
                    mapping[j] = val;
                    j++;
                }
                old_val = val;
                text[alpha[i]] = j - 1;
            }
        }
//        alpha.resize(0);
//        alpha.shrink_to_fit();
//        alpha_bounds.resize(0);
//        alpha_bounds.shrink_to_fit();
//        alpha_counts.resize(0);
//        alpha_counts.shrink_to_fit();
#ifdef BENCH
        const auto endMappingTime = recomp::timer::now();
        const auto timeSpanMapping = endMappingTime - startTime;
        const auto timeSpanMap = endMappingTime - startMapTime;
        std::cout << " mapping=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMap).count()
                  << " compute_mapping=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMapping).count();
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
                    // char_j1 > char_j -> (char_i, char_i1, 0) (char_j1, char_j, 1)
                    bool less = char_i < char_j1;
                    if (char_i == char_j1) {
                        if (char_i1 == char_j) {
                            return true;
                        }
                        less = char_i1 < char_j;
                    }
                    return less;
                }
            } else {
                if (char_j > char_j1) {
                    // char_i1 > char_i -> (char_i1, char_i, 1) (char_j, char_j1, 0)
                    bool less = char_i1 < char_j;
                    if (char_i1 == char_j) {
                        if (char_i == char_j1) {
                            return false;
                        }
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
     * @brief Computes and sorts the adjacency list of the text in ascending order.
     *
     * The adjacency list is stored as a list of text positions.
     *
     * @param text The text
     * @param adj_list[out] The adjacency list (represented as text positions)
     */
    inline void compute_adj_list_reverse(const text_t& text, adj_list_t& adj_list) {
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
            if (char_i < char_i1) {
                if (char_j < char_j1) {
                    bool lt = char_i < char_j;
                    if (char_i == char_j) {
                        lt = char_i1 < char_j1;
                    }
                    return lt;
                } else {
                    // char_j1 < char_j -> (char_i, char_i1, 0) (char_j1, char_j, 1)
                    bool greater = char_i < char_j1;
                    if (char_i == char_j1) {
                        if (char_i1 == char_j) {
                            return true;
                        }
                        greater = char_i1 < char_j;
                    }
                    return greater;
                }
            } else {
                if (char_j < char_j1) {
                    // char_i1 > char_i -> (char_i1, char_i, 1) (char_j, char_j1, 0)
                    bool greater = char_i1 < char_j;
                    if (char_i1 == char_j) {
                        if (char_i == char_j1) {
                            return false;
                        }
                        greater = char_i < char_j1;
                    }
                    return greater;
                } else {
                    bool greater = char_i1 < char_j1;
                    if (char_i1 == char_j1) {
                        greater = char_i < char_j;
                    }
                    return greater;
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
    inline void compute_partition(const text_t& text, partition_t& partition, bool& part_l) {
        adj_list_t adj_list(text.size() - 1);
        compute_adj_list(text, adj_list);
        adj_list_t reverse_adj_list(text.size() - 1);
        compute_adj_list_reverse(text, reverse_adj_list);
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
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
#endif

        std::cout << std::endl;

#ifdef BENCH
        const auto startTimeLocalSearch = recomp::timer::now();
#endif
        std::vector<size_t> adj_bounds(partition.size() + 1, adj_list.size());
        std::vector<size_t> reverse_adj_bounds(partition.size() + 1, reverse_adj_list.size());
        std::vector<size_t> bounds;
        std::vector<std::uint8_t> flip(partition.size());
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = (size_t)omp_get_num_threads();

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, adj_list.size() - 1);
            }
            for (size_t i = 0; i < adj_list.size() - 1; ++i) {
                bounds[thread_id] = i;
                i = adj_list.size();
            }

            size_t i = bounds[thread_id];
            variable_t val = 0;
            variable_t rev_val = 0;
            if (i == 0) {
                if (text[adj_list[0]] > text[adj_list[0] + 1]) {
                    val = text[adj_list[0]];
                } else {
                    val = text[adj_list[0] + 1];
                }
                if (text[reverse_adj_list[0]] < text[reverse_adj_list[0] + 1]) {
                    rev_val = text[reverse_adj_list[0]];
                } else {
                    rev_val = text[reverse_adj_list[0] + 1];
                }
                adj_bounds[val] = 0;
                reverse_adj_bounds[rev_val] = 0;
            } else if (i < bounds[thread_id + 1]) {
                if (text[adj_list[i - 1]] > text[adj_list[i - 1] + 1]) {
                    val = text[adj_list[i - 1]];
                } else {
                    val = text[adj_list[i - 1] + 1];
                }
                if (text[reverse_adj_list[i - 1]] < text[reverse_adj_list[i - 1] + 1]) {
                    rev_val = text[reverse_adj_list[i - 1]];
                } else {
                    rev_val = text[reverse_adj_list[i - 1] + 1];
                }
            }

            for (; i < bounds[thread_id + 1]; ++i) {
                auto char_i = text[adj_list[i]];
                auto char_i1 = text[adj_list[i] + 1];
                if (char_i < char_i1) {
                    char_i = char_i1;
                }
                if (char_i > val) {
                    adj_bounds[char_i] = i;
                    val = char_i;
                }
                char_i = text[reverse_adj_list[i]];
                char_i1 = text[reverse_adj_list[i] + 1];
                if (char_i < char_i1) {
                    char_i = char_i1;
                }
                if (char_i > rev_val) {
                    reverse_adj_bounds[char_i] = i;
                    rev_val = char_i;
                }
            }

#pragma omp barrier
#pragma omp single
            {
                std::cout << "Bounds" << std::endl;
#pragma omp task
                {
                    for (size_t j = adj_bounds.size() - 1; j > 0; --j) {
                        if (adj_bounds[j - 1] == adj_list.size()) {  // TODO(Chris): check correctness
                            adj_bounds[j - 1] = adj_bounds[j];
                        }
                    }
                }
#pragma omp task
                {
                    for (size_t j = reverse_adj_bounds.size() - 1; j > 0; --j) {
                        if (reverse_adj_bounds[j - 1] == reverse_adj_bounds.size()) {  // TODO(Chris): check correctness
                            reverse_adj_bounds[j - 1] = reverse_adj_bounds[j];
                        }
                    }
                }

                std::cout << "Bounds finished" << std::endl;
            }

            int w_l = 0;
            int w_r = 0;
#pragma omp barrier
#pragma omp for schedule(static)
            for (size_t j = 0; j < partition.size(); ++j) {
                for (size_t k = adj_bounds[j]; k < adj_bounds[j + 1]; ++k) {  // TODO(Chris): check correctness
                    auto char_i = text[adj_list[k]];
                    auto char_i1 = text[adj_list[k] + 1];
                    if (char_i < char_i1) {
                        char_i = char_i1;
                    }
                    if (partition[char_i]) {
                        w_r++;
                    } else {
                        w_l++;
                    }
                }
                for (size_t k = reverse_adj_bounds[j]; k < reverse_adj_bounds[j + 1]; ++k) {  // TODO(Chris): check correctness
                    auto char_i = text[reverse_adj_list[k]];
                    auto char_i1 = text[reverse_adj_list[k] + 1];
                    if (char_i < char_i1) {
                        char_i = char_i1;
                    }
                    if (partition[char_i]) {
                        w_r++;
                    } else {
                        w_l++;
                    }
                }
                if (partition[j]) {
                    flip[j] = (std::uint8_t)((w_r > w_l)? 1 : 0);
                } else {
                    flip[j] = (std::uint8_t)((w_l > w_r)? 1 : 0);
                }
            }

#pragma omp single
            {
                std::cout << "Flipping" << std::endl;
            }
#pragma omp for schedule(static)
            for (size_t j = 0; j < partition.size(); ++j) {
                if (flip[j]) {
                    if (partition[j]) {
                        partition[j] = 0;
                    } else {
                        partition[j] = 1;
                    }
                }
            }
        }
        std::cout << std::endl;
        adj_bounds.resize(0);
        adj_bounds.shrink_to_fit();
        reverse_adj_list.resize(0);
        reverse_adj_list.shrink_to_fit();
        flip.resize(0);
        flip.shrink_to_fit();
#ifdef BENCH
        const auto endTimeLocalSearch = recomp::timer::now();
        const auto timeSpanLocalSearch = endTimeLocalSearch - startTimeLocalSearch;
        std::cout << " local_search=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanLocalSearch).count();
#endif

        std::cout << std::endl;

#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
//        std::vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();
//            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
//                bounds.reserve(n_threads + 1);
//                bounds.resize(n_threads + 1, adj_list.size());
                std::cout << "Bounds" << std::endl;
                std::fill(bounds.begin(), bounds.end(), adj_list.size());
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
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif
        std::cout << std::endl;
        part_l = rl_count > lr_count;
        if (rl_count == lr_count) {
            part_l = prod_r < prod_l;
        }

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
        std::vector<variable_t> mapping;
        compute_mapping(text, mapping);

        partition_t partition(mapping.size(), 0);

        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(text, partition, part_l);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
        std::cout << " alphabet=" << partition.size();
#endif
        std::cout << std::endl;
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
#endif
        std::cout << std::endl;

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
        positions.shrink_to_fit();
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                  << " productions=" << distinct_pairs[distinct_pairs.size() - 1] << " elements=" << pair_count;
#endif

        std::cout << std::endl;

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
