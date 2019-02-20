
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
                compact_bounds.resize(n_threads + 1, 0);
                compact_bounds[n_threads] = text.size();
                block_counts.reserve(n_threads + 1);
                block_counts.resize(n_threads + 1, 0);
                block_overlaps.reserve(n_threads);
                block_overlaps.resize(n_threads, 0);
            }
            std::vector<position_t> t_positions;
//            bool begin = true;
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

//#pragma omp single
//            {
//                std::cout << std::endl << "compact: ";
//                for (size_t k = 0; k < compact_bounds.size(); ++k) {
//                    std::cout << compact_bounds[k] << ", ";
//                }
//                std::cout << std::endl;
//            }

            size_t i = compact_bounds[thread_id];
            if (i > 0 && i < compact_bounds[thread_id + 1]) {
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
                    block_len++;
                    i++;
                }
                if (!add) {
//#pragma omp critical
//                    {std::cout << "skipping block (" << text[i] << "," << block_len << ") by " << thread_id << std::endl;}
                    block_overlaps[thread_id] = block_len;
                    block_len = 1;
                    add = true;
                }
                if (block_len > 1) {
//#pragma omp critical
//                    {std::cout << "Block (" << text[i] << "," << block_len << ") found at " << (i - block_len + 1)
//                               << " by thread " << thread_id << std::endl;}
                    t_positions.emplace_back(block_len, i - block_len + 1);
                    block_count++;
                    block_counts[thread_id + 1] += block_len - 1;
                    block_len = 1;
                }
                i++;
            }

//#pragma omp critical
//            {std::cout << "i: " << i << " by " << thread_id << std::endl;}

            for (; i < compact_bounds[thread_id + 1]; ++i) {
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
                    block_len++;
                    i++;
                }
                if (block_len > 1) {
//#pragma omp critical
//                    {std::cout << "Block (" << text[i] << "," << block_len << ") found at " << (i - block_len + 1)
//                               << " by thread " << thread_id << std::endl;}
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
        block_overlaps.resize(0);
        block_overlaps.shrink_to_fit();
#ifdef BENCH
        const auto endTimeBlocks = recomp::timer::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());
#endif

//        std::cout << std::endl << "Blocks: ";
//        for (const auto& block : positions) {
//            std::cout << "(" << text[block.second] << ", " << block.first << ", " << block.second << "),";
//        }
//        std::cout << std::endl;

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

//            std::cout << std::endl << "sorted blocks: ";
//            for (const auto& block : positions) {
//                std::cout << "(" << text[block.second] << ", " << block.first << ", " << block.second << "),";
//            }
//            std::cout << std::endl << util::text_vector_to_string(text) << std::endl;


            auto nt_count = rlslp.non_terminals.size();
            auto next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);

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
//#pragma omp critical
//                    {
//                        std::cout << "i: " << i << " by " << thread_id << std::endl;
//                    }
                    if (positions[i].first != positions[i - 1].first ||
                        text[positions[i].second] != text[positions[i - 1].second]) {
//#pragma omp critical
//                        {
//                            std::cout << "found distinct: " << i << " by " << thread_id << std::endl;
//                        }
                        distinct_blocks[thread_id + 1]++;
                    }
                }

