
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
//
#include <glog/logging.h>
// #include <ips4o.hpp>

#include "defs.hpp"
#include "util.hpp"
#include "partition.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

#ifndef THREAD_COUNT
#define THREAD_COUNT std::thread::hardware_concurrency()
#endif

namespace recomp {

namespace parallel {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression {
 public:
    typedef std::vector<variable_t> text_t;
    typedef std::vector<variable_t> alphabet_t;
    typedef std::vector<std::tuple<variable_t, variable_t, bool>> multiset_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
//    typedef size_t position_t;
    typedef size_t pair_position_t;

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
     */
    void recomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, const terminal_count_t& alphabet_size) {
        DLOG(INFO) << "recomp input - text size: " << text.size() << " - alphabet size: "
                   << std::to_string(alphabet_size);
        const auto startTime = std::chrono::system_clock::now();

        bool not_finished = text.size() > 1;
        rlslp.terminals = alphabet_size;

        while (not_finished) {
//            std::cout << "bcomp" << std::endl;
            bcomp(text, rlslp);
//            std::cout << "bcomp finished" << std::endl;
            not_finished = text.size() > 1;

            if (not_finished) {
//                std::cout << "pcomp" << std::endl;
                pcomp(text, rlslp);
//                std::cout << "pcomp finished" << std::endl;
                not_finished = text.size() > 1;
            }
        }

        if (!rlslp.empty()) {
            rlslp.root = static_cast<variable_t>(rlslp.size() - 1);
        }

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for recomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void recomp(text_t& text, rlslp <variable_t, terminal_count_t>& rlslp) {
        recomp(text, rlslp, recomp::CHAR_ALPHABET);
    }


 private:
    const variable_t DELETED = UINT_MAX;

    /**
     * @brief Replaces all block in the text with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void bcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
        DLOG(INFO) << "BComp input - text size: " << text.size();
//        DLOG(INFO) << "Text: " << recomp::util::text_vector_to_string<text_t>(text);
        std::cout << " text=" << text.size();  // << " alphabet=" << alphabet.size();
        const auto startTime = std::chrono::system_clock::now();

        size_t block_count = 0;
        size_t substr_len = 0;

        const auto startTimeBlocks = std::chrono::system_clock::now();
        std::unordered_map<block_t, variable_t, pair_hash> blocks;
        std::vector<position_t> positions;

        std::vector<size_t> bounds;
//        size_t *bounds;
#pragma omp parallel num_threads(THREAD_COUNT)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            variable_t block_len = 1;

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
//                bounds = new size_t[n_threads + 1];
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
//                    t_positions.emplace_back(i - block_len + 1);
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
//        delete[] bounds;
        const auto endTimeBlocks = std::chrono::system_clock::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());

        DLOG(INFO) << "Blocks found: " << block_count;

//        DLOG(INFO) << "Blocks are " << recomp::util::blocks_to_string<block_t, variable_t>(blocks);

        const auto startTimeCopy = std::chrono::system_clock::now();
        std::vector<block_t> sort_blocks(blocks.size());

#pragma omp parallel num_threads(THREAD_COUNT)
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
        const auto endTimeCopy = std::chrono::system_clock::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_blocks=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());

        const auto startTimeSort = std::chrono::system_clock::now();
//    ips4o::parallel::sort(sort_blocks.begin(), sort_blocks.end());
        // __gnu_parallel::sort(sort_blocks.begin(), sort_blocks.end(), __gnu_parallel::multiway_mergesort_tag());

        parallel::partitioned_radix_sort(sort_blocks);
        const auto endTimeSort = std::chrono::system_clock::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_blocks.size() << " hits=" << block_count;;

//        DLOG(INFO) << "Sorted blocks are " << recomp::util::vector_blocks_to_string<block_t>(sort_blocks);
        DLOG(INFO) << "Number of different blocks: " << sort_blocks.size();

        const auto startTimeAss = std::chrono::system_clock::now();
        block_count = sort_blocks.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + block_count);
        rlslp.resize(nt_count + block_count, true);
        rlslp.block_count += block_count;

        auto next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
        for (size_t i = 0; i < sort_blocks.size(); ++i) {
//            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_blocks[i].first << ","
//                       << sort_blocks[i].second << ") at index " << nt_count + i;
            blocks[sort_blocks[i]] = next_nt + static_cast<variable_t>(i);
            auto len = static_cast<size_t>(sort_blocks[i].second);
            if (sort_blocks[i].first >= static_cast<variable_t>(rlslp.terminals)) {
                len *= rlslp[sort_blocks[i].first - rlslp.terminals].len;
            }
            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_blocks[i].first, sort_blocks[i].second, len);
        }

        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " block_rules=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
        DLOG(INFO) << "Time for block nts: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count()
                   << "[ms]";


        const auto startTimeRep = std::chrono::system_clock::now();

//        DLOG(INFO) << "Positions are " << recomp::util::block_positions_to_string<position_t>(positions);

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
        for (size_t i = 0; i < positions.size(); ++i) {
            auto block = std::make_pair(text[positions[i].second], positions[i].first);
            text[positions[i].second] = blocks[block];

            auto length = static_cast<size_t>(positions[i].first);
            for (size_t j = 1; j < length; ++j) {
                text[j + positions[i].second] = DELETED;
            }
        }
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_blocks=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
        DLOG(INFO) << "Time for replacing blocks: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count() << "[ms]";

        const auto startTimeCompact = std::chrono::system_clock::now();
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
        const auto endTimeCompact = std::chrono::system_clock::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());

        DLOG(INFO) << "Shrinking text by " << substr_len << " from length " << text.size() << " to length "
                   << new_text_size;

        text.resize(new_text_size);
        text.shrink_to_fit();

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for bcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";

// #ifdef DEBUG
//        if (text.size() < 30) {
//            DLOG(INFO) << "Text: " << recomp::util::text_vector_to_string<text_t>(text);
//        }
// #endif

        std::cout << " comp_text=" << text.size();
        DLOG(INFO) << "BComp ouput - text size: " << text.size() << " - distinct blocks: " << block_count
                   << " - string length reduce by: " << substr_len;
    }

    /**
     * @brief Computes the adjacency list of the text.
     *
     * @param text The text
     * @param multiset[out] The adjacency list
     */
    inline void compute_multiset(const text_t& text, multiset_t& multiset) {
//        std::cout << "multiset" << std::endl;
        const auto startTime = std::chrono::system_clock::now();

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
        for (size_t i = 0; i < multiset.size(); ++i) {
            if (text[i] > text[i + 1]) {
                multiset[i] = std::make_tuple(text[i], text[i + 1], false);
            } else {
                multiset[i] = std::make_tuple(text[i + 1], text[i], true);
            }
        }


        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " multiset=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        DLOG(INFO) << "Time for computing multiset: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
//        std::cout << "multiset finished" << std::endl;
    }

//    /**
//     * @brief Computes a partitioning of the symbol in the text.
//     *
//     * @param multiset[in] The adjacency list of the text
//     * @param alphabet[in] The effective alphabet
//     * @param partition[out] The partition
//     */
//    inline void compute_partition(const multiset_t& multiset, const alphabet_t& alphabet, partition_t& partition) {
////        std::cout << "partition" << std::endl;
//        const auto startTime = std::chrono::system_clock::now();
//
////        DLOG(INFO) << util::text_vector_to_string(alphabet);
//
////        std::unordered_map<variable_t, size_t> mapping;
////        std::unordered_map<variable_t, bool> locks;
////#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
////        for (size_t i = 0; i < alphabet.size(); ++i) {
////#pragma omp critical
////            {
////                mapping[alphabet[i]] = i;
////                locks[alphabet[i]] = false;
////            }
////        }
////
//////        std::cout << "Locks: " << std::endl;
//////        for (const auto& lock : locks) {
//////            std::cout << lock.first << ", " << lock.second << std::endl;
//////        }
////
//////        std::cout << std::endl;
////        // TODO(Chris): parallelize histgram
////        std::vector<size_t> hist(alphabet.size() + 1, 0);
////        for (size_t i = 0; i < multiset.size(); ++i) {
//////            std::cout << std::get<0>(multiset[i]) << " ";
////            hist[mapping[std::get<0>(multiset[i])]]++;
////        }
//////        std::cout << std::endl;
//////
//////        for (size_t i = 0; i < multiset.size(); ++i) {
//////            std::cout << std::get<1>(multiset[i]) << " ";
//////        }
//////        std::cout << std::endl;
////
////        std::vector<size_t> starts;
////        starts.push_back(0);
////        size_t j = 0;
////        for (size_t i = 0; i < hist.size(); ++i) {
////            if (hist[i] > 0) {
////                starts.push_back(starts[j] + hist[i]);
////            }
////        }
////
//////        std::cout << "starts: " << starts[0] << ", ";
////        for (size_t i = 1; i < starts.size(); ++i) {
//////            std::cout << starts[i] << ", ";
////            starts[i] += starts[i - 1];
////        }
//////        std::cout << std::endl;
//////
//////        std::cout << "starts: ";
//////        for (size_t i = 0; i < starts.size(); ++i) {
//////            std::cout << starts[i] << ", ";
//////        }
//////        std::cout << std::endl;
//////
//////
//////
////////        std::unordered_map<variable_t, omp_lock_t> locks{};
////////        for (size_t i = 0; i < alphabet.size(); ++i) {
////////            omp_lock_t lock;
////////            locks[alphabet[i]] = lock;
////////            omp_init_lock(&locks[alphabet[i]]);
////////        }
////
////        std::vector<size_t> bounds;
////#pragma omp parallel num_threads(THREAD_COUNT)
////        {
////            auto thread_id = omp_get_thread_num();
////            auto n_threads = static_cast<size_t>(omp_get_num_threads());
////
////#pragma omp barrier
////#pragma omp single
////            {
////                bounds.reserve(n_threads + 1);
////                bounds.resize(n_threads + 1, starts.size() - 1);
////
////                size_t step = starts.size() / n_threads;
////                if (starts.size() < n_threads) {
////                    step = 1;
////                    n_threads = starts.size();
////                }
////                bounds[0] = 0;
//////                std::cout << "bounds: 0, ";
////                for (size_t i = 1; i < n_threads; ++i) {
////                    bounds[i] = bounds[i-1] + step;
//////                    std::cout << bounds[i] << ", ";
////                }
//////                bounds[n_threads] = starts.size() - 1;
//////                std::cout << bounds[omp_get_num_threads()] << ", " << std::endl;
////            }
////
////            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
////// #pragma omp critical
//////                {
//////                    std::cout << "Locking: " << std::get<0>(multiset[starts[i]]) << " by thread " << thread_id
//////                              << " i: " << i << std::endl;
//////                }
////                auto found = locks.find(std::get<0>(multiset[starts[i]]));
////                (*found).second = !(*found).second;
//////                locks[std::get<0>(multiset[starts[i]])] = true;
////
//////                omp_set_lock(&locks[std::get<0>(multiset[starts[i]])]);
////            }
////#pragma omp barrier
////// #pragma omp single
//////            {
//////                std::cout << "All locks aquired" << std::endl;
//////                for (const auto& lock : locks) {
//////                    std::cout << lock.first << ", " << lock.second << std::endl;
//////                }
//////            }
////
////            int l_count = 0;
////            int r_count = 0;
////            size_t index = 0;
////            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
////                index = starts[i];
////
////                while (index < starts[i + 1]) {
////// #pragma omp critical
//////                    {
//////                        std::cout << "Test for: " << std::get<1>(multiset[index]) << " by thread " << thread_id
//////                                  << std::endl;
//////                    }
////                    bool locked = false;
////#pragma omp critical
////                    {
////                        locked = locks[std::get<1>(multiset[index])];
////                    }
////                    while (locked) {
////#pragma omp critical
////                        {
////                            locked = locks[std::get<1>(multiset[index])];
////                        }
////                    }
//////                    while (!omp_test_lock(&locks[std::get<1>(multiset[index])])) {
//////                        std::cout << "Waiting thread: " << thread_id << std::endl;
//////                    }
////                    bool check = false;
////#pragma omp critical
////                    {
////                        check = partition[std::get<1>(multiset[index])];
////                    }
////                    if (check) {
////                        r_count++;
////                    } else {
////                        l_count++;
////                    }
////                    index++;
////                }
////#pragma omp critical
////                {
////                    partition[std::get<0>(multiset[index - 1])] = l_count > r_count;
////                }
////                l_count = 0;
////                r_count = 0;
////// #pragma omp critical
//////                {
//////                    std::cout << "Releasing: " << std::get<0>(multiset[index - 1]) << " by thread " << thread_id
//////                              << std::endl;
//////                }
////#pragma omp critical
////                {
////                    locks[std::get<0>(multiset[index - 1])] = false;
////                }
//////                omp_unset_lock(&locks[std::get<0>(multiset[index - 1])]);
////            }
////
////#pragma omp barrier
////        }
//////        delete[] bounds;
////
//////        for (size_t i = 0; i < alphabet.size(); ++i) {
//////            omp_destroy_lock(&locks[alphabet[i]]);
//////        }
//
//        int l_count = 0;
//        int r_count = 0;
//        size_t j = 0;
//        for (size_t i = 0; i < multiset.size() - 1; ++i) {
//            while (i < multiset.size() - 1 && std::get<0>(multiset[i]) < std::get<0>(multiset[i + 1])) {
//                if (partition[std::get<1>(multiset[i])]) {
//                    r_count++;
//                } else {
//                    l_count++;
//                }
//                i++;
//            }
//            partition[std::get<0>(multiset[i - 1])] = l_count > r_count;
////            while (j < alphabet.size() && std::get<0>(multiset[i]) > alphabet[j]) {
////                partition[alphabet[j]] = l_count > r_count;
//////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
////                j++;
////                l_count = 0;
////                r_count = 0;
////            }
//
//        }
////        for (size_t i = 0; i < multiset.size(); ++i) {
////            while (j < alphabet.size() && std::get<0>(multiset[i]) > alphabet[j]) {
////                partition[alphabet[j]] = l_count > r_count;
//////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
////                j++;
////                l_count = 0;
////                r_count = 0;
////            }
////            if (partition[std::get<1>(multiset[i])]) {
////                r_count++;
////            } else {
////                l_count++;
////            }
////        }
////        partition[alphabet[j]] = l_count > r_count;
//        const auto endTimePar = std::chrono::system_clock::now();
//        const auto timeSpanPar = endTimePar - startTime;
//        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
//        DLOG(INFO) << "j: " << j;
////        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
//
//
//        const auto startTimeCount = std::chrono::system_clock::now();
//        int lr_count = 0;
//        int rl_count = 0;
//#pragma omp parallel for num_threads(THREAD_COUNT) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
//        for (size_t i = 0; i < multiset.size(); ++i) {
//            if (std::get<2>(multiset[i])) {
//                if (!partition[std::get<0>(multiset[i])] &&
//                    partition[std::get<1>(multiset[i])]) {  // bc in text and b in right set and c in left
//                    rl_count++;
//                } else if (partition[std::get<0>(multiset[i])] &&
//                           !partition[std::get<1>(multiset[i])]) {  // bc in text and b in left set and c in right
//                    lr_count++;
//                }
//            } else {
//                if (!partition[std::get<0>(multiset[i])] &&
//                    partition[std::get<1>(multiset[i])]) {  // cb in text and c in left set and b in right
//                    lr_count++;
//                } else if (partition[std::get<0>(multiset[i])] &&
//                           !partition[std::get<1>(multiset[i])]) {  // cb in text and c in right set and b in left
//                    rl_count++;
//                }
//            }
//        }
//        const auto endTimeCount = std::chrono::system_clock::now();
//        const auto timeSpanCount = endTimeCount - startTimeCount;
//        std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
//
//        if (rl_count > lr_count) {
//            DLOG(INFO) << "Swap partition sets";
//#pragma omp parallel num_threads(THREAD_COUNT)
//            {
//#pragma omp single
//                {
//                    for (auto iter = partition.begin(); iter != partition.end(); ++iter) {
//#pragma omp task
//                        {
//                            (*iter).second = !(*iter).second;
////                            ++iter;
//                        }
//                    }
//                }
//#pragma omp barrier
//            }
////            auto iter = partition.begin();
////            for (size_t i = 0; i < partition.size(); ++i) {
////    //            if (iter == partition.begin()) {
////    //                std::advance(iter, i);
////    //            }
////                (*iter).second = !(*iter).second;
////                ++iter;
////            }
////            DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
//        }
////        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
//
//        const auto endTime = std::chrono::system_clock::now();
//        const auto timeSpan = endTime - startTime;
//        std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
//        DLOG(INFO) << "Time for computing partition: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
//
////        std::cout << "partition finished" << std::endl;
//    }

/**
 * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
 *
 * @param text The text
 * @param rlslp The rlslp
 */
    void pcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
        DLOG(INFO) << "PComp input - text size: " << text.size();
        const auto startTime = std::chrono::system_clock::now();

        partition_t partition;
        for (size_t i = 0; i < text.size(); ++i) {
            partition[text[i]] = false;
        }
        alphabet_t alphabet(partition.size());
        auto iter = partition.begin();
        for (size_t i = 0; i < partition.size(); ++i, ++iter) {
            alphabet[i] = (*iter).first;
        }
        __gnu_parallel::sort(alphabet.begin(), alphabet.end(), __gnu_parallel::multiway_mergesort_tag());

        DLOG(INFO) << "Text: " << util::text_vector_to_string(text);

        std::cout << " text=" << text.size() << " alphabet=" << alphabet.size();


        multiset_t multiset(text.size() - 1);
        compute_multiset(text, multiset);
        __gnu_parallel::sort(multiset.begin(), multiset.end(), __gnu_parallel::multiway_mergesort_tag());

        size_t pair_count = 0;


        compute_partition<multiset_t, partition_t>(multiset, /*alphabet,*/ partition);

        const auto startTimePairs = std::chrono::system_clock::now();
        std::unordered_map<pair_t, variable_t, pair_hash> pairs;
        std::vector<pair_position_t> positions;

        std::vector<size_t> bounds;
//        size_t *bounds;
#pragma omp parallel num_threads(THREAD_COUNT)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1);
//                bounds = new size_t[n_threads + 1];
                bounds[0] = 0;
            }
            std::vector<pair_position_t> t_positions;
            std::unordered_map<pair_t, variable_t, pair_hash> t_pairs;
            //        size_t begin = 0;
            //        bool add = false;

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

//            DLOG(INFO) << "Inserting pairs " << util::blocks_to_string(t_pairs);

#pragma omp critical
            pairs.insert(t_pairs.begin(), t_pairs.end());
        }
//        delete[] bounds;
        const auto endTimePairs = std::chrono::system_clock::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());

        DLOG(INFO) << "Pairs found: " << pair_count;

