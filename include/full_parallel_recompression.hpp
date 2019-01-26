
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

//#include <glog/logging.h>
#include <ips4o.hpp>

#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression_full {
 public:
    typedef std::vector<variable_t> text_t;
    typedef std::vector<variable_t> alphabet_t;
    typedef std::vector<bool> bitvector_t;
    typedef std::vector<std::tuple<variable_t, variable_t, bool>> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;

    std::string dataset = "data";

    size_t level = 0;
    size_t cores = 1;

    recompression_full() = default;

    recompression_full(std::string& dataset) : dataset(dataset) {}

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
     */
    void recomp(text_t& text,
                rlslp<variable_t, terminal_count_t>& rlslp,
                const terminal_count_t& alphabet_size,
                const size_t cores = std::thread::hardware_concurrency()) {
#ifdef BENCH_RECOMP
        const auto startTime = std::chrono::system_clock::now();
#endif
        this->cores = cores;
        rlslp.terminals = alphabet_size;

        while (text.size() > 1) {
            bcomp(text, rlslp);
            level++;

            if (text.size() > 1) {
                pcomp(text, rlslp);
                level++;
            }
        }

        if (!rlslp.empty()) {
            rlslp.root = static_cast<variable_t>(rlslp.size() - 1);
        }

#ifdef BENCH_RECOMP
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=parallel_recompression dataset=" << dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << level
                  << " cores=" << cores << std::endl;
#endif
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void recomp(text_t& text,
                rlslp <variable_t, terminal_count_t>& rlslp,
                const size_t cores = std::thread::hardware_concurrency()) {
        recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
    }


 private:
    const variable_t DELETED = UINT_MAX;

    /**
     * @brief Replaces all block in the text with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    inline void bcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
//        DLOG(INFO) << "Text: " << recomp::util::text_vector_to_string<text_t>(text);
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
        std::cout << "RESULT algo=parallel_bcomp dataset=" << dataset << " text=" << text.size()
                  << " level=" << level << " cores=" << cores;
#endif

        size_t block_count = 0;
        size_t substr_len = 0;

#ifdef BENCH
        const auto startTimeBlocks = std::chrono::system_clock::now();
#endif
        std::unordered_map<block_t, variable_t, pair_hash> blocks;
        std::vector<position_t> positions;

        std::vector<size_t> bounds;
#pragma omp parallel num_threads(cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            variable_t block_len = 1;

#pragma omp single
            {
                std::cout << " used_cores=" << n_threads;
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
            }
            std::vector<position_t> t_positions;
            std::unordered_map<block_t, variable_t, pair_hash> t_blocks;
            size_t begin = 0;
            bool add = false;

#pragma omp for schedule(static) nowait reduction(+:block_count) reduction(+:substr_len)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                if (begin == 0) {
//                    DLOG(INFO) << "begin at " << i << " for thread " << thread_id;
                    begin = i;
                    if (i == 0) {
                        begin = 1;
                    }
                    add = !(begin > 1 && text[begin - 1] == text[begin]);
                }
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
                    block_len++;
                    i++;
                }
                if (!add) {
//                    DLOG(INFO) << "skipping block (" << text[i] << "," << block_len << ")";
                    block_len = 1;
                    add = true;
                }
                if (block_len > 1) {
                    substr_len += block_len - 1;
//                    DLOG(INFO) << "Block (" << text[i] << "," << block_len << ") found at " << (i - block_len + 1)
//                               << " by thread " << thread_id;
                    t_positions.emplace_back(block_len, i - block_len + 1);
                    block_t block = std::make_pair(text[i], block_len);
                    t_blocks[block] = 1;
                    block_count++;
                    block_len = 1;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < n_threads + 1; ++i) {
                    bounds[i] += bounds[i - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

//            DLOG(INFO) << "Thread " << thread_id << " inserting blocks "
//                       << recomp::util::blocks_to_string<block_t, variable_t>(t_blocks);

#pragma omp critical
            blocks.insert(t_blocks.begin(), t_blocks.end());
        }
#ifdef BENCH
        const auto endTimeBlocks = std::chrono::system_clock::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());
#endif

//        DLOG(INFO) << "Blocks found: " << block_count;
//        DLOG(INFO) << "Blocks are " << recomp::util::blocks_to_string<block_t, variable_t>(blocks);

#ifdef BENCH
        const auto startTimeCopy = std::chrono::system_clock::now();
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
        const auto endTimeCopy = std::chrono::system_clock::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
#endif

#ifdef BENCH
        const auto startTimeSort = std::chrono::system_clock::now();
#endif
//        parallel::partitioned_radix_sort(sort_blocks);
        ips4o::parallel::sort(sort_blocks.begin(), sort_blocks.end());
#ifdef BENCH
        const auto endTimeSort = std::chrono::system_clock::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_blocks.size() << " blocks=" << block_count;;
#endif

//        DLOG(INFO) << "Sorted blocks are " << recomp::util::vector_blocks_to_string<block_t>(sort_blocks);
//        DLOG(INFO) << "Number of different blocks: " << sort_blocks.size();

#ifdef BENCH
        const auto startTimeAss = std::chrono::system_clock::now();
#endif
        block_count = sort_blocks.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + block_count);
        rlslp.resize(nt_count + block_count, true);
        rlslp.block_count += block_count;

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
        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " block_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif
//        DLOG(INFO) << "Time for block nts: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count()
//                   << "[ms]";


#ifdef BENCH
        const auto startTimeRep = std::chrono::system_clock::now();
#endif

//        DLOG(INFO) << "Positions are " << recomp::util::block_positions_to_string<position_t>(positions);

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
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif

#ifdef BENCH
        const auto startTimeCompact = std::chrono::system_clock::now();
#endif
        size_t new_text_size = text.size() - substr_len;
        if (new_text_size > 1 && block_count > 0) {
            size_t copy_i = positions[0].second + 1;
            size_t i = positions[0].second + positions[0].first;  // jump to first position to copy

            for (; i < text.size(); ++i) {
                if (text[i] != DELETED) {
                    text[copy_i++] = text[i];
                }
            }
        }
#ifdef BENCH
        const auto endTimeCompact = std::chrono::system_clock::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

        text.resize(new_text_size);
        text.shrink_to_fit();

#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
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
        const auto startTime = std::chrono::system_clock::now();
#endif

#pragma omp parallel for schedule(static) num_threads(cores)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (text[i] > text[i + 1]) {
                adj_list[i] = std::make_tuple(text[i], text[i + 1], false);
            } else {
                adj_list[i] = std::make_tuple(text[i + 1], text[i], true);
            }
        }

#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
    }

    /**
     * @brief Computes a partitioning of the symbol in the text.
     *
     * @param adj_list[in] The adjacency list of the text
     * @param partition[out] The partition
     */
    inline void compute_partition(const adj_list_t& adj_list, partition_t& partition) {
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
#endif

        alphabet_t alphabet(partition.size());
#pragma omp parallel num_threads(cores)
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
        ips4o::parallel::sort(alphabet.begin(), alphabet.end());

        std::vector<std::shared_timed_mutex> mutexes(alphabet.size());
        std::unordered_map<variable_t, size_t> mapping;
#pragma omp parallel for schedule(static) num_threads(cores)
        for (size_t i = 0; i < alphabet.size(); ++i) {
#pragma omp critical
            {
                mapping[alphabet[i]] = i;
            }
        }

        std::vector<size_t> hist(alphabet.size() + 1, 0);
#pragma omp parallel num_threads(cores)
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
#pragma omp parallel num_threads(cores)
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
// #pragma omp critical
//            {std::cout << "Locking write " << std::get<0>(adj_list[starts[i]]) << " by thread " << thread_id
//                       << std::endl;}
                mutexes[mapping[std::get<0>(adj_list[starts[i]])]].lock();
// #pragma omp critical
//            {std::cout << "Locked write " << std::get<0>(adj_list[starts[i]]) << " by thread " << thread_id
//                       << std::endl;}
            }
#pragma omp barrier

            int l_count = 0;
            int r_count = 0;
            size_t index = 0;
            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
                index = starts[i];

                while (index < starts[i + 1]) {
// #pragma omp critical
//                {std::cout << "Index " << index << " to " << starts[i+1] << " by thread " << thread_id << " char "
//                           << mapping[std::get<0>(adj_list[index])] << std::endl;}

// #pragma omp critical
//                {std::cout << "Locking shared " << std::get<1>(adj_list[index]) << " by thread " << thread_id
//                           << std::endl;}
                    mutexes[mapping[std::get<1>(adj_list[index])]].lock_shared();
// #pragma omp critical
//                {std::cout << "Locked shared " << std::get<1>(adj_list[index]) << " by thread " << thread_id
//                           << std::endl;}
                    if (partition[std::get<1>(adj_list[index])]) {
                        r_count++;
                    } else {
                        l_count++;
                    }
// #pragma omp critical
//                {std::cout << "Unlocking shared " << std::get<1>(adj_list[index]) << " by thread " << thread_id
//                           << std::endl;}
                    mutexes[mapping[std::get<1>(adj_list[index])]].unlock_shared();
// #pragma omp critical
//                {std::cout << "Unlocked shared " << std::get<1>(adj_list[index]) << " by thread " << thread_id
//                           << std::endl;}
                    index++;
                }
// #pragma omp critical
//            {std::cout << "finished " << std::get<0>(adj_list[index - 1]) << " by thread " << thread_id
//                       << " at index " << (index-1) << std::endl;}
#pragma omp critical
                {
                    partition[std::get<0>(adj_list[index - 1])] = l_count > r_count;
                }
// #pragma omp critical
//            {std::cout << "Unlocking write " << std::get<0>(adj_list[index - 1]) << " by thread " << thread_id
//                       << std::endl;}
                mutexes[mapping[std::get<0>(adj_list[index - 1])]].unlock();
// #pragma omp critical
//            {std::cout << "Unlocked write " << std::get<0>(adj_list[index - 1]) << " by thread " << thread_id
//                       << std::endl;}

                l_count = 0;
                r_count = 0;
            }
        }