#pragma omp barrier
#pragma omp single
                {
                    for (size_t j = 1; j < distinct_blocks.size(); ++j) {
                        distinct_blocks[j] += distinct_blocks[j - 1];
                    }

//                    std::cout << "assign bounds: ";
//                    for (size_t k = 0; k < assign_bounds.size(); ++k) {
//                        std::cout << assign_bounds[k] << ", ";
//                    }
//                    std::cout << std::endl << "distinct blocks: ";
//                    for (size_t k = 0; k < distinct_blocks.size(); ++k) {
//                        std::cout << distinct_blocks[k] << ", ";
//                    }
//                    std::cout << std::endl;

                    auto bc = distinct_blocks[n_threads];
                    auto rlslp_size = nt_count + bc;
                    rlslp.reserve(rlslp_size);
                    rlslp.resize(rlslp_size);
                    rlslp.blocks += bc;
                    bv.resize(rlslp_size, true);

//                    std::cout << "bc: " << bc << std::endl;
//                    std::cout << "rlslp_size: " << rlslp_size << std::endl;
//                    std::cout << "next_nt: " << next_nt << std::endl;
//                    std::cout << "nt_count: " << nt_count << std::endl;
                }

                i = assign_bounds[thread_id];
                auto last_var = next_nt + distinct_blocks[thread_id] - 1;  // TODO: catch case that block with text[position[i-1]] = text[position[i]]
                variable_t last_char = 0;
                if (i > 0 && i < assign_bounds[thread_id + 1]) {
                    last_char = text[positions[i - 1].second];
                }
#pragma omp barrier

//#pragma omp critical
//                {
//                    std::cout << "last: " << last_var << ", i: " << i << " by " << thread_id << std::endl;
//                }

                size_t j = 0;
                if (thread_id == 0) {
                    last_char = text[positions[i].second];
                    auto b_len = positions[i].first;
                    auto len = b_len;
//#pragma omp critical
//                    {
//                        std::cout << "(" << last_char << ", " << b_len << ") by " << thread_id << std::endl;
//                    }
                    if (last_char >= rlslp.terminals) {
                        len *= rlslp[last_char - rlslp.terminals].len;
                    }
                    rlslp[nt_count + distinct_blocks[thread_id] + j] = recomp::rlslp<>::non_terminal(last_char, b_len,
                                                                                                     len);
                    j++;
                    last_var++;  // = next_nt + distinct_blocks[thread_id] + j;
                    text[positions[i].second] = last_var;
//#pragma omp critical
//                    {
//                        std::cout << "Set pos " << positions[i].second << " to " << last_var << " by " << thread_id << std::endl;
//                    }
                    for (size_t k = 1; k < b_len; ++k) {
                        text[positions[i].second + k] = DELETED;
                    }
                    i++;
                }

                for (; i < assign_bounds[thread_id + 1]; ++i) {
                    auto char_i = text[positions[i].second];
                    auto b_len = positions[i].first;
//#pragma omp critical
//                    {
//                        std::cout << "(" << char_i << ", " << b_len << "), (" << text[positions[i - 1].second] << ", " << positions[i - 1].first << ") by " << thread_id << std::endl;
//                    }
                    if (char_i == last_char && b_len == positions[i - 1].first) {
                        text[positions[i].second] = last_var;
//#pragma omp critical
//                        {
//                            std::cout << "Set pos " << positions[i].second << " to " << last_var << " by " << thread_id << std::endl;
//                        }
                    } else {
                        auto len = b_len;
                        if (char_i >= rlslp.terminals) {
                            len *= rlslp[char_i - rlslp.terminals].len;
                        }
                        rlslp[nt_count + distinct_blocks[thread_id] + j] = recomp::rlslp<>::non_terminal(char_i, b_len,
                                                                                                         len);
                        j++;
                        last_var++;  // = next_nt + distinct_blocks[thread_id] + j;
                        text[positions[i].second] = last_var;
                        last_char = char_i;
//#pragma omp critical
//                        {
//                            std::cout << "Set pos " << positions[i].second << " to " << last_var << " by " << thread_id << std::endl;
//                        }
                    }
                    for (size_t k = 1; k < b_len; ++k) {
                        text[positions[i].second + k] = DELETED;
                    }
                }
            }

#ifdef BENCH
        std::cout << " elements=" << distinct_blocks[distinct_blocks.size() - 1] << " blocks=" << block_count;
#endif

