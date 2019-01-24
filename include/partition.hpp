
#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <glog/logging.h>
#include <ips4o.hpp>

#include "defs.hpp"
#include "util.hpp"

namespace recomp {

namespace parallel {
/**
 * @brief Computes a partitioning of the symbol in the text.
 *
 * @param adj_list[in] The adjacency list of the text
 * @param partition[out] The partition
 */
template<typename adj_list_t, typename partition_t>
inline void compute_partition(const adj_list_t& adj_list,
                              partition_t& partition,
                              const size_t cores = std::thread::hardware_concurrency()) {
#ifdef BENCH
    const auto startTime = std::chrono::system_clock::now();
#endif
//        DLOG(INFO) << util::text_vector_to_string(alphabet);

    int l_count = 0;
    int r_count = 0;
    if (adj_list.size() > 0) {
        if (partition[std::get<0>(adj_list[0])]) {
            r_count++;
        } else {
            l_count++;
        }
    }
    for (size_t i = 1; i < adj_list.size(); ++i) {
        if (std::get<0>(adj_list[i - 1]) < std::get<0>(adj_list[i])) {
//            LOG(INFO) << "Setting " << std::get<0>(adj_list[i - 1]) << " to " << (l_count > r_count) << " ; "
//                      << l_count << ", " << r_count;
            partition[std::get<0>(adj_list[i - 1])] = l_count > r_count;
            l_count = 0;
            r_count = 0;
        }
        if (partition[std::get<1>(adj_list[i])]) {
            r_count++;
        } else {
            l_count++;
        }
    }
    partition[std::get<0>(adj_list[adj_list.size() - 1])] = l_count > r_count;

//    LOG(INFO) << "Setting " << std::get<0>(adj_list[adj_list.size() - 1]) << " to " << (l_count > r_count) << " ; "
//              << l_count << ", " << r_count;
//        for (size_t i = 0; i < adj_list.size(); ++i) {
//            while (j < alphabet.size() && std::get<0>(adj_list[i]) > alphabet[j]) {
//                partition[alphabet[j]] = l_count > r_count;
////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
//                j++;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(adj_list[i])]) {
//                r_count++;
//            } else {
//                l_count++;
//            }
//        }
//        partition[alphabet[j]] = l_count > r_count;
#ifdef BENCH
    const auto endTimePar = std::chrono::system_clock::now();
    const auto timeSpanPar = endTimePar - startTime;
    std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
//    LOG(INFO) << "Partition: " << util::partition_to_string(partition);

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

//    LOG(INFO) << "rl: " << rl_count << ", lr: " << lr_count;
    if (rl_count > lr_count) {
//        LOG(INFO) << "Swap partition sets";
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
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
#ifdef BENCH
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
}


/**
 * @brief Computes a partitioning of the symbol in the text.
 *
 * @param adj_list[in] The adjacency list of the text
 * @param partition[out] The partition
 */
template<typename variable_t, typename adj_list_t, typename alphabet_t, typename partition_t>
inline void compute_partition_full_parallel(const adj_list_t& adj_list,
                                            partition_t& partition,
                                            const size_t cores = std::thread::hardware_concurrency()) {
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

    // TODO(Chris): parallelize histgram
    std::vector<size_t> hist(alphabet.size() + 1, 0);
    for (size_t i = 0; i < adj_list.size(); ++i) {
        hist[mapping[std::get<0>(adj_list[i])]]++;
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
            partition[std::get<0>(adj_list[index - 1])] = l_count > r_count;
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
//    mutexes.clear();

#ifdef BENCH
    const auto endTimePar = std::chrono::system_clock::now();
    const auto timeSpanPar = endTimePar - startTime;
    std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);

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
//        DLOG(INFO) << "Swap partition sets";
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
//    std::cout << std::endl << util::partition_to_string(partition) << std::endl;
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
#ifdef BENCH
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
}


/**
 * @brief Computes a partitioning of the symbol in the text.
 *
 * @param adj_list[in] The adjacency list of the text
 * @param partition[out] The partition
 */
template<typename adj_list_t, typename partition_t>
inline void compute_partition(const adj_list_t& adj_list,
                              partition_t& partition,
                              size_t& begin,
                              bool& part_l,
                              const size_t cores = std::thread::hardware_concurrency()) {
#ifdef BENCH
    const auto startTime = std::chrono::system_clock::now();
#endif
//        DLOG(INFO) << util::text_vector_to_string(alphabet);

    int l_count = 0;
    int r_count = 0;
    if (adj_list.size() > 0) {
        if (partition[std::get<1>(adj_list[0])]) {
            r_count++;
        } else {
            l_count++;
        }
    }
    for (size_t i = 1; i < adj_list.size(); ++i) {
        if (std::get<1>(adj_list[i - 1]) < std::get<1>(adj_list[i])) {
//            LOG(INFO) << "Setting " << std::get<0>(adj_list[i - 1]) << " to " << (l_count > r_count) << " ; "
//                      << l_count << ", " << r_count;
            partition[std::get<1>(adj_list[i - 1])] = l_count > r_count;
            l_count = 0;
            r_count = 0;
        }
        if (partition[std::get<2>(adj_list[i])]) {
            r_count++;
        } else {
            l_count++;
        }
    }
    partition[std::get<1>(adj_list[adj_list.size() - 1])] = l_count > r_count;

//    LOG(INFO) << "Setting " << std::get<0>(adj_list[adj_list.size() - 1]) << " to " << (l_count > r_count) << " ; "
//              << l_count << ", " << r_count;
//        for (size_t i = 0; i < adj_list.size(); ++i) {
//            while (j < alphabet.size() && std::get<0>(adj_list[i]) > alphabet[j]) {
//                partition[alphabet[j]] = l_count > r_count;
////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
//                j++;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(adj_list[i])]) {
//                r_count++;
//            } else {
//                l_count++;
//            }
//        }
//        partition[alphabet[j]] = l_count > r_count;
#ifdef BENCH
    const auto endTimePar = std::chrono::system_clock::now();
    const auto timeSpanPar = endTimePar - startTime;
    std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
//    LOG(INFO) << "Partition: " << util::partition_to_string(partition);

#ifdef BENCH
    const auto startTimeCount = std::chrono::system_clock::now();
#endif
    int lr_count = 0;
    int rl_count = 0;
#pragma omp parallel for num_threads(cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
    for (size_t i = 0; i < adj_list.size(); ++i) {
        if (std::get<0>(adj_list[i])) {
            if (!partition[std::get<1>(adj_list[i])] &&
                partition[std::get<2>(adj_list[i])]) {  // bc in text and b in right set and c in left
                rl_count++;
            } else if (partition[std::get<1>(adj_list[i])] &&
                       !partition[std::get<2>(adj_list[i])]) {  // bc in text and b in left set and c in right
                lr_count++;
            }
        } else {
            if (!partition[std::get<1>(adj_list[i])] &&
                partition[std::get<2>(adj_list[i])]) {  // cb in text and c in left set and b in right
                lr_count++;
            } else if (partition[std::get<1>(adj_list[i])] &&
                       !partition[std::get<2>(adj_list[i])]) {  // cb in text and c in right set and b in left
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

    part_l = lr_count >= rl_count;
////    LOG(INFO) << "rl: " << rl_count << ", lr: " << lr_count;
//    if (rl_count > lr_count) {
////        LOG(INFO) << "Swap partition sets";
//#pragma omp parallel num_threads(cores)
//        {
//#pragma omp single
//            {
//                for (auto iter = partition.begin(); iter != partition.end(); ++iter) {
//#pragma omp task
//                    {
//                        (*iter).second = !(*iter).second;
//                    }
//                }
//            }
//#pragma omp barrier
//        }
//    }
////        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
#ifdef BENCH
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
}

}  // namespace parallel

}  // namespace recomp