#ifdef BENCH
        const auto endTimePar = std::chrono::system_clock::now();
        const auto timeSpanPar = endTimePar - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif

#ifdef BENCH
        const auto startTimeCount = std::chrono::system_clock::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
#pragma omp parallel for num_threads(cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
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
#ifdef BENCH
        const auto endTimeCount = std::chrono::system_clock::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif

        if (rl_count > lr_count) {
#pragma omp parallel num_threads(cores)
            {
#pragma omp single
                {
                    for (auto iter = partition.begin(); iter != partition.end(); ++iter) {
#pragma omp task
                        {
                            (*iter).second = !(*iter).second;
                        }
                    }
                }
#pragma omp barrier
            }
        }
#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
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
    inline void pcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
        std::cout << "RESULT algo=parallel_pcomp dataset=" << dataset << " text=" << text.size() << " level=" << level
                  << " cores=" << cores;
#endif
        partition_t partition;
        for (size_t i = 0; i < text.size(); ++i) {
            partition[text[i]] = false;
        }

#ifdef BENCH
        std::cout << " alphabet=" << partition.size();
#endif
        adj_list_t adj_list(text.size() - 1);
        compute_adj_list(text, adj_list);

#ifdef BENCH
        const auto startTimeMult = std::chrono::system_clock::now();
