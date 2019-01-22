
#pragma once

#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include <glog/logging.h>
#include <ips4o.hpp>

#include "defs.hpp"
#include "util.hpp"

//#ifndef THREAD_COUNT
//#define THREAD_COUNT std::thread::hardware_concurrency()
//#endif

namespace recomp {

namespace parallel {
/**
 * @brief Computes a partitioning of the symbol in the text.
 *
 * @param multiset[in] The adjacency list of the text
 * @param alphabet[in] The effective alphabet
 * @param partition[out] The partition
 */
template<typename multiset_t/*, typename alphabet_t*/, typename partition_t>
inline void compute_partition(const multiset_t& multiset, /*const alphabet_t& alphabet,*/
                              partition_t& partition,
                              const size_t cores = std::thread::hardware_concurrency()) {
#ifdef BENCH
    const auto startTime = std::chrono::system_clock::now();
#endif
//        DLOG(INFO) << util::text_vector_to_string(alphabet);

    int l_count = 0;
    int r_count = 0;
    if (multiset.size() > 0) {
        if (partition[std::get<0>(multiset[0])]) {
            r_count++;
        } else {
            l_count++;
        }
    }
    for (size_t i = 1; i < multiset.size(); ++i) {
        if (std::get<0>(multiset[i - 1]) < std::get<0>(multiset[i])) {
//            LOG(INFO) << "Setting " << std::get<0>(multiset[i - 1]) << " to " << (l_count > r_count) << " ; "
//                      << l_count << ", " << r_count;
            partition[std::get<0>(multiset[i - 1])] = l_count > r_count;
            l_count = 0;
            r_count = 0;
        }
        if (partition[std::get<1>(multiset[i])]) {
            r_count++;
        } else {
            l_count++;
        }
    }
    partition[std::get<0>(multiset[multiset.size() - 1])] = l_count > r_count;

//    LOG(INFO) << "Setting " << std::get<0>(multiset[multiset.size() - 1]) << " to " << (l_count > r_count) << " ; "
//              << l_count << ", " << r_count;
//        for (size_t i = 0; i < multiset.size(); ++i) {
//            while (j < alphabet.size() && std::get<0>(multiset[i]) > alphabet[j]) {
//                partition[alphabet[j]] = l_count > r_count;
////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
//                j++;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(multiset[i])]) {
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
    for (size_t i = 0; i < multiset.size(); ++i) {
        if (std::get<2>(multiset[i])) {
            if (!partition[std::get<0>(multiset[i])] &&
                partition[std::get<1>(multiset[i])]) {  // bc in text and b in right set and c in left
                rl_count++;
            } else if (partition[std::get<0>(multiset[i])] &&
                       !partition[std::get<1>(multiset[i])]) {  // bc in text and b in left set and c in right
                lr_count++;
            }
        } else {
            if (!partition[std::get<0>(multiset[i])] &&
                partition[std::get<1>(multiset[i])]) {  // cb in text and c in left set and b in right
                lr_count++;
            } else if (partition[std::get<0>(multiset[i])] &&
                       !partition[std::get<1>(multiset[i])]) {  // cb in text and c in right set and b in left
                rl_count++;
            }
        }
    }
#ifdef BENCH
    const auto endTimeCount = std::chrono::system_clock::now();
    const auto timeSpanCount = endTimeCount - startTimeCount;
    std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
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
//    DLOG(INFO) << "Time for computing partition: "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
}


/**
 * @brief Computes a partitioning of the symbol in the text.
 *
 * @param multiset[in] The adjacency list of the text
 * @param alphabet[in] The effective alphabet
 * @param partition[out] The partition
 */
template<typename variable_t, typename multiset_t, typename alphabet_t, typename partition_t>
inline void compute_partition_full_parallel(const multiset_t& multiset,
//                                            const alphabet_t& alphabet,
                                                  partition_t& partition,
                                                  const size_t cores = std::thread::hardware_concurrency()) {
#ifdef BENCH
    const auto startTime = std::chrono::system_clock::now();
#endif
    alphabet_t alphabet(partition.size());
//#pragma omp parallel num_threads(cores)
//    {
        auto partition_iter = partition.begin();
//#pragma omp for schedule(static)
        for (size_t i = 0; i < partition.size(); ++i) {

            alphabet[i] = (*partition_iter).first;
            ++partition_iter;
        }
//    }
    ips4o::parallel::sort(alphabet.begin(), alphabet.end());
//    partitioned_radix_sort(alphabet);
//    __gnu_parallel::sort(alphabet.begin(), alphabet.end(), __gnu_parallel::multiway_mergesort_tag());
//    DLOG(INFO) << util::text_vector_to_string(alphabet);

//    std::unordered_map<variable_t, std::shared_timed_mutex> mutexes;
//    std::cout << "Alphabet: " << util::text_vector_to_string(alphabet) << std::endl;

    std::vector<std::shared_timed_mutex> mutexes(alphabet.size());
    std::unordered_map<variable_t, size_t> mapping;
//    std::unordered_map<variable_t, bool> locks;
#pragma omp parallel for schedule(static) num_threads(cores)
    for (size_t i = 0; i < alphabet.size(); ++i) {
#pragma omp critical
        {
            mapping[alphabet[i]] = i;
//            locks[alphabet[i]] = false;
        }
    }

//    for (const auto& m: mapping) {
//        std::cout << m.first << ", " << m.second << std::endl;
//    }

//        std::cout << "Locks: " << std::endl;
//        for (const auto& lock : locks) {
//            std::cout << lock.first << ", " << lock.second << std::endl;
//        }

//        std::cout << std::endl;
    // TODO(Chris): parallelize histgram
    std::vector<size_t> hist(alphabet.size() + 1, 0);
    for (size_t i = 0; i < multiset.size(); ++i) {
//            std::cout << std::get<0>(multiset[i]) << " ";
        hist[mapping[std::get<0>(multiset[i])]]++;
    }
//    std::cout << std::endl;

//    for (size_t i = 0; i < multiset.size(); ++i) {
//        std::cout << std::get<1>(multiset[i]) << " ";
//    }
//    std::cout << std::endl;

    std::vector<size_t> starts;
    starts.push_back(0);
    size_t j = 0;
    for (size_t i = 0; i < alphabet.size(); ++i) {
        if (hist[mapping[alphabet[i]]] > 0) {
            starts.push_back(starts[j] + hist[mapping[alphabet[i]]]);
        }
    }

//        std::cout << "starts: " << starts[0] << ", ";
    for (size_t i = 1; i < starts.size(); ++i) {
////            std::cout << starts[i] << ", ";
        starts[i] += starts[i - 1];
    }
//        std::cout << std::endl;
//
//        std::cout << "starts: ";
//        for (size_t i = 0; i < starts.size(); ++i) {
//            std::cout << starts[i] << ", ";
//        }
//        std::cout << std::endl;
//
//
//
////        std::unordered_map<variable_t, omp_lock_t> locks{};
////        for (size_t i = 0; i < alphabet.size(); ++i) {
////            omp_lock_t lock;
////            locks[alphabet[i]] = lock;
////            omp_init_lock(&locks[alphabet[i]]);
////        }

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
//                std::cout << "bounds: 0, ";
            for (size_t i = 1; i < n_threads; ++i) {
                bounds[i] = bounds[i-1] + step;
//                    std::cout << bounds[i] << ", ";
            }
//                bounds[n_threads] = starts.size() - 1;
//                std::cout << bounds[omp_get_num_threads()] << ", " << std::endl;
        }

