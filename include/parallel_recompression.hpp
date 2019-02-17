
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

//    const std::string name = "parallel";
    size_t cores = 1;

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
                  << " cores=" << cores << std::endl;
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
                  << " level=" << this->level << " cores=" << cores;
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
#pragma omp parallel num_threads(cores)
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

#pragma omp parallel num_threads(cores)
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
        ips4o::parallel::sort(sort_blocks.begin(), sort_blocks.end(), std::less<block_t>(), cores);
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

#pragma omp parallel for schedule(static) num_threads(cores)
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

#pragma omp parallel for schedule(static) num_threads(cores)
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

#pragma omp parallel num_threads(cores)
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
#ifdef BENCH
            const auto endTimeCompact = recomp::timer::now();
            const auto timeSpanCompact = endTimeCompact - startTimeCompact;
            std::cout << " compact_text="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

            text = std::move(new_text);
        } else if (new_text_size == 1) {
            text.resize(new_text_size);
            text.shrink_to_fit();
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

#pragma omp parallel for schedule(static) num_threads(cores)
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
            if (text[i] > text[i + 1]) {
                if (text[j] > text[j + 1]) {
                    bool less = text[i] < text[j];
                    if (text[i] == text[j]) {
                        less = text[i + 1] < text[j + 1];
                    }
                    return less;
                } else {
                    bool less = text[i] < text[j + 1];
                    if (text[i] == text[j + 1]) {
                        less = text[i + 1] < text[j];
                    }
                    return less;
                }
            } else {
                if (text[j] > text[j + 1]) {
                    bool less = text[i + 1] < text[j];
                    if (text[i + 1] == text[j]) {
                        less = text[i] < text[j + 1];
                    }
                    return less;
                } else {
                    bool less = text[i + 1] < text[j + 1];
                    if (text[i + 1] == text[j + 1]) {
                        less = text[i] < text[j];
                    }
                    return less;
                }
            }
        };

        ips4o::parallel::sort(adj_list.begin(), adj_list.end(), sort_adj, cores);
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
                if (!partition[text[adj_list[0]]]) {
                    l_count++;
                } else {
                    r_count++;
                }
            } else {
                val = text[adj_list[0] + 1];
                if (!partition[text[adj_list[0] + 1]]) {
                    l_count++;
                } else {
                    r_count++;
                }
            }
        }
        for (size_t i = 1; i < adj_list.size(); ++i) {
            if (text[adj_list[i]] > text[adj_list[i] + 1]) {
                if (val < text[adj_list[i]]) {
                    partition[val] = l_count > r_count;
                    l_count = 0;
                    r_count = 0;
                    val = text[adj_list[i]];
                }
                if (partition[text[adj_list[i] + 1]]) {
                    r_count++;
                } else {
                    l_count++;
                }
            } else {
                if (val < text[adj_list[i] + 1]) {
                    partition[val] = l_count > r_count;
                    l_count = 0;
                    r_count = 0;
                    val = text[adj_list[i] + 1];
                }
                if (partition[text[adj_list[i]]]) {
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
#pragma omp parallel for num_threads(cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
        for (size_t i = 0; i < text.size() - 1; ++i) {
            if (!partition[text[i]] && partition[text[i + 1]]) {
                lr_count++;
            } else if (partition[text[i]] && !partition[text[i + 1]]) {
                rl_count++;
            }
        }
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif

        part_l = rl_count > lr_count;
//        if (rl_count > lr_count) {
//#ifdef BENCH
//            const auto startTimeSwap = recomp::timer::now();
//#endif
//#pragma omp parallel num_threads(cores)
//            {
//#pragma omp single
//                {
//                    for (auto iter = partition.begin(); iter != partition.end(); ++iter) {
//#pragma omp task
//                        {
//                            (*iter).second = !(*iter).second;
//                        }
//                    }
//                }
//            }
//#ifdef BENCH
//            const auto endTimeSwap = recomp::timer::now();
//            const auto timeSpanSwap = endTimeSwap - startTimeSwap;
//            std::cout << " swap="
//                      << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSwap).count();
//#endif
//        } else {
//#ifdef BENCH
//            std::cout << " swap=0";
//#endif
//        }
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
                  << " level=" << this->level << " cores=" << cores;
#endif
        partition_t partition;
        for (size_t i = 0; i < text.size(); ++i) {
            partition[text[i]] = false;
        }

#ifdef BENCH
        const auto endTimeCreate = recomp::timer::now();
        const auto timeSpanCreate = endTimeCreate - startTime;
        std::cout << " alphabet=" << partition.size() << " create_partition="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCreate).count();
#endif
        adj_list_t adj_list(text.size() - 1);
        compute_adj_list(text, adj_list);

        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(text, adj_list, partition, part_l);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
#endif
        std::unordered_map<pair_t, variable_t, pair_hash> pairs;
        std::vector<pair_position_t> positions;

        std::vector<size_t> bounds;
        std::vector<size_t> pair_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                compact_bounds.reserve(n_threads + 1);
                compact_bounds.resize(n_threads + 1, 0);
                compact_bounds[n_threads] = text.size();
                pair_counts.reserve(n_threads + 1);
                pair_counts.resize(n_threads + 1, 0);
                pair_overlaps.reserve(n_threads);
                pair_overlaps.resize(n_threads, 0);
            }
            std::vector<pair_position_t> t_positions;
            std::unordered_map<pair_t, variable_t, pair_hash> t_pairs;

            bool begin = true;

#pragma omp for schedule(static) nowait reduction(+:pair_count)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                if (begin) {
                    compact_bounds[thread_id] = i;
                    begin = false;
                }
                if (part_l == partition[text[i]] && part_l != partition[text[i + 1]]) {
//                if (!partition[text[i]] && partition[text[i + 1]]) {
//                    DLOG(INFO) << "Pair (" << text[i] << "," << text[i + 1] << ") found at " << i << " by thread "
//                               << thread_id;
                    auto pair = std::make_pair(text[i], text[i + 1]);
                    t_pairs[pair] = 1;
                    t_positions.emplace_back(i);
                    pair_count++;
                    pair_counts[thread_id + 1]++;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < n_threads + 1; ++i) {
                    bounds[i] += bounds[i - 1];
                    pair_counts[i] += pair_counts[i - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            size_t cb = compact_bounds[thread_id];
            if (thread_id > 0 && compact_bounds[thread_id] == 0) {
                compact_bounds[thread_id] = text.size();
            } else if (cb > 0) {
                pair_overlaps[thread_id] = (partition[text[cb - 1]] == part_l && partition[text[cb]] != part_l)? 1 : 0;
                if (compact_bounds[thread_id] + pair_overlaps[thread_id] > pair_counts[thread_id]) {
                    pair_counts[thread_id] = compact_bounds[thread_id] + pair_overlaps[thread_id] - pair_counts[thread_id];
                } else {
                    pair_counts[thread_id] = 0;
                }
            }

#pragma omp critical
            pairs.insert(t_pairs.begin(), t_pairs.end());
            // TODO(Chris): maybe use std::vector, sort it and than make pairs unique, possibly better scaling
        }
        pair_overlaps.resize(0);
        pair_overlaps.shrink_to_fit();
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());
#endif