//#ifdef BENCH
//        const auto startTimeAss = recomp::timer::now();
//#endif
//        block_count = sort_blocks.size();
//        auto nt_count = rlslp.non_terminals.size();
//        rlslp.reserve(nt_count + block_count);
//        rlslp.resize(nt_count + block_count/*, true*/);
//        rlslp.blocks += block_count;
//        bv.resize(nt_count + block_count, true);
////        rlslp.block_count += block_count;
//
//        auto next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);
//
//#pragma omp parallel for schedule(static) num_threads(this->cores)
//        for (size_t i = 0; i < sort_blocks.size(); ++i) {
////            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_blocks[i].first << ","
////                       << sort_blocks[i].second << ") at index " << nt_count + i;
//            blocks[sort_blocks[i]] = next_nt + static_cast<variable_t>(i);
//            auto len = static_cast<size_t>(sort_blocks[i].second);
//            if (sort_blocks[i].first >= rlslp.terminals) {
//                len *= rlslp[sort_blocks[i].first - rlslp.terminals].len;
//            }
//            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_blocks[i].first, sort_blocks[i].second, len);
//        }
//#ifdef BENCH
//        const auto endTimeAss = recomp::timer::now();
//        const auto timeSpanAss = endTimeAss - startTimeAss;
//        std::cout << " block_rules="
//                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
//#endif


//        std::cout << std::endl << "compact_bounds: ";
//        for (size_t i = 0; i < compact_bounds.size(); ++i) {
//            std::cout << compact_bounds[i] << ", ";
//        }
//        std::cout << std::endl << "block_counts: ";
//        for (size_t i = 0; i < block_counts.size(); ++i) {
//            std::cout << block_counts[i] << ", ";
//        }
//        std::cout << std::endl << "positions: ";
//        for (size_t i = 0; i < positions.size(); ++i) {
//            for (size_t j = 0; j < positions[i].size(); ++j) {
//                std::cout << "(" << positions[i][j].first << ", " << positions[i][j].second << "), ";
//            }
//        }
//        std::cout << std::endl << "map: ";
//        for (const auto& b : blocks) {
//            std::cout << "(" << b.first.first << "," << b.first.second << ")->" << b.second << "; " << std::endl;
//        }
//
//        std::cout << util::text_vector_to_string(text) << std::endl;

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
        } else {
#ifdef BENCH
            std::cout << " sort=0 elements=0 blocks=0 compact_text=0";
#endif
        }

//        std::cout << std::endl << util::text_vector_to_string(text) << std::endl;

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
                if (partition.find(val) == partition.end()) {
                    partition[val] = false;
                }
                if (!partition[val]) {
                    l_count++;
                } else {
                    r_count++;
                }
            } else {
                val = text[adj_list[0] + 1];
                if (partition.find(val) == partition.end()) {
                    partition[val] = false;
                }
                if (!partition[val]) {
                    l_count++;
                } else {
                    r_count++;
                }
            }
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
//        if (rl_count > lr_count) {
//#ifdef BENCH
//            const auto startTimeSwap = recomp::timer::now();
//#endif
//#pragma omp parallel num_threads(this->cores)
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
                  << " level=" << this->level << " cores=" << this->cores;
#endif
        partition_t partition;
//        for (size_t i = 0; i < text.size(); ++i) {
//            partition[text[i]] = false;
//        }
//
//#ifdef BENCH
//        const auto endTimeCreate = recomp::timer::now();
//        const auto timeSpanCreate = endTimeCreate - startTime;
//        std::cout << " alphabet=" << partition.size() << " create_partition="
//                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCreate).count();
//#endif
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
//        std::vector<std::vector<pair_position_t>> positions;

        std::vector<size_t> bounds;
        std::vector<size_t> pair_counts;
        std::vector<size_t> compact_bounds;
        std::vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
//                positions.resize(n_threads);

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
//                    positions[thread_id].emplace_back(i);
                    pair_count++;
                    pair_counts[thread_id + 1]++;
                }
            }