        for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
//#pragma omp critical
//            {std::cout << "Locking write " << std::get<0>(multiset[starts[i]]) << " by thread " << thread_id << std::endl;}
            mutexes[mapping[std::get<0>(multiset[starts[i]])]].lock();
//#pragma omp critical
//            {std::cout << "Locked write " << std::get<0>(multiset[starts[i]]) << " by thread " << thread_id << std::endl;}



// #pragma omp critical
//                {
//                    std::cout << "Locking: " << std::get<0>(multiset[starts[i]]) << " by thread " << thread_id
//                              << " i: " << i << std::endl;
//                }
//            auto found = mutexes.find(std::get<0>(multiset[starts[i]]));
//            (*found).second.lock();
//            std::unique_lock<std::shared_timed_mutex> lock((*found).second, std::defer_lock);
//            std::lock(lock);
//            (*found).second = !(*found).second;
//                locks[std::get<0>(multiset[starts[i]])] = true;

//                omp_set_lock(&locks[std::get<0>(multiset[starts[i]])]);
        }
#pragma omp barrier
// #pragma omp single
//            {
//                std::cout << "All locks aquired" << std::endl;
//                for (const auto& lock : locks) {
//                    std::cout << lock.first << ", " << lock.second << std::endl;
//                }
//            }

