
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
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
 * @brief This class is a parallel implementation of the recompression computing an undirected maximum cut using a
 * parallelized greedy algorithm with locks.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class parallel_lock_recompression : public recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename recompression<variable_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t>::bv_t bv_t;
    typedef std::tuple<variable_t, variable_t, bool> adj_t;
    typedef std::vector<adj_t> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;


    inline parallel_lock_recompression() {
        this->name = "parallel_lock";
    }

    inline parallel_lock_recompression(std::string& dataset) : recompression<variable_t>(dataset) {
        this->name = "parallel_lock";
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
    inline void compute_adj_list(const text_t& text, adj_list_t& adj_list) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (text[i] > text[i + 1]) {
                adj_list[i] = std::make_tuple(text[i], text[i + 1], false);
            } else {
                adj_list[i] = std::make_tuple(text[i + 1], text[i], true);
            }
        }

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
     * @param part_l[out] Indicates which value is used for the left partition set
     */
    inline void compute_partition(const text_t& text, partition_t& partition, bool& part_l) {
        adj_list_t adj_list(text.size() - 1);
        compute_adj_list(text, adj_list);
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

        alphabet_t alphabet(partition.size());
#pragma omp parallel num_threads(this->cores)
        {
            auto partition_iter = partition.begin();
#pragma omp for schedule(static)
            for (size_t i = 0; i < partition.size(); ++i) {
                if (partition_iter == partition.begin()) {
                    std::advance(partition_iter, i);
                }
                alphabet[i] = (*partition_iter).first;
                ++partition_iter;
            }
        }
        ips4o::parallel::sort(alphabet.begin(), alphabet.end(), std::less<variable_t>(), this->cores);

        std::vector<std::shared_timed_mutex> mutexes(alphabet.size());
        std::unordered_map<variable_t, size_t> mapping;
#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < alphabet.size(); ++i) {
#pragma omp critical
            {
                mapping[alphabet[i]] = i;
            }
        }

        std::vector<size_t> hist(alphabet.size() + 1, 0);
#pragma omp parallel num_threads(this->cores)
        {

            std::vector<size_t> t_hist(alphabet.size() + 1, 0);
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < adj_list.size(); ++i) {
                t_hist[mapping[std::get<0>(adj_list[i])]]++;
            }

#pragma omp critical
            {
                for (size_t i = 0; i < t_hist.size(); ++i) {
                    hist[i] += t_hist[i];
                }
            }
        }

        std::vector<size_t> starts;
        starts.push_back(0);
        size_t j = 0;
        for (size_t i = 0; i < alphabet.size(); ++i) {
            if (hist[mapping[alphabet[i]]] > 0) {
                starts.push_back(starts[j] + hist[mapping[alphabet[i]]]);
            }
        }

        for (size_t i = 1; i < starts.size(); ++i) {
            starts[i] += starts[i - 1];
        }

        std::vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp barrier
#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, starts.size() - 1);

                size_t step = starts.size() / n_threads;
                if (starts.size() < n_threads) {
                    step = 1;
                    n_threads = starts.size();
                }
                bounds[0] = 0;
                for (size_t i = 1; i < n_threads; ++i) {
                    bounds[i] = bounds[i-1] + step;
                }
            }

            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
                mutexes[mapping[std::get<0>(adj_list[starts[i]])]].lock();
            }
#pragma omp barrier

            int l_count = 0;
            int r_count = 0;
            size_t index = 0;
            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
                index = starts[i];

                while (index < starts[i + 1]) {
                    mutexes[mapping[std::get<1>(adj_list[index])]].lock_shared();
                    if (partition[std::get<1>(adj_list[index])]) {
                        r_count++;
                    } else {
                        l_count++;
                    }
                    mutexes[mapping[std::get<1>(adj_list[index])]].unlock_shared();
                    index++;
                }
