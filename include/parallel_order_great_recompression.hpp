
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
class recompression_order_gr : public recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename recompression<variable_t, terminal_count_t>::alphabet_t alphabet_t;
    typedef typename recompression<variable_t, terminal_count_t>::bv_t bv_t;
    typedef std::tuple<bool, variable_t, variable_t> adj_t;
    typedef std::vector<adj_t> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;

//    const std::string name = "parallel_gr";
//    size_t cores = 1;

    inline recompression_order_gr() {
        this->name = "parallel_gr";
    }

    inline recompression_order_gr(std::string& dataset) : recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "parallel_gr";
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
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

        rlslp.resize(rlslp.size());

        if (!text.empty()) {
//            rlslp.root = static_cast<variable_t>(rlslp.size() - 1);
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
//        size_t substr_len = 0;

#ifdef BENCH
        const auto startTimeBlocks = recomp::timer::now();
#endif
        std::unordered_map<block_t, variable_t, pair_hash> blocks;
        std::vector<position_t> positions;

        std::vector<size_t> bounds;
        std::vector<size_t> block_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> block_overlaps;
#pragma omp parallel num_threads(this->cores)
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
            std::unordered_map<block_t, variable_t, pair_hash> t_blocks;
            bool begin = true;
            bool add = false;

#pragma omp for schedule(static) nowait reduction(+:block_count)  // reduction(+:substr_len)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                if (begin) {
                    compact_bounds[thread_id] = i;
//#pragma omp critical
//                    {std::cout << "begin at " << i << " for thread " << thread_id << std::endl;}
                    begin = false;
                    if (i == 0) {
                        add = thread_id == 0;
                    } else {
                        add = text[i - 1] != text[i];
                    }
//                    add = !(begin > 1 && i > 0 && text[begin - 1] == text[begin]);
//                    std::cout << "add: " << add << std::endl;
                }
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
                    block_len++;
                    i++;
                }
                if (!add) {
//#pragma omp critical
//                    {std::cout << "skipping block (" << text[i] << "," << block_len << ")" << std::endl;}
                    block_overlaps[thread_id] = block_len;
                    block_len = 1;
                    add = true;
                }
                if (block_len > 1) {
//                    substr_len += block_len - 1;
//#pragma omp critical
//                    {std::cout << "Block (" << text[i] << "," << block_len << ") found at " << (i - block_len + 1)
//                               << " by thread " << thread_id << std::endl;}
                    t_positions.emplace_back(block_len, i - block_len + 1);
                    block_t block = std::make_pair(text[i], block_len);
                    t_blocks[block] = 1;
                    block_count++;
                    block_counts[thread_id + 1] += block_len - 1;
                    block_len = 1;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < n_threads + 1; ++i) {
                    bounds[i] += bounds[i - 1];
                    block_counts[i] += block_counts[i - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
                compact_bounds[thread_id] = text.size();
            } else if (compact_bounds[thread_id] > 0) {
                if (compact_bounds[thread_id] + block_overlaps[thread_id] > block_counts[thread_id]) {
                    block_counts[thread_id] = compact_bounds[thread_id] + block_overlaps[thread_id] - block_counts[thread_id];
                } else {
                    block_counts[thread_id] = 0;
                }
            }

#pragma omp critical
            blocks.insert(t_blocks.begin(), t_blocks.end());
        }
        block_overlaps.resize(0);
        block_overlaps.shrink_to_fit();
#ifdef BENCH
        const auto endTimeBlocks = recomp::timer::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());
#endif

#ifdef BENCH
        const auto startTimeCopy = recomp::timer::now();
#endif

        std::vector<block_t> sort_blocks(blocks.size());

#pragma omp parallel num_threads(this->cores)
        {
            auto iter = blocks.begin();

#pragma omp for schedule(static)
            for (size_t i = 0; i < blocks.size(); ++i) {
                if (iter == blocks.begin()) {
                    std::advance(iter, i);
                }
//                DLOG(INFO) << "Adding block (" << (*iter).first.first << "," << (*iter).first.second << ") at index "
//                           << i;
                sort_blocks[i] = (*iter).first;
                ++iter;
            }
        }
#ifdef BENCH
        const auto endTimeCopy = recomp::timer::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
#endif

#ifdef BENCH
        const auto startTimeSort = recomp::timer::now();
#endif
//        parallel::partitioned_radix_sort(sort_blocks);
        ips4o::parallel::sort(sort_blocks.begin(), sort_blocks.end(), std::less<block_t>(), this->cores);
#ifdef BENCH
        const auto endTimeSort = recomp::timer::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_blocks.size() << " blocks=" << block_count;;
#endif

#ifdef BENCH
        const auto startTimeAss = recomp::timer::now();
#endif
        block_count = sort_blocks.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + block_count);
        rlslp.resize(nt_count + block_count/*, true*/);
        rlslp.blocks += block_count;
        bv.resize(nt_count + block_count, true);
//        rlslp.block_count += block_count;

        auto next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);

#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < sort_blocks.size(); ++i) {
//            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_blocks[i].first << ","
//                       << sort_blocks[i].second << ") at index " << nt_count + i;
            blocks[sort_blocks[i]] = next_nt + static_cast<variable_t>(i);
            auto len = static_cast<size_t>(sort_blocks[i].second);
            if (sort_blocks[i].first >= rlslp.terminals) {
                len *= rlslp[sort_blocks[i].first - rlslp.terminals].len;
            }
            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_blocks[i].first, sort_blocks[i].second, len);
        }
#ifdef BENCH
        const auto endTimeAss = recomp::timer::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " block_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif

#ifdef BENCH
        const auto startTimeRep = recomp::timer::now();
#endif

#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < positions.size(); ++i) {
            auto block = std::make_pair(text[positions[i].second], positions[i].first);
            text[positions[i].second] = blocks[block];

            auto length = static_cast<size_t>(positions[i].first);
            for (size_t j = 1; j < length; ++j) {
                text[j + positions[i].second] = DELETED;
            }
        }
#ifdef BENCH
        const auto endTimeRep = recomp::timer::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif

#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = text.size() - block_counts[block_counts.size() - 1];  // substr_len;
        if (new_text_size > 1 && block_count > 0) {
            text_t new_text;
            new_text.reserve(new_text_size);
            new_text.resize(new_text_size);

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

//        if (new_text_size > 1 && block_count > 0) {
//            size_t copy_i = positions[0].second + 1;
//            size_t i = positions[0].second + positions[0].first;  // jump to first position to copy
//
//            for (; i < text.size(); ++i) {
//                if (text[i] != DELETED) {
//                    text[copy_i++] = text[i];
//                }
//            }
//        }
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
            if (text[i] > text[i + 1]) {
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
//        partitioned_radix_sort(adj_list);
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
    inline void compute_partition(const text_t& text,
                                  const adj_list_t& adj_list,
                                  partition_t& partition,
                                  size_t& begin,
                                  bool& part_l) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        int l_count = 0;
        int r_count = 0;
        size_t glob_i = 0;
        size_t i = 0;
        size_t j = begin;
        variable_t actual = std::get<1>(adj_list[0]);
        if (begin < adj_list.size()) {
            actual = std::min(actual, std::get<1>(adj_list[begin]));
        }
        variable_t next = actual;
        while (glob_i < adj_list.size()) {
            while (i < begin && std::get<1>(adj_list[i]) == actual) {
                if (partition[std::get<2>(adj_list[i])]) {
                    r_count++;
                } else {
                    l_count++;
                }
                i++;
                glob_i++;
            }
            if (i < begin) {
                next = std::get<1>(adj_list[i]);
            }

            while (j < adj_list.size() && std::get<1>(adj_list[j]) == actual) {
                if (partition[std::get<2>(adj_list[j])]) {
                    r_count++;
                } else {
                    l_count++;
                }
                j++;
                glob_i++;
            }
            partition[actual] = l_count > r_count;
            l_count = 0;
            r_count = 0;

            if (j < adj_list.size() && i < begin) {
                actual = std::min(next, std::get<1>(adj_list[j]));
            } else if (j < adj_list.size()) {
                actual = std::get<1>(adj_list[j]);
            } else {
                actual = next;
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
        for (size_t k = 0; k < text.size() - 1; ++k) {
            if (!partition[text[k]] && partition[text[k + 1]]) {
                lr_count++;
            } else if (partition[text[k]] && !partition[text[k + 1]]) {
                rl_count++;
            }
        }
//        for (size_t i = 0; i < adj_list.size(); ++i) {
//            if (std::get<0>(adj_list[i])) {
//                if (!partition[std::get<1>(adj_list[i])] &&
//                    partition[std::get<2>(adj_list[i])]) {  // bc in text and b in right set and c in left
//                    rl_count++;
//                } else if (partition[std::get<1>(adj_list[i])] &&
//                           !partition[std::get<2>(adj_list[i])]) {  // bc in text and b in left set and c in right
//                    lr_count++;
//                }
//            } else {
//                if (!partition[std::get<1>(adj_list[i])] &&
//                    partition[std::get<2>(adj_list[i])]) {  // cb in text and c in left set and b in right
//                    lr_count++;
//                } else if (partition[std::get<1>(adj_list[i])] &&
//                           !partition[std::get<2>(adj_list[i])]) {  // cb in text and c in right set and b in left
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
     */
    inline void pcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, bv_t& bv) {
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
        compute_partition(text, adj_list, partition, begin, part_l);

#ifdef BENCH
        const auto startTimeRules = recomp::timer::now();
#endif
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
                rlslp.non_terminals.emplace_back(left, right, len);
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
                rlslp.non_terminals.emplace_back(left, right, len);
            }
        }
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " pair_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count());
#endif

//        rlslp.blocks.reserve(rlslp.size());
//        rlslp.blocks.resize(rlslp.size()/*, false*/);
        bv.resize(rlslp.size(), false);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
#endif

#pragma omp parallel num_threads(this->cores)
        {
#pragma omp for schedule(static) reduction(+:pair_count)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                auto ti = partition.find(text[i]);
                auto tn = partition.find(text[i + 1]);
                if (ti != partition.end() && tn != partition.end()) {
                    bool p_l = (*ti).second;
                    bool p_r = (*tn).second;
                    if (p_l == part_l && p_l != p_r) {
//                    DLOG(INFO) << "Pair (" << text[i] << "," << text[i + 1] << ") found at " << i << " by thread "
//                               << thread_id;
                        auto pair = std::make_pair(text[i], text[i + 1]);
                        text[i++] = pairs[pair];
                        text[i] = DELETED;
                        pair_count++;
                    }
                }
            }
        }
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count())
                  << " elements=" << pairs.size() << " pairs=" << pair_count;;
#endif

#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = text.size() - pair_count;
        if (new_text_size > 1 && pair_count > 0) {
            size_t copy_i = 1;
            for (size_t i = 1; i < text.size(); ++i) {
                if (text[i] != DELETED) {
                    text[copy_i++] = text[i];
                }
            }
        }
#ifdef BENCH
        const auto endTimeCompact = recomp::timer::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

        text.resize(new_text_size);
        text.shrink_to_fit();

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