#endif
//        partitioned_radix_sort(adj_list);
        ips4o::parallel::sort(adj_list.begin(), adj_list.end());
#ifdef BENCH
        const auto endTimeMult = std::chrono::system_clock::now();
        const auto timeSpanMult = endTimeMult - startTimeMult;
        std::cout << " sort_adj_list="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMult).count());
#endif

        size_t pair_count = 0;

        compute_partition(adj_list, partition);

#ifdef BENCH
        const auto startTimePairs = std::chrono::system_clock::now();
#endif
        std::unordered_map<pair_t, variable_t, pair_hash> pairs;
        std::vector<pair_position_t> positions;

        std::vector<size_t> bounds;
#pragma omp parallel num_threads(cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
            }
            std::vector<pair_position_t> t_positions;
            std::unordered_map<pair_t, variable_t, pair_hash> t_pairs;

#pragma omp for schedule(static) nowait reduction(+:pair_count)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                if (!partition[text[i]] && partition[text[i + 1]]) {
//                    DLOG(INFO) << "Pair (" << text[i] << "," << text[i + 1] << ") found at " << i << " by thread "
//                               << thread_id;
                    auto pair = std::make_pair(text[i], text[i + 1]);
                    t_pairs[pair] = 1;
                    t_positions.emplace_back(i);
                    pair_count++;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < n_threads + 1; ++i) {
                    bounds[i] += bounds[i - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

#pragma omp critical
            pairs.insert(t_pairs.begin(), t_pairs.end());
        }
#ifdef BENCH
        const auto endTimePairs = std::chrono::system_clock::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());