#pragma omp critical
                {
                    partition[std::get<0>(adj_list[index - 1])] = l_count > r_count;
                }
                mutexes[mapping[std::get<0>(adj_list[index - 1])]].unlock();

                l_count = 0;
                r_count = 0;
            }
        }

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
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (std::get<2>(adj_list[i])) {
                if (!partition[std::get<0>(adj_list[i])] &&
                    partition[std::get<1>(adj_list[i])]) {  // bc in text and b in right set and c in left
                    rl_count++;
                } else if (partition[std::get<0>(adj_list[i])] &&
                           !partition[std::get<1>(adj_list[i])]) {  // bc in text and b in left set and c in right
                    lr_count++;
                }
            } else {
                if (!partition[std::get<0>(adj_list[i])] &&
                    partition[std::get<1>(adj_list[i])]) {  // cb in text and c in left set and b in right
                    lr_count++;
                } else if (partition[std::get<0>(adj_list[i])] &&
                           !partition[std::get<1>(adj_list[i])]) {  // cb in text and c in right set and b in left
                    rl_count++;
                }
            }
        }
        part_l = rl_count > lr_count;

        adj_list.resize(1);
        adj_list.shrink_to_fit();
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif

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

        size_t pair_count = 0;

        bool part_l = false;
        compute_partition(text, partition, part_l);

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
                compact_bounds.reserve(n_threads + 1);
                compact_bounds.resize(n_threads + 1, 0);
                compact_bounds[n_threads] = text.size() - 1;
                pair_counts.reserve(n_threads + 1);
                pair_counts.resize(n_threads + 1, 0);
                pair_overlaps.reserve(n_threads);
                pair_overlaps.resize(n_threads, 0);
            }
            std::vector<pair_position_t> t_positions;

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                compact_bounds[thread_id] = i;
                i = text.size();
            }
            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
                compact_bounds[thread_id] = text.size() - 1;
            }
#pragma omp barrier

            size_t i = compact_bounds[thread_id];
            for (; i < compact_bounds[thread_id + 1]; ++i) {
                if (part_l == partition[text[i]] && part_l != partition[text[i + 1]]) {
                    t_positions.emplace_back(i);
                    pair_count++;
                    pair_counts[thread_id + 1]++;
                }
            }

            bounds[thread_id + 1] = t_positions.size();
            if (compact_bounds[thread_id] == text.size() - 1) {
                compact_bounds[thread_id] = text.size();
            }

#pragma omp barrier
#pragma omp single
            {
                compact_bounds[n_threads] = text.size();
                for (size_t j = 1; j < n_threads + 1; ++j) {
                    bounds[j] += bounds[j - 1];
                    pair_counts[j] += pair_counts[j - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            size_t cb = compact_bounds[thread_id];
            if (cb > 0 && cb < text.size()) {
                pair_overlaps[thread_id] = (partition[text[cb - 1]] == part_l && partition[text[cb]] != part_l) ? 1 : 0;
                if (cb + pair_overlaps[thread_id] > pair_counts[thread_id]) {
                    pair_counts[thread_id] = cb + pair_overlaps[thread_id] - pair_counts[thread_id];
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
                assign_bounds.resize(n_threads + 1, 0);
                assign_bounds[n_threads] = positions.size();
                distinct_pairs.reserve(n_threads + 1);
                distinct_pairs.resize(n_threads + 1, 0);
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
                rlslp.resize(rlslp_size);
                bv.resize(rlslp_size, false);
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
                rlslp[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(last_char1, last_char2, len);
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
                    rlslp[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(char_i1, char_i2, len);
                    j++;
                    last_var++;
                    text[positions[i]] = last_var;
                    last_char1 = char_i1;
                    last_char2 = char_i2;
                }
                text[positions[i] + 1] = DELETED;
            }
        }
        positions.resize(1);
        positions.shrink_to_fit();
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count())
                  << " productions=" << distinct_pairs[distinct_pairs.size() - 1] << " elements=" << pair_count;
#endif

#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = text.size() - pair_counts[pair_counts.size() - 1];
        if (new_text_size > 1 && pair_count > 0) {
            text_t new_text(new_text_size);

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
