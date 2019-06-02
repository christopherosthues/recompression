
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <deque>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression/recompression.hpp"
#include "recompression/defs.hpp"
#include "recompression/util.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/radix_sort.hpp"

namespace recomp {

namespace parallel {

/**
 * @brief This class is a parallel implementation of the recompression computing a sequential undirected maximum cut.
 *
 * Renames the pairs using the adjacency list. At first all pairs ba with a < b are renamed than all pairs ab.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class recompression_order_ls : public recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename recompression<variable_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t>::bv_t bv_t;
    typedef std::tuple<bool, variable_t, variable_t> adj_t;
    typedef std::vector<adj_t> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;


    inline recompression_order_ls() {
        this->name = "parallel_order_ls";
    }

    inline recompression_order_ls(std::string& dataset) : recompression<variable_t>(dataset) {
        this->name = "parallel_order_ls";
    }

    inline virtual void recomp(text_t& text,
                               rlslp<variable_t>& rlslp,
                               const size_t& alphabet_size,
                               const size_t cores) override {
#ifdef BENCH_RECOMP
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

        rlslp.resize(rlslp.size());

        if (text.size() > 0) {
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
                  << " cores=" << this->cores << " size=" << text_size << std::endl;
#endif
    }

    using recompression<variable_t>::recomp;

 private:
    const variable_t DELETED = UINT_MAX;

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
                compact_bounds.resize(n_threads + 1, 0);
                compact_bounds[n_threads] = text.size();
                block_counts.reserve(n_threads + 1);
                block_counts.resize(n_threads + 1, 0);
                block_overlaps.reserve(n_threads);
                block_overlaps.resize(n_threads, 0);
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

            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
                compact_bounds[thread_id] = text.size();
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

            if (compact_bounds[thread_id] > 0 && compact_bounds[thread_id] < text.size()) {
                if (compact_bounds[thread_id] + block_overlaps[thread_id] > block_counts[thread_id]) {
                    block_counts[thread_id] = compact_bounds[thread_id] + block_overlaps[thread_id] - block_counts[thread_id];
                } else {
                    block_counts[thread_id] = 0;
                }
            }
        }
        block_overlaps.resize(1);
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
                    assign_bounds.resize(n_threads + 1, 0);
                    assign_bounds[n_threads] = positions.size();
                    distinct_blocks.reserve(n_threads + 1);
                    distinct_blocks.resize(n_threads + 1, 0);
                }

#pragma omp for schedule(static)
                for (size_t i = 0; i < positions.size(); ++i) {
                    assign_bounds[thread_id] = i;
                    i = positions.size();
                }

                if (thread_id > 0 && assign_bounds[thread_id] == 0) {
                    assign_bounds[thread_id] = positions.size();
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
                    rlslp[nt_count + distinct_blocks[thread_id] + j] = non_terminal<variable_t>(last_char, b_len, len);
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
                        rlslp[nt_count + distinct_blocks[thread_id] + j] = non_terminal<variable_t>(char_i, b_len, len);
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
            positions.resize(1);
            positions.shrink_to_fit();
#ifdef BENCH
            const auto endTimeRules = recomp::timer::now();
            const auto timeSpanRules = endTimeRules - startTimeRules;
            std::cout << " rules="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                      << " productions=" << distinct_blocks[distinct_blocks.size() - 1] << " elements=" << block_count;
#endif

#ifdef BENCH
            const auto startTimeCompact = recomp::timer::now();
#endif
            size_t new_text_size = text.size() - block_counts[block_counts.size() - 1];
            if (new_text_size > 1 && block_count > 0) {
                text_t new_text(new_text_size);

#pragma omp parallel num_threads(this->cores)
                {
                    auto thread_id = omp_get_thread_num();
                    size_t copy_i = block_counts[thread_id];
                    for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                        if (text[i] != DELETED) {
                            new_text[copy_i++] = text[i];
                        }
                    }
                }

                text = std::move(new_text);
            } else if (new_text_size == 1) {
                text.resize(new_text_size);
            }
#ifdef BENCH
            const auto endTimeCompact = recomp::timer::now();
            const auto timeSpanCompact = endTimeCompact - startTimeCompact;
            std::cout << " compact_text="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif
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
     * @brief Computes the adjacency list of the text.
     *
     * @param text The text
     * @param adj_list[out] The adjacency list
     */
    inline void compute_adj_list(const text_t& text, adj_list_t& adj_list, size_t& begin) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

        size_t beg = 0;
#pragma omp parallel for schedule(static) num_threads(this->cores) reduction(+:beg)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (text[i] < text[i + 1]) {
                adj_list[i] = std::make_tuple(false, text[i], text[i + 1]);
                beg++;
            } else {
                adj_list[i] = std::make_tuple(true, text[i + 1], text[i]);
            }
        }
        begin = beg;

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif

#ifdef BENCH
        const auto startTimeMult = recomp::timer::now();
#endif
        ips4o::parallel::sort(adj_list.begin(), adj_list.end(), std::less<adj_t>(), this->cores);
#ifdef BENCH
        const auto endTimeMult = recomp::timer::now();
        const auto timeSpanMult = endTimeMult - startTimeMult;
        std::cout << " sort_adj_list="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMult).count());
#endif
    }


    /**
     * @brief Computes a partitioning of the symbol in the text.
     *
     * @param adj_list[in] The adjacency list of the text
     * @param partition[out] The partition
     */
    inline void compute_partition(const adj_list_t& adj_list, partition_t& partition, size_t& begin, bool& part_l) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        std::unordered_map<variable_t, std::int32_t> sums;
        for (const auto& c : partition) {
            sums[c.first] = 0;
        }

        std::vector<variable_t> alphabet(partition.size());
#pragma omp parallel num_threads(this->cores)
        {
            auto iter = partition.begin();
#pragma omp for schedule(static)
            for (size_t i = 0; i < alphabet.size(); ++i) {
                if (iter == partition.begin()) {
                    std::advance(iter, i);
                }
                alphabet[i] = (*iter).first;
                ++iter;
            }
        }

        ips4o::parallel::sort(alphabet.begin(), alphabet.end(), std::less<variable_t>(), this->cores);

#ifdef BENCH
        const auto startTimePar = recomp::timer::now();
#endif
        size_t l = 0;
        size_t r = begin;
        for (size_t i = 0; i < alphabet.size(); ++i) {
            auto found = partition.find(alphabet[i]);
            (*found).second = sums[alphabet[i]] < 0;

            while (l < begin && std::get<1>(adj_list[l]) == alphabet[i]) {
                sums[std::get<2>(adj_list[l])] += ((*found).second)? 1 : -1;
                l++;
            }
            while (r < adj_list.size() && std::get<1>(adj_list[r]) == alphabet[i]) {
                sums[std::get<2>(adj_list[r])] += ((*found).second)? 1 : -1;
                r++;
            }
        }
#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTimePar;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif

#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
#pragma omp parallel for num_threads(this->cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (std::get<0>(adj_list[i])) {
                if (!partition[std::get<1>(adj_list[i])] &&
                    partition[std::get<2>(adj_list[i])]) {  // cb in text and c in right set and b in left
                    rl_count++;
                } else if (partition[std::get<1>(adj_list[i])] &&
                           !partition[std::get<2>(adj_list[i])]) {  // cb in text and c in left set and b in right
                    lr_count++;
                }
            } else {
                if (!partition[std::get<1>(adj_list[i])] &&
                    partition[std::get<2>(adj_list[i])]) {  // bc in text and b in left set and c in right
                    lr_count++;
                } else if (partition[std::get<1>(adj_list[i])] &&
                           !partition[std::get<2>(adj_list[i])]) {  // bc in text and b in right set and c in left
                    rl_count++;
                }
            }
        }
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif

        part_l = lr_count < rl_count;
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
        partition_t partition;
        for (size_t i = 0; i < text.size(); ++i) {
            partition[text[i]] = false;
        }

#ifdef BENCH
        std::cout << " alphabet=" << partition.size();
#endif
        adj_list_t adj_list(text.size() - 1);
        size_t begin = 0;
        compute_adj_list(text, adj_list, begin);

        variable_t next_nt = rlslp.terminals + rlslp.non_terminals.size();

        std::unordered_map<pair_t, variable_t, pair_hash> pairs;
        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(adj_list, partition, begin, part_l);

#ifdef BENCH
        const auto startTimeRules = recomp::timer::now();