#endif

#ifdef BENCH
        const auto startTimeCopy = std::chrono::system_clock::now();
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
        const auto endTimeCopy = std::chrono::system_clock::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_pairs=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
#endif

#ifdef BENCH
        const auto startTimeSort = std::chrono::system_clock::now();
#endif
//        parallel::partitioned_radix_sort(sort_pairs);
        ips4o::parallel::sort(sort_pairs.begin(), sort_pairs.end());
#ifdef BENCH
        const auto endTimeSort = std::chrono::system_clock::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_pairs.size() << " pairs=" << pair_count;
#endif

#ifdef BENCH
        const auto startTimeAss = std::chrono::system_clock::now();
#endif
        pair_count = sort_pairs.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + pair_count);
        rlslp.resize(nt_count + pair_count);

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
        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " pair_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif
//        DLOG(INFO) << "Time for pair nts: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count()
//                   << "[ms]";

//        std::cout << std::endl;
//        for (const auto& p : sort_pairs) {
//            std::cout << p.first << ", " << p.second << ": " << pairs[p] << std::endl;
//        }

#ifdef BENCH
        const auto startTimeRep = std::chrono::system_clock::now();
#endif
//        DLOG(INFO) << "Positions are " << util::pair_positions_to_string(positions);

#pragma omp parallel for schedule(static) num_threads(cores)
        for (size_t i = 0; i < positions.size(); ++i) {
            auto pos = positions[i];
            auto pair = std::make_pair(text[pos], text[pos + 1]);
            text[pos] = pairs[pair];

            text[pos + 1] = DELETED;
        }
#ifdef BENCH
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif

#ifdef BENCH
        const auto startTimeCompact = std::chrono::system_clock::now();
#endif
        size_t new_text_size = text.size() - positions.size();
        if (new_text_size > 1 && pair_count > 0) {
            size_t copy_i = positions[0] + 1;
            size_t i = positions[0] + 2;  // jump to first position to copy

            for (; i < text.size(); ++i) {
                if (text[i] != DELETED) {
                    text[copy_i++] = text[i];
                }
            }
        }
#ifdef BENCH
        const auto endTimeCompact = std::chrono::system_clock::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

        text.resize(new_text_size);
        text.shrink_to_fit();

#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif

//        std::cout << std::endl << util::text_vector_to_string(text) << std::endl;
    }
};

}  // namespace parallel
}  // namespace recomp