//            bounds[thread_id + 1] = positions[thread_id].size();
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

#pragma omp parallel num_threads(this->cores)
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
        ips4o::parallel::sort(sort_pairs.begin(), sort_pairs.end(), std::less<pair_t>(), this->cores);
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

#pragma omp parallel for schedule(static) num_threads(this->cores)
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

#pragma omp parallel for schedule(static) num_threads(this->cores)
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

//        std::cout << std::endl << "compact_bounds: ";
//        for (size_t i = 0; i < compact_bounds.size(); ++i) {
//            std::cout << compact_bounds[i] << ", ";
//        }
//        std::cout << std::endl << "pair_counts: ";
//        for (size_t i = 0; i < pair_counts.size(); ++i) {
//            std::cout << pair_counts[i] << ", ";
//        }
//        std::cout << std::endl << "positions: ";
//        for (size_t i = 0; i < positions.size(); ++i) {
//            std::cout << "thread: " << i << "; ";
//            for (size_t j = 0; j < positions[i].size(); ++j) {
//                std::cout << positions[i][j] << ", " << std::endl;
//            }
//        }
//        std::cout << std::endl << "map: ";
//        for (const auto& b : pairs) {
//            std::cout << "(" << b.first.first << "," << b.first.second << ")->" << b.second << "; " << std::endl;
//        }
//
//        std::cout << util::text_vector_to_string(text) << std::endl;

#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = text.size() - pair_counts[pair_counts.size() - 1];  // positions.size();
        if (new_text_size > 1 && pair_count > 0) {
            text_t new_text;
            new_text.reserve(new_text_size);
            new_text.resize(new_text_size);
#pragma omp parallel num_threads(this->cores)
            {
                auto thread_id = omp_get_thread_num();
                size_t copy_i = pair_counts[thread_id];
//                size_t j = 0;
//                pair_position_t act_pos = text.size();
//                if (!positions[thread_id].empty()) {
//                    act_pos = positions[thread_id][j];
//                }
//                size_t i = compact_bounds[thread_id];
//                if (i < compact_bounds[thread_id + 1]) {
//#pragma omp critical
//                    {
//                        if (i > 0) {
//                            std::cout << "i: " << i << " last pos: "
//                                      << positions[thread_id - 1][positions[thread_id - 1].size() - 1] << " by "
//                                      << thread_id << std::endl;
//                        }
//                    }
//                    // TODO(Chris): problem if positions[thread - 1] is empty
//                    if (i > 0 && i - 1 == positions[thread_id - 1][positions[thread_id - 1].size() - 1]) {
//                        i++;
//                    }
//                }
//                for (; i < compact_bounds[thread_id + 1]; ++i) {
//#pragma omp critical
//                    {
//                        std::cout << "i: " << i << " thread: " << thread_id << std::endl;
//                    }
//                    if (j < positions[thread_id].size() && act_pos == i) {
//#pragma omp critical
//                        {
//                            std::cout << "Replace: i: " << i << ", text: " << text[i] << ", act: " << act_pos << " by "
//                                      << thread_id << ", copy: " << copy_i << std::endl;
//                        }
//                        new_text[copy_i++] = pairs[std::make_pair(text[i], text[i + 1])];
//                        i++;
//                        j++;
//                        if (j < positions[thread_id].size()) {
//                            act_pos = positions[thread_id][j];
//                        }
//                    } else {
////                    if (text[i] != DELETED) {
//#pragma omp critical
//                        {
//                            std::cout << "Copy: " << copy_i << ", i: " << i << ", text: " << text[i] << " by " << thread_id
//                                      << std::endl;
//                        }
//                        new_text[copy_i++] = text[i];
//                    }
//                }
//            }
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
            text = std::move(new_text);
        } else if (new_text_size == 1) {
//            text[0] = pairs[std::make_pair(text[0], text[1])];
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
};

}  // namespace parallel
}  // namespace recomp