        int l_count = 0;
        int r_count = 0;
        size_t index = 0;
        for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
            index = starts[i];

            while (index < starts[i + 1]) {
//#pragma omp critical
//                {std::cout << "Index " << index << " to " << starts[i+1] << " by thread " << thread_id << " char " << mapping[std::get<0>(multiset[index])] << std::endl;}

//// #pragma omp critical
////                    {
////                        std::cout << "Test for: " << std::get<1>(multiset[index]) << " by thread " << thread_id
////                                  << std::endl;
////                    }
//                bool locked = false;
//#pragma omp critical
//                {
//                    locked = locks[std::get<1>(multiset[index])];
//                }
//                while (locked) {
//#pragma omp critical
//                    {
//                        locked = locks[std::get<1>(multiset[index])];
//                    }
//                }
//                    while (!omp_test_lock(&locks[std::get<1>(multiset[index])])) {
//                        std::cout << "Waiting thread: " << thread_id << std::endl;
//                    }
//                bool check = false;
//#pragma omp critical
//                {
//                    check = partition[std::get<1>(multiset[index])];
//                }
//                if (check) {
//                    r_count++;
//                } else {
//                    l_count++;
//                }
//                index++;


//#pragma omp critical
//                {std::cout << "Locking shared " << std::get<1>(multiset[index]) << " by thread " << thread_id << std::endl;}
                mutexes[mapping[std::get<1>(multiset[index])]].lock_shared();
//#pragma omp critical
//                {std::cout << "Locked shared " << std::get<1>(multiset[index]) << " by thread " << thread_id << std::endl;}
                if (partition[std::get<1>(multiset[index])]) {
                    r_count++;
                } else {
                    l_count++;
                }
//#pragma omp critical
//                {std::cout << "Unlocking shared " << std::get<1>(multiset[index]) << " by thread " << thread_id << std::endl;}
                mutexes[mapping[std::get<1>(multiset[index])]].unlock_shared();
//#pragma omp critical
//                {std::cout << "Unlocked shared " << std::get<1>(multiset[index]) << " by thread " << thread_id << std::endl;}
                index++;
            }
//#pragma omp critical
//            {std::cout << "finished " << std::get<0>(multiset[index - 1]) << " by thread " << thread_id << " at index " << (index-1) << std::endl;}
            partition[std::get<0>(multiset[index - 1])] = l_count > r_count;
//#pragma omp critical
//            {std::cout << "Unlocking write " << std::get<0>(multiset[index - 1]) << " by thread " << thread_id << std::endl;}
            mutexes[mapping[std::get<0>(multiset[index - 1])]].unlock();
//#pragma omp critical
//            {std::cout << "Unlocked write " << std::get<0>(multiset[index - 1]) << " by thread " << thread_id << std::endl;}

//            (*write_found).second.unlock();
//#pragma omp critical
//            {
//                partition[std::get<0>(multiset[index - 1])] = l_count > r_count;
//            }
            l_count = 0;
            r_count = 0;
// #pragma omp critical
//                {
//                    std::cout << "Releasing: " << std::get<0>(multiset[index - 1]) << " by thread " << thread_id
//                              << std::endl;
//                }
//#pragma omp critical
//            {
//                locks[std::get<0>(multiset[index - 1])] = false;
//            }
//                omp_unset_lock(&locks[std::get<0>(multiset[index - 1])]);
        }

//#pragma omp barrier
    }
//        delete[] bounds;

//        for (size_t i = 0; i < alphabet.size(); ++i) {
//            omp_destroy_lock(&locks[alphabet[i]]);
//        }

//        for (size_t i = 0; i < multiset.size(); ++i) {
//            while (j < alphabet.size() && std::get<0>(multiset[i]) > alphabet[j]) {
//                partition[alphabet[j]] = l_count > r_count;
////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
//                j++;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(multiset[i])]) {
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
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);

#ifdef BENCH
    const auto startTimeCount = std::chrono::system_clock::now();
#endif
    int lr_count = 0;
    int rl_count = 0;