//        DLOG(INFO) << "Pairs are " << util::blocks_to_string(pairs);

        const auto startTimeCopy = std::chrono::system_clock::now();
        std::vector<pair_t> sort_pairs(pairs.size());

#pragma omp parallel num_threads(THREAD_COUNT)
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
        const auto endTimeCopy = std::chrono::system_clock::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_pairs=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());

        const auto startTimeSort = std::chrono::system_clock::now();

        // ips4o::parallel::sort(sort_blocks.begin(), sort_blocks.end());
        // __gnu_parallel::sort(sort_blocks.begin(), sort_blocks.end(), __gnu_parallel::multiway_mergesort_tag());
        parallel::partitioned_radix_sort(sort_pairs);

        const auto endTimeSort = std::chrono::system_clock::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count()) << " elements=" << sort_pairs.size() << " hits=" << pair_count;

//        DLOG(INFO) << "Sorted pairs are " << util::vector_blocks_to_string(sort_pairs);

        DLOG(INFO) << "Number of different pairs: " << sort_pairs.size();

        const auto startTimeAss = std::chrono::system_clock::now();
        pair_count = sort_pairs.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + pair_count);
        rlslp.resize(nt_count + pair_count);

        variable_t next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
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

        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " pair_rules=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
        DLOG(INFO) << "Time for pair nts: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count()
                   << "[ms]";


        const auto startTimeRep = std::chrono::system_clock::now();
//        DLOG(INFO) << "Positions are " << util::pair_positions_to_string(positions);

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
        for (size_t i = 0; i < positions.size(); ++i) {
            auto pos = positions[i];
            auto pair = std::make_pair(text[pos], text[pos + 1]);
            text[pos] = pairs[pair];

            text[pos + 1] = DELETED;
        }
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_pairs=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
        DLOG(INFO) << "Time for replacing pairs: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count() << "[ms]";

        const auto startTimeCompact = std::chrono::system_clock::now();
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
        const auto endTimeCompact = std::chrono::system_clock::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());

        DLOG(INFO) << "Shrinking text by " << positions.size() << " from length " << text.size() << " to length "
                   << new_text_size;

        text.resize(new_text_size);
        text.shrink_to_fit();

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for pcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";
#ifdef DEBUG
        if (text.size() < 30) {
            DLOG(INFO) << "Text: " << util::text_vector_to_string(text);
        }
#endif

        std::cout << " comp_text=" << text.size();
        DLOG(INFO) << "PComp ouput - text size: " << text.size() << " - distinct pairs: " << pair_count
                   << " - string length reduce by: " << positions.size();
    }
};

}  // namespace parallel
}  // namespace recomp