#endif
        std::deque<non_terminal<variable_t>> new_rules;
        if (adj_list.size() > 0) {
            variable_t left, right;
            bool pair_found;
            if (std::get<0>(adj_list[0])) {
                left = std::get<2>(adj_list[0]);
                right = std::get<1>(adj_list[0]);
                pair_found = partition[left] == part_l && partition[right] != part_l;
            } else {
                left = std::get<1>(adj_list[0]);
                right = std::get<2>(adj_list[0]);
                pair_found = partition[left] == part_l && partition[right] != part_l;
            }
            if (pair_found) {
                auto pair = std::make_pair(left, right);
                pairs[pair] = next_nt++;
                size_t len = 0;
                if (left >= static_cast<variable_t>(rlslp.terminals)) {
                    len = rlslp[left - rlslp.terminals].len;
                } else {
                    len = 1;
                }
                if (right >= static_cast<variable_t>(rlslp.terminals)) {
                    len += rlslp[right - rlslp.terminals].len;
                } else {
                    len += 1;
                }
                new_rules.emplace_back(left, right, len);
//                rlslp.non_terminals.emplace_back(left, right, len);
            }
        }

        for (size_t i = 1; i < adj_list.size(); ++i) {
            variable_t left, right;
            variable_t l_before = std::get<1>(adj_list[i - 1]);
            variable_t r_before = std::get<2>(adj_list[i - 1]);
            bool pair_found;
            if (std::get<0>(adj_list[i])) {
                left = std::get<2>(adj_list[i]);
                right = std::get<1>(adj_list[i]);

                pair_found = partition[left] == part_l && partition[right] != part_l;
                if (std::get<0>(adj_list[i - 1])) {
                    pair_found &= (r_before != left || l_before != right);
                }
            } else {
                left = std::get<1>(adj_list[i]);
                right = std::get<2>(adj_list[i]);

                pair_found = partition[left] == part_l && partition[right] != part_l;
                if (!std::get<0>(adj_list[i - 1])) {
                    pair_found &= (l_before != left || r_before != right);
                }
            }
            if (pair_found) {
                auto pair = std::make_pair(left, right);
                pairs[pair] = next_nt++;
                size_t len = 0;
                if (left >= static_cast<variable_t>(rlslp.terminals)) {
                    len = rlslp[left - rlslp.terminals].len;
                } else {
                    len = 1;
                }
                if (right >= static_cast<variable_t>(rlslp.terminals)) {
                    len += rlslp[right - rlslp.terminals].len;
                } else {
                    len += 1;
                }
                new_rules.emplace_back(left, right, len);
//                rlslp.non_terminals.emplace_back(left, right, len);
            }
        }
        size_t size = rlslp.size();
        rlslp.resize(size + new_rules.size());
#pragma omp parallel for num_threads(this->cores) schedule(static)
        for (size_t i = 0; i < new_rules.size(); ++i) {
            rlslp[size + i] = new_rules[i];
        }
        adj_list.resize(1);
        adj_list.shrink_to_fit();
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count());
#endif

        bv.resize(rlslp.size(), false);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
#endif

        std::vector<size_t> pair_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                compact_bounds.reserve(n_threads + 1);
                compact_bounds.resize(n_threads + 1, 0);
                compact_bounds[n_threads] = text.size();
                pair_counts.reserve(n_threads + 1);
                pair_counts.resize(n_threads + 1, 0);
                pair_overlaps.reserve(n_threads);
                pair_overlaps.resize(n_threads, 0);
            }

            bool begin_p = true;

#pragma omp for schedule(static) reduction(+:pair_count)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                if (begin_p) {
                    compact_bounds[thread_id] = i;
                    begin_p = false;
                }
                auto ti = partition.find(text[i]);
                auto tn = partition.find(text[i + 1]);
                if (ti != partition.end() && tn != partition.end()) {
                    bool p_l = (*ti).second;
                    bool p_r = (*tn).second;
                    if (p_l == part_l && p_l != p_r) {
                        auto pair = std::make_pair(text[i], text[i + 1]);
                        text[i++] = pairs[pair];
                        text[i] = DELETED;
                        pair_count++;
                        pair_counts[thread_id + 1]++;
                    }
                }
            }

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < n_threads + 1; ++i) {
                    pair_counts[i] += pair_counts[i - 1];
                }
            }

            size_t cb = compact_bounds[thread_id];
            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
                compact_bounds[thread_id] = text.size();
            } else if (cb > 0) {
                pair_overlaps[thread_id] = (text[cb] == DELETED)? 1 : 0;
                if (compact_bounds[thread_id] + pair_overlaps[thread_id] > pair_counts[thread_id]) {
                    pair_counts[thread_id] = compact_bounds[thread_id] + pair_overlaps[thread_id] - pair_counts[thread_id];
                } else {
                    pair_counts[thread_id] = 0;
                }
            }
        }
        {
            auto discard = std::move(partition);
        }
        pair_overlaps.resize(1);
        pair_overlaps.shrink_to_fit();
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count())
                  << " productions=" << pairs.size() << " elements=" << pair_count;;
#endif

#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = text.size() - pair_count;
        if (new_text_size > 1 && pair_count > 0) {
            text_t new_text(new_text_size);
//            new_text.reserve(new_text_size);
//            new_text.resize(new_text_size);
#pragma omp parallel num_threads(this->cores)
            {
                auto thread_id = omp_get_thread_num();
                size_t copy_i = pair_counts[thread_id];
                for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                    if (text[i] != DELETED) {
                        new_text[copy_i++] = text[i];
                    }
                }
            }
            text = std::move(new_text);
        } else if (new_text_size == 1) {
            text.resize(new_text_size);
//            text.shrink_to_fit();
        }
#ifdef BENCH
        const auto endTimeCompact = recomp::timer::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif
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