#pragma omp parallel for num_threads(cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
    for (size_t i = 0; i < multiset.size(); ++i) {
        if (std::get<2>(multiset[i])) {
            if (!partition[std::get<0>(multiset[i])] &&
                partition[std::get<1>(multiset[i])]) {  // bc in text and b in right set and c in left
                rl_count++;
            } else if (partition[std::get<0>(multiset[i])] &&
                       !partition[std::get<1>(multiset[i])]) {  // bc in text and b in left set and c in right
                lr_count++;
            }
        } else {
            if (!partition[std::get<0>(multiset[i])] &&
                partition[std::get<1>(multiset[i])]) {  // cb in text and c in left set and b in right
                lr_count++;
            } else if (partition[std::get<0>(multiset[i])] &&
                       !partition[std::get<1>(multiset[i])]) {  // cb in text and c in right set and b in left
                rl_count++;
            }
        }
    }
#ifdef BENCH
    const auto endTimeCount = std::chrono::system_clock::now();
    const auto timeSpanCount = endTimeCount - startTimeCount;
    std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
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
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
#ifdef BENCH
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
//    DLOG(INFO) << "Time for computing partition: "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
}


/**
 * @brief Computes a partitioning of the symbol in the text.
 *
 * @param multiset[in] The adjacency list of the text
 * @param alphabet[in] The effective alphabet
 * @param partition[out] The partition
 */
template<typename variable_t, typename multiset_t, typename alphabet_t, typename partition_t>
inline void compute_partition_full_parallel_naive(const multiset_t& multiset,
//                                            const alphabet_t& alphabet,
                                            partition_t& partition,
                                            const size_t cores = std::thread::hardware_concurrency()) {
#ifdef BENCH
    const auto startTime = std::chrono::system_clock::now();
#endif
    alphabet_t alphabet(partition.size());
    auto partition_iter = partition.begin();
#pragma omp parallel for schedule(static) num_threads(cores)
    for (size_t i = 0; i < partition.size(); ++i) {
        alphabet[i] = (*partition_iter).first;
        ++partition_iter;
    }
    ips4o::parallel::sort(alphabet.begin(), alphabet.end());
//    partitioned_radix_sort(alphabet);
//    __gnu_parallel::sort(alphabet.begin(), alphabet.end(), __gnu_parallel::multiway_mergesort_tag());
//    DLOG(INFO) << util::text_vector_to_string(alphabet);

    std::unordered_map<variable_t, size_t> mapping;
    std::unordered_map<variable_t, bool> locks;
#pragma omp parallel for schedule(static) num_threads(cores)
    for (size_t i = 0; i < alphabet.size(); ++i) {
#pragma omp critical
        {
            mapping[alphabet[i]] = i;
            locks[alphabet[i]] = false;
        }
    }

//        std::cout << "Locks: " << std::endl;
//        for (const auto& lock : locks) {
//            std::cout << lock.first << ", " << lock.second << std::endl;
//        }

//        std::cout << std::endl;
        // TODO(Chris): parallelize histgram
        std::vector<size_t> hist(alphabet.size() + 1, 0);
        for (size_t i = 0; i < multiset.size(); ++i) {
//            std::cout << std::get<0>(multiset[i]) << " ";
            hist[mapping[std::get<0>(multiset[i])]]++;
        }
//        std::cout << std::endl;
//
//        for (size_t i = 0; i < multiset.size(); ++i) {
//            std::cout << std::get<1>(multiset[i]) << " ";
//        }
//        std::cout << std::endl;

        std::vector<size_t> starts;
        starts.push_back(0);
        size_t j = 0;
        for (size_t i = 0; i < hist.size(); ++i) {
            if (hist[i] > 0) {
                starts.push_back(starts[j] + hist[i]);
            }
        }

//        std::cout << "starts: " << starts[0] << ", ";
        for (size_t i = 1; i < starts.size(); ++i) {
//            std::cout << starts[i] << ", ";
            starts[i] += starts[i - 1];
        }
//        std::cout << std::endl;
//
//        std::cout << "starts: ";
//        for (size_t i = 0; i < starts.size(); ++i) {
//            std::cout << starts[i] << ", ";
//        }
//        std::cout << std::endl;
//
//
//
////        std::unordered_map<variable_t, omp_lock_t> locks{};
////        for (size_t i = 0; i < alphabet.size(); ++i) {
////            omp_lock_t lock;
////            locks[alphabet[i]] = lock;
////            omp_init_lock(&locks[alphabet[i]]);
////        }

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
//                std::cout << "bounds: 0, ";
                for (size_t i = 1; i < n_threads; ++i) {
                    bounds[i] = bounds[i-1] + step;
//                    std::cout << bounds[i] << ", ";
                }
//                bounds[n_threads] = starts.size() - 1;
//                std::cout << bounds[omp_get_num_threads()] << ", " << std::endl;
            }

            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