#ifdef BENCH
        const auto startTimeCopy = recomp::timer::now();
#endif
        std::vector<pair_t> sort_pairs(pairs.size());

#pragma omp parallel num_threads(cores)
        {
            auto iter = pairs.begin();

#pragma omp for schedule(static)
            for (size_t i = 0; i < pairs.size(); ++i) {
                if (iter == pairs.begin()) {
                    std::advance(iter, i);
                }
//                DLOG(INFO) << "Adding pair (" << (*iter).first.first << "," << (*iter).first.second << ") at index "
//                           << i;
                sort_pairs[i] = (*iter).first;
                ++iter;
            }
        }
#ifdef BENCH
        const auto endTimeCopy = recomp::timer::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_pairs=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
#endif

#ifdef BENCH
        const auto startTimeSort = recomp::timer::now();
#endif
//        parallel::partitioned_radix_sort(sort_pairs);
        ips4o::parallel::sort(sort_pairs.begin(), sort_pairs.end(), std::less<pair_t>(), cores);
#ifdef BENCH
        const auto endTimeSort = recomp::timer::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_pairs.size() << " pairs=" << pair_count;
#endif

#ifdef BENCH
        const auto startTimeAss = recomp::timer::now();
#endif
        pair_count = sort_pairs.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + pair_count);
        rlslp.resize(nt_count + pair_count);
        bv.resize(nt_count + pair_count, false);

        variable_t next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);

#pragma omp parallel for schedule(static) num_threads(cores)
        for (size_t i = 0; i < sort_pairs.size(); ++i) {
//            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_pairs[i].first << ","
//                       << sort_pairs[i].second << ") at index " << nt_count + i;
            pairs[sort_pairs[i]] = next_nt + static_cast<variable_t>(i);
            size_t len = 0;
            if (sort_pairs[i].first >= static_cast<variable_t>(rlslp.terminals)) {
                len = rlslp[sort_pairs[i].first - rlslp.terminals].len;
            } else {
                len = 1;
            }
            if (sort_pairs[i].second >= static_cast<variable_t>(rlslp.terminals)) {
                len += rlslp[sort_pairs[i].second - rlslp.terminals].len;
            } else {
                len += 1;
            }
            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_pairs[i].first, sort_pairs[i].second, len);
        }
#ifdef BENCH
        const auto endTimeAss = recomp::timer::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " pair_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif

#ifdef BENCH
        const auto startTimeRep = recomp::timer::now();
#endif

#pragma omp parallel for schedule(static) num_threads(cores)
        for (size_t i = 0; i < positions.size(); ++i) {
            auto pos = positions[i];
            auto pair = std::make_pair(text[pos], text[pos + 1]);
            text[pos] = pairs[pair];

            text[pos + 1] = DELETED;
        }
#ifdef BENCH
        const auto endTimeRep = recomp::timer::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif

#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = text.size() - positions.size();
        if (new_text_size > 1 && pair_count > 0) {
            text_t new_text;
            new_text.reserve(new_text_size);
            new_text.resize(new_text_size);
#pragma omp parallel num_threads(cores)
            {
                auto thread_id = omp_get_thread_num();
                size_t copy_i = pair_counts[thread_id];
                for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                    if (text[i] != DELETED) {
                        new_text[copy_i++] = text[i];
                    }
                }
            }

//        if (new_text_size > 1 && pair_count > 0) {
//            size_t copy_i = positions[0] + 1;
//            size_t i = positions[0] + 2;  // jump to first position to copy
//
//            for (; i < text.size(); ++i) {
//                if (text[i] != DELETED) {
//                    text[copy_i++] = text[i];
//                }
//            }
//        }
#ifdef BENCH
            const auto endTimeCompact = recomp::timer::now();
            const auto timeSpanCompact = endTimeCompact - startTimeCompact;
            std::cout << " compact_text="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

            text = std::move(new_text);
        } else if (new_text_size == 1) {
            text.resize(new_text_size);
            text.shrink_to_fit();
        }

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