// #pragma omp critical
//                {
//                    std::cout << "Locking: " << std::get<0>(multiset[starts[i]]) << " by thread " << thread_id
//                              << " i: " << i << std::endl;
//                }
                auto found = locks.find(std::get<0>(multiset[starts[i]]));
                (*found).second = !(*found).second;
//                locks[std::get<0>(multiset[starts[i]])] = true;

//                omp_set_lock(&locks[std::get<0>(multiset[starts[i]])]);
            }
#pragma omp barrier
// #pragma omp single
//            {
//                std::cout << "All locks aquired" << std::endl;
//                for (const auto& lock : locks) {
//                    std::cout << lock.first << ", " << lock.second << std::endl;
//                }
//            }

            int l_count = 0;
            int r_count = 0;
            size_t index = 0;
            for (size_t i = bounds[thread_id]; i < bounds[thread_id + 1]; ++i) {
                index = starts[i];

                while (index < starts[i + 1]) {
// #pragma omp critical
//                    {
//                        std::cout << "Test for: " << std::get<1>(multiset[index]) << " by thread " << thread_id
//                                  << std::endl;
//                    }
                    bool locked = false;
#pragma omp critical
                    {
                        locked = locks[std::get<1>(multiset[index])];
                    }
                    while (locked) {
#pragma omp critical
                        {
                            locked = locks[std::get<1>(multiset[index])];
                        }
                    }
//                    while (!omp_test_lock(&locks[std::get<1>(multiset[index])])) {
//                        std::cout << "Waiting thread: " << thread_id << std::endl;
//                    }
                    bool check = false;
#pragma omp critical
                    {
                        check = partition[std::get<1>(multiset[index])];
                    }
                    if (check) {
                        r_count++;
                    } else {
                        l_count++;
                    }
                    index++;
                }
#pragma omp critical
                {
                    partition[std::get<0>(multiset[index - 1])] = l_count > r_count;
                }
                l_count = 0;
                r_count = 0;
// #pragma omp critical
//                {
//                    std::cout << "Releasing: " << std::get<0>(multiset[index - 1]) << " by thread " << thread_id
//                              << std::endl;
//                }
#pragma omp critical
                {
                    locks[std::get<0>(multiset[index - 1])] = false;
                }
//                omp_unset_lock(&locks[std::get<0>(multiset[index - 1])]);
            }

#pragma omp barrier
        }
//        delete[] bounds;

//        for (size_t i = 0; i < alphabet.size(); ++i) {
//            omp_destroy_lock(&locks[alphabet[i]]);
//        }

//        for (size_t i = 0; i < multiset.size(); ++i) {
//            while (j < alphabet.size() && std::get<0>(multiset[i]) > alphabet[j]) {
//                partition[alphabet[j]] = l_count > r_count;
////                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
//                j++;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(multiset[i])]) {
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
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);

#ifdef BENCH
    const auto startTimeCount = std::chrono::system_clock::now();
#endif
    int lr_count = 0;
    int rl_count = 0;
#pragma omp parallel for num_threads(cores) schedule(static) reduction(+:lr_count) reduction(+:rl_count)
    for (size_t i = 0; i < multiset.size(); ++i) {
        if (std::get<2>(multiset[i])) {
            if (!partition[std::get<0>(multiset[i])] &&
                partition[std::get<1>(multiset[i])]) {  // bc in text and b in right set and c in left
                rl_count++;
            } else if (partition[std::get<0>(multiset[i])] &&
                       !partition[std::get<1>(multiset[i])]) {  // bc in text and b in left set and c in right
                lr_count++;
            }
        } else {
            if (!partition[std::get<0>(multiset[i])] &&
                partition[std::get<1>(multiset[i])]) {  // cb in text and c in left set and b in right
                lr_count++;
            } else if (partition[std::get<0>(multiset[i])] &&
                       !partition[std::get<1>(multiset[i])]) {  // cb in text and c in right set and b in left
                rl_count++;
            }
        }
    }
#ifdef BENCH
    const auto endTimeCount = std::chrono::system_clock::now();
    const auto timeSpanCount = endTimeCount - startTimeCount;
    std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
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
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
#ifdef BENCH
    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
//    DLOG(INFO) << "Time for computing partition: "
//               << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
}

}  // namespace parallel

}  // namespace recomp
