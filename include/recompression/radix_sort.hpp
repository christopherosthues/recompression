
#pragma once

#include <omp.h>

#include <algorithm>
#include <array>
#include <climits>
#include <deque>
#include <queue>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "util.hpp"
#include "defs.hpp"


namespace recomp {

/**
 * @brief TODO
 *
 * @tparam value_t The type of integer value
 * @tparam D The number of digits
 * @param value The value
 * @param digits The block of digits to extract (0 gives the least significant block of D bits)
 * @return TODO
 */
template<typename value_t, std::uint8_t D = 8>
value_t digits(value_t value, std::uint8_t digits) {
    value_t mask = (1 << D) - 1;
    for (size_t i = 0; i < digits; ++i) {
        value >>= D;
    }
    return value & mask;
}

template<typename value_t, std::uint8_t D = 8>
value_t digits(value_t value, std::uint8_t digits, value_t mask) {
    for (size_t i = 0; i < digits; ++i) {
        value >>= D;
    }
    return value & mask;
}

template<typename variable_t = recomp::var_t, std::uint8_t D = 8>
void lsd_radix_sort(std::vector<std::pair<variable_t, variable_t>>& vec) {
    const auto lsd_blocks = sizeof(variable_t) * CHAR_BIT / D;
    const auto n_buckets = (1 << D);
    const variable_t mask = (1 << D) - 1;

    std::array<std::queue<std::pair<variable_t, variable_t>>, n_buckets> queues;
    std::array<size_t, n_buckets> sizes;

    // Fill buckets with first exchange radix sort setp
    for (size_t b = 0; b < vec.size(); ++b) {
        queues[digits<variable_t, D>(vec[b].second, 0, mask)].push(vec[b]);
    }

    // Store sizes of each bucket
    for (size_t b = 0; b < queues.size(); ++b) {
        sizes[b] = queues[b].size();
    }

    // Exchange radix sort for the remaining blocks
    for (std::uint8_t j = 1; j < lsd_blocks; ++j) {
        for (size_t b = 0; b < queues.size(); ++b) {
            for (size_t k = 0; k < sizes[b]; ++k) {
                auto elem = queues[b].front();
                queues[digits<variable_t, D>(elem.second, j, mask)].push(elem);
                queues[b].pop();
            }
        }

        for (size_t b = 0; b < queues.size(); ++b) {
            sizes[b] = queues[b].size();
        }
    }

    for (std::uint8_t j = 0; j < lsd_blocks; ++j) {
        for (size_t b = 0; b < queues.size(); ++b) {
            for (size_t k = 0; k < sizes[b]; ++k) {
                auto elem = queues[b].front();
                queues[b].pop();
                queues[digits<variable_t, D>(elem.first, j, mask)].push(elem);
            }
        }

        for (size_t b = 0; b < queues.size(); ++b) {
            sizes[b] = queues[b].size();
        }
    }

    // Copy back sorted tuples to their correct position in the vector
    size_t pos = 0;
    for (size_t j = 0; j < queues.size(); ++j) {
        for (size_t b = 0; b < sizes[j]; ++b) {
            vec[pos++] = queues[j].front();
            queues[j].pop();
        }
    }
}

namespace parallel {

template<typename text_t, typename variable_t, typename T = size_t, std::uint8_t D = 8>
void partitioned_parallel_radix_sort_pairs(text_t& text, ui_vector<T>& vector, const size_t cores) {
    const auto lsd_blocks = sizeof(variable_t) * CHAR_BIT / D;  // assumed that the number of bits is integer-divisible by D
    int n_blocks = lsd_blocks;
    const auto n_buckets = (1 << D);
    bool insuff_key = true;

    variable_t mask = n_buckets - 1;
    std::array<std::deque<T>, n_buckets> buckets;
    std::deque<size_t> bucks(n_buckets);
    const auto hist_size = n_buckets + 1;
    std::array<size_t, hist_size> hist;
    hist[0] = 0;
    std::array<size_t*, n_buckets> bounds;
#pragma omp parallel num_threads(cores)
    {
        auto thread_id = omp_get_thread_num();
        auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
        {
//            DLOG(INFO) << "Init bounds";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i] = new size_t[n_threads + 1];
                bounds[i][0] = 0;
            }
        }

#pragma omp barrier
        while (insuff_key && n_blocks > 0) {
            std::array<std::deque<T>, n_buckets> t_buckets;
//            DLOG(INFO) << "MSD radix sort step for key block " << n_blocks;
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < vector.size(); ++i) {
                t_buckets[digits<variable_t, D>(text[vector[i]], n_blocks - 1, mask)].emplace_back(vector[i]);
            }

#ifdef DEBUG
            //            for (size_t i = 0; i < t_buckets.size(); ++i) {
//                if (!t_buckets[i].empty()) {
//                    DLOG(INFO) << "Bucket " << i << " of thread " << thread_id << ": "
//                               << util::vector_blocks_to_string(t_buckets[i]);
//                }
//            }
#endif

//            DLOG(INFO) << "Write lengths";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i][thread_id + 1] = t_buckets[i].size();
            }

#pragma omp barrier
#pragma omp single
            {
//                DLOG(INFO) << "Computing prefix sums";
                size_t buck = 0;
                for (size_t i = 0; i < bounds.size(); ++i) {
                    for (size_t j = 1; j < n_threads + 1; ++j) {
#ifdef DEBUG
                        //                        std::cout << bounds[i][j] << " ";
#endif
                        bounds[i][j] += bounds[i][j - 1];
                    }
#ifdef DEBUG
                    //                    std::cout << std::endl;
#endif
                    buckets[i].resize(buckets[i].size() + bounds[i][n_threads]);
                    hist[i + 1] = hist[i] + bounds[i][n_threads];
                    if (hist[i] != hist[i+1]) {
//                        DLOG(INFO) << "Adding bucket " << i << ", hist[i]: " << (hist[i] + bounds[i][n_threads]);
                        bucks[buck++] = i;
                    }
                }
                bucks.resize(buck);
            }

//            DLOG(INFO) << "Computing global buckets";
            for (size_t i = 0; i < buckets.size(); ++i) {
                std::copy(t_buckets[i].begin(), t_buckets[i].end(), buckets[i].begin() + bounds[i][thread_id]);
            }

#pragma omp barrier
#pragma omp single
            {
                insuff_key = false;
                for (size_t i = 0; i < buckets.size(); ++i) {
                    if (buckets[i].size() == vector.size()) {
                        insuff_key = true;
                        if (n_blocks > 1) {
                            buckets[i].resize(0);
                            buckets[i].shrink_to_fit();
                            bucks.resize(n_buckets);
                        }
                    }
                }

                if (insuff_key) {
                    n_blocks--;
                }
//                DLOG(INFO) << "Key not sufficient: " << std::to_string(insuff_key);
            }
        }
    }
//    DLOG(INFO) << "Delete bounds arrays";
    for (size_t i = 0; i < bounds.size(); ++i) {
        delete[] bounds[i];
    }

    // sort buckets in increaseing order via LSD radix sort first by the second coordinate than by the first with exchange radix sort
#pragma omp parallel num_threads(cores)
    {
#pragma omp for schedule(dynamic)
        for (size_t buck = 0; buck < bucks.size(); ++buck) {
            auto i = bucks[buck];
            std::array<std::queue<T>, n_buckets> queues;
            std::array<size_t, n_buckets> sizes;

            // Fill buckets with first exchange radix sort setp
            for (size_t b = 0; b < buckets[i].size(); ++b) {
                queues[digits<variable_t, D>(text[buckets[i][b] + 1], 0, mask)].push(buckets[i][b]);
            }
            buckets[i].resize(0);  // release memory of the buckets used for the MSD radix sort step

            // Store sizes of each bucket
            for (size_t b = 0; b < queues.size(); ++b) {
                sizes[b] = queues[b].size();
            }

            // Exchange radix sort for the remaining blocks
            for (std::uint8_t j = 1; j < lsd_blocks; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[digits<variable_t, D>(text[elem + 1], j, mask)].push(elem);
                        queues[b].pop();
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            for (std::uint8_t j = 0; j < n_blocks - 1; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[b].pop();
                        queues[digits<variable_t, D>(text[elem], j, mask)].push(elem);
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            // Copy back sorted tuples to their correct position in the vector
            size_t pos = hist[i];
            for (size_t j = 0; j < queues.size(); ++j) {
                for (size_t b = 0; b < sizes[j]; ++b) {
                    vector[pos++] = queues[j].front();
                    queues[j].pop();
                }
            }
        }
    }
}


template<typename text_t, typename variable_t, typename T = std::pair<variable_t, size_t>, std::uint8_t D = 8>
void partitioned_parallel_radix_sort_blocks(text_t& text, ui_vector<T>& vector, const size_t cores) {
    const auto lsd_blocks = sizeof(variable_t) * CHAR_BIT / D;  // assumed that the number of bits is integer-divisible by D
    int n_blocks = lsd_blocks;
    const auto n_buckets = (1 << D);
    bool insuff_key = true;

    variable_t mask = n_buckets - 1;
    std::array<std::deque<T>, n_buckets> buckets;
    std::deque<size_t> bucks(n_buckets);
    const auto hist_size = n_buckets + 1;
    std::array<size_t, hist_size> hist;
    hist[0] = 0;
    std::array<size_t*, n_buckets> bounds;
#pragma omp parallel num_threads(cores)
    {
        auto thread_id = omp_get_thread_num();
        auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
        {
//            DLOG(INFO) << "Init bounds";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i] = new size_t[n_threads + 1];
                bounds[i][0] = 0;
            }
        }

#pragma omp barrier
        while (insuff_key && n_blocks > 0) {
            std::array<std::deque<T>, n_buckets> t_buckets;
//            DLOG(INFO) << "MSD radix sort step for key block " << n_blocks;
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < vector.size(); ++i) {
                t_buckets[digits<variable_t, D>(text[vector[i].second], n_blocks - 1, mask)].emplace_back(vector[i]);
            }

#ifdef DEBUG
            //            for (size_t i = 0; i < t_buckets.size(); ++i) {
//                if (!t_buckets[i].empty()) {
//                    DLOG(INFO) << "Bucket " << i << " of thread " << thread_id << ": "
//                               << util::vector_blocks_to_string(t_buckets[i]);
//                }
//            }
#endif

//            DLOG(INFO) << "Write lengths";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i][thread_id + 1] = t_buckets[i].size();
            }

#pragma omp barrier
#pragma omp single
            {
//                DLOG(INFO) << "Computing prefix sums";
                size_t buck = 0;
                for (size_t i = 0; i < bounds.size(); ++i) {
                    for (size_t j = 1; j < n_threads + 1; ++j) {
#ifdef DEBUG
                        //                        std::cout << bounds[i][j] << " ";
#endif
                        bounds[i][j] += bounds[i][j - 1];
                    }
#ifdef DEBUG
                    //                    std::cout << std::endl;
#endif
                    buckets[i].resize(buckets[i].size() + bounds[i][n_threads]);
                    hist[i + 1] = hist[i] + bounds[i][n_threads];
                    if (hist[i] != hist[i+1]) {
//                        DLOG(INFO) << "Adding bucket " << i << ", hist[i]: " << (hist[i] + bounds[i][n_threads]);
                        bucks[buck++] = i;
                    }
                }
                bucks.resize(buck);
            }

//            DLOG(INFO) << "Computing global buckets";
            for (size_t i = 0; i < buckets.size(); ++i) {
                std::copy(t_buckets[i].begin(), t_buckets[i].end(), buckets[i].begin() + bounds[i][thread_id]);
            }

#pragma omp barrier
#pragma omp single
            {
                insuff_key = false;
                for (size_t i = 0; i < buckets.size(); ++i) {
                    if (buckets[i].size() == vector.size()) {
                        insuff_key = true;
                        if (n_blocks > 1) {
                            buckets[i].resize(0);
                            buckets[i].shrink_to_fit();
                            bucks.resize(n_buckets);
                        }
                    }
                }

                if (insuff_key) {
                    n_blocks--;
                }
//                DLOG(INFO) << "Key not sufficient: " << std::to_string(insuff_key);
            }
        }
    }
//    DLOG(INFO) << "Delete bounds arrays";
    for (size_t i = 0; i < bounds.size(); ++i) {
        delete[] bounds[i];
    }

    // sort buckets in increaseing order via LSD radix sort first by the second coordinate than by the first with exchange radix sort
#pragma omp parallel num_threads(cores)
    {
#pragma omp for schedule(dynamic)
        for (size_t buck = 0; buck < bucks.size(); ++buck) {
            auto i = bucks[buck];
            std::array<std::queue<T>, n_buckets> queues;
            std::array<size_t, n_buckets> sizes;

            // Fill buckets with first exchange radix sort setp
            for (size_t b = 0; b < buckets[i].size(); ++b) {
                queues[digits<variable_t, D>(buckets[i][b].first, 0, mask)].push(buckets[i][b]);
            }
            buckets[i].resize(0);  // release memory of the buckets used for the MSD radix sort step

            // Store sizes of each bucket
            for (size_t b = 0; b < queues.size(); ++b) {
                sizes[b] = queues[b].size();
            }

            // Exchange radix sort for the remaining blocks
            for (std::uint8_t j = 1; j < lsd_blocks; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[digits<variable_t, D>(elem.first, j, mask)].push(elem);
                        queues[b].pop();
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            for (std::uint8_t j = 0; j < n_blocks - 1; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[b].pop();
                        queues[digits<variable_t, D>(text[elem.second], j, mask)].push(elem);
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            // Copy back sorted tuples to their correct position in the vector
            size_t pos = hist[i];
            for (size_t j = 0; j < queues.size(); ++j) {
                for (size_t b = 0; b < sizes[j]; ++b) {
                    vector[pos++] = queues[j].front();
                    queues[j].pop();
                }
            }
        }
    }
}




















/**
 * @brief
 *
 * @tparam D The number of digits to use for one radix sort run (default = 4 bits)
 * @param vec The vector to sort
 */
template<typename variable_t = recomp::var_t, std::uint8_t D = 8>
void partitioned_radix_sort(std::vector<variable_t>& vec, const size_t cores = std::thread::hardware_concurrency()) {
    const auto lsd_blocks = sizeof(variable_t) * CHAR_BIT / D;  // assumed that the number of bits is integer-divisible by D
    int n_blocks = lsd_blocks;
    const auto n_buckets = (1 << D);
    bool insuff_key = true;

    variable_t mask = n_buckets - 1;

    std::array<std::deque<variable_t>, n_buckets> buckets;
    std::deque<size_t> bucks(n_buckets);
    const auto hist_size = n_buckets + 1;
    std::array<size_t, hist_size> hist;
    hist[0] = 0;
    std::array<size_t*, n_buckets> bounds;
#pragma omp parallel num_threads(cores)
    {
        auto thread_id = omp_get_thread_num();
        auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
        {
//            DLOG(INFO) << "Init bounds";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i] = new size_t[n_threads + 1];
                bounds[i][0] = 0;
            }
        }

#pragma omp barrier
        while (insuff_key && n_blocks > 0) {
            std::array<std::deque<variable_t>, n_buckets> t_buckets;
//            DLOG(INFO) << "MSD radix sort step for key block " << n_blocks;
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < vec.size(); ++i) {
                t_buckets[digits<variable_t, D>(vec[i], n_blocks - 1, mask)].emplace_back(vec[i]);
            }

#ifdef DEBUG
//            for (size_t i = 0; i < t_buckets.size(); ++i) {
//                if (!t_buckets[i].empty()) {
//                    DLOG(INFO) << "Bucket " << i << " of thread " << thread_id << ": "
//                               << util::vector_blocks_to_string(t_buckets[i]);
//                }
//            }
#endif

//            DLOG(INFO) << "Write lengths";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i][thread_id + 1] = t_buckets[i].size();
            }

#pragma omp barrier
#pragma omp single
            {
//                DLOG(INFO) << "Computing prefix sums";
                size_t buck = 0;
                for (size_t i = 0; i < bounds.size(); ++i) {
                    for (size_t j = 1; j < n_threads + 1; ++j) {
#ifdef DEBUG
//                        std::cout << bounds[i][j] << " ";
#endif
                        bounds[i][j] += bounds[i][j - 1];
                    }
#ifdef DEBUG
//                    std::cout << std::endl;
#endif
                    buckets[i].resize(buckets[i].size() + bounds[i][n_threads]);
                    hist[i + 1] = hist[i] + bounds[i][n_threads];
                    if (hist[i] != hist[i+1]) {
//                        DLOG(INFO) << "Adding bucket " << i << ", hist[i]: " << (hist[i] + bounds[i][n_threads]);
                        bucks[buck++] = i;
                    }
                }
                bucks.resize(buck);
            }

//            DLOG(INFO) << "Computing global buckets";
            for (size_t i = 0; i < buckets.size(); ++i) {
                std::copy(t_buckets[i].begin(), t_buckets[i].end(), buckets[i].begin() + bounds[i][thread_id]);
            }

#pragma omp barrier
#pragma omp single
            {
                insuff_key = false;
                for (size_t i = 0; i < buckets.size(); ++i) {
                    if (buckets[i].size() == vec.size()) {
                        insuff_key = true;
                        if (n_blocks > 1) {
                            buckets[i].resize(0);
                            buckets[i].shrink_to_fit();
                            bucks.resize(n_buckets);
                        }
                    }
                }

                if (insuff_key) {
                    n_blocks--;
                }
//                DLOG(INFO) << "Key not sufficient: " << std::to_string(insuff_key);
            }
        }
    }
//    DLOG(INFO) << "Delete bounds arrays";
    for (size_t i = 0; i < bounds.size(); ++i) {
        delete[] bounds[i];
    }

#ifdef DEBUG
//    for (size_t i = 0; i < buckets.size(); ++i) {
//        if (!buckets[i].empty()) {
//            DLOG(INFO) << "Bucket " << i << " after MSD radix sort step(s): " << util::vector_blocks_to_string(buckets[i]);
//        }
//    }
//
//    DLOG(INFO) << "Prefix sums: " << util::array_to_string(hist);
//    DLOG(INFO) << "Buckets to sort: " << util::pair_positions_to_string(bucks);
//    std::stringstream sstream;
//    for (size_t i = 0; i < buckets.size(); ++i) {
//        sstream << std::to_string(i) << "," << buckets[i].size() << std::endl;
//    }
//    DLOG(INFO) << "Histogram: " << sstream.str();
//    DLOG(INFO) << "Actual block: " << n_blocks;
#endif

    // sort buckets in increaseing order via LSD radix sort first by the second coordinate than by the first with exchange radix sort
#pragma omp parallel num_threads(cores)
    {
#pragma omp for schedule(dynamic)
        for (size_t buck = 0; buck < bucks.size(); ++buck) {
            auto i = bucks[buck];
            std::array<std::queue<variable_t>, n_buckets> queues;
            std::array<size_t, n_buckets> sizes;

            // Fill buckets with first exchange radix sort setp
            for (size_t b = 0; b < buckets[i].size(); ++b) {
                queues[digits<variable_t, D>(buckets[i][b], 0, mask)].push(buckets[i][b]);
            }
            buckets[i].resize(0);  // release memory of the buckets used for the MSD radix sort step
            buckets[i].shrink_to_fit();

            // Store sizes of each bucket
            for (size_t b = 0; b < queues.size(); ++b) {
                sizes[b] = queues[b].size();
            }

            // Exchange radix sort for the remaining blocks
//            for (std::uint8_t j = 1; j < lsd_blocks; ++j) {
//                for (size_t b = 0; b < queues.size(); ++b) {
//                    for (size_t k = 0; k < sizes[b]; ++k) {
//                        auto elem = queues[b].front();
//                        queues[digits<variable_t, D>(elem.second, j, mask)].push(elem);
//                        queues[b].pop();
//                    }
//                }
//
//                for (size_t b = 0; b < queues.size(); ++b) {
//                    sizes[b] = queues[b].size();
//                }
//            }

            for (std::uint8_t j = 1; j < n_blocks - 1; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[b].pop();
                        queues[digits<variable_t, D>(elem, j, mask)].push(elem);
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            // Copy back sorted tuples to their correct position in the vector
            size_t pos = hist[i];
            for (size_t j = 0; j < queues.size(); ++j) {
                for (size_t b = 0; b < sizes[j]; ++b) {
                    vec[pos++] = queues[j].front();
                    queues[j].pop();
                }
            }
        }
    }
}

/**
 * @brief 
 *
 * @tparam D The number of digits to use for one radix sort run (default = 4 bits)
 * @param vec The vector to sort
 */
template<typename variable_t = recomp::var_t, std::uint8_t D = 8>
void partitioned_radix_sort(std::vector<std::pair<variable_t, variable_t>>& vec,
                            const size_t cores = std::thread::hardware_concurrency()) {
    const auto lsd_blocks = sizeof(variable_t) * CHAR_BIT / D;  // assumed that the number of bits is integer-divisible by D
    int n_blocks = lsd_blocks;
    const auto n_buckets = (1 << D);
    bool insuff_key = true;

    variable_t mask = n_buckets - 1;

    std::array<std::deque<std::pair<variable_t, variable_t>>, n_buckets> buckets;
    std::deque<size_t> bucks(n_buckets);
    const auto hist_size = n_buckets + 1;
    std::array<size_t, hist_size> hist;
    hist[0] = 0;
    std::array<size_t*, n_buckets> bounds;
#pragma omp parallel num_threads(cores)
    {
        auto thread_id = omp_get_thread_num();
        auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
        {
//            DLOG(INFO) << "Init bounds";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i] = new size_t[n_threads + 1];
                bounds[i][0] = 0;
            }
        }

#pragma omp barrier
        while (insuff_key && n_blocks > 0) {
            std::array<std::deque<std::pair<variable_t, variable_t>>, n_buckets> t_buckets;
//            DLOG(INFO) << "MSD radix sort step for key block " << n_blocks;
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < vec.size(); ++i) {
                t_buckets[digits<variable_t, D>(vec[i].first, n_blocks - 1, mask)].emplace_back(vec[i]);
            }

#ifdef DEBUG
//            for (size_t i = 0; i < t_buckets.size(); ++i) {
//                if (!t_buckets[i].empty()) {
//                    DLOG(INFO) << "Bucket " << i << " of thread " << thread_id << ": "
//                               << util::vector_blocks_to_string(t_buckets[i]);
//                }
//            }
#endif

//            DLOG(INFO) << "Write lengths";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i][thread_id + 1] = t_buckets[i].size();
            }

#pragma omp barrier
#pragma omp single
            {
//                DLOG(INFO) << "Computing prefix sums";
                size_t buck = 0;
                for (size_t i = 0; i < bounds.size(); ++i) {
                    for (size_t j = 1; j < n_threads + 1; ++j) {
#ifdef DEBUG
//                        std::cout << bounds[i][j] << " ";
#endif
                        bounds[i][j] += bounds[i][j - 1];
                    }
#ifdef DEBUG
//                    std::cout << std::endl;
#endif
                    buckets[i].resize(buckets[i].size() + bounds[i][n_threads]);
                    hist[i + 1] = hist[i] + bounds[i][n_threads];
                    if (hist[i] != hist[i+1]) {
//                        DLOG(INFO) << "Adding bucket " << i << ", hist[i]: " << (hist[i] + bounds[i][n_threads]);
                        bucks[buck++] = i;
                    }
                }
                bucks.resize(buck);
            }

//            DLOG(INFO) << "Computing global buckets";
            for (size_t i = 0; i < buckets.size(); ++i) {
                std::copy(t_buckets[i].begin(), t_buckets[i].end(), buckets[i].begin() + bounds[i][thread_id]);
            }

#pragma omp barrier
#pragma omp single
            {
                insuff_key = false;
                for (size_t i = 0; i < buckets.size(); ++i) {
                    if (buckets[i].size() == vec.size()) {
                        insuff_key = true;
                        if (n_blocks > 1) {
                            buckets[i].resize(0);
                            bucks.resize(n_buckets);
                        }
                    }
                }

                if (insuff_key) {
                    n_blocks--;
                }
//                DLOG(INFO) << "Key not sufficient: " << std::to_string(insuff_key);
            }
        }
    }
//    DLOG(INFO) << "Delete bounds arrays";
    for (size_t i = 0; i < bounds.size(); ++i) {
        delete[] bounds[i];
    }

#ifdef DEBUG
//    for (size_t i = 0; i < buckets.size(); ++i) {
//        if (!buckets[i].empty()) {
//            DLOG(INFO) << "Bucket " << i << " after MSD radix sort step(s): " << util::vector_blocks_to_string(buckets[i]);
//        }
//    }
//
//    DLOG(INFO) << "Prefix sums: " << util::array_to_string(hist);
//    DLOG(INFO) << "Buckets to sort: " << util::pair_positions_to_string(bucks);
//    std::stringstream sstream;
//    for (size_t i = 0; i < buckets.size(); ++i) {
//        sstream << std::to_string(i) << "," << buckets[i].size() << std::endl;
//    }
//    DLOG(INFO) << "Histogram: " << sstream.str();
//    DLOG(INFO) << "Actual block: " << n_blocks;
#endif

    // sort buckets in increaseing order via LSD radix sort first by the second coordinate than by the first with exchange radix sort
#pragma omp parallel num_threads(cores)
    {
#pragma omp for schedule(dynamic)
        for (size_t buck = 0; buck < bucks.size(); ++buck) {
            auto i = bucks[buck];
            std::array<std::queue<std::pair<variable_t, variable_t>>, n_buckets> queues;
            std::array<size_t, n_buckets> sizes;

            // Fill buckets with first exchange radix sort setp
            for (size_t b = 0; b < buckets[i].size(); ++b) {
                queues[digits<variable_t, D>(buckets[i][b].second, 0, mask)].push(buckets[i][b]);
            }
            buckets[i].resize(0);  // release memory of the buckets used for the MSD radix sort step

            // Store sizes of each bucket
            for (size_t b = 0; b < queues.size(); ++b) {
                sizes[b] = queues[b].size();
            }

            // Exchange radix sort for the remaining blocks
            for (std::uint8_t j = 1; j < lsd_blocks; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[digits<variable_t, D>(elem.second, j, mask)].push(elem);
                        queues[b].pop();
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            for (std::uint8_t j = 0; j < n_blocks - 1; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[b].pop();
                        queues[digits<variable_t, D>(elem.first, j, mask)].push(elem);
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            // Copy back sorted tuples to their correct position in the vector
            size_t pos = hist[i];
            for (size_t j = 0; j < queues.size(); ++j) {
                for (size_t b = 0; b < sizes[j]; ++b) {
                    vec[pos++] = queues[j].front();
                    queues[j].pop();
                }
            }
        }
    }
}

template<typename variable_t = recomp::var_t, std::uint8_t D = 8>
void partitioned_radix_sort(std::vector<std::tuple<variable_t, variable_t, bool>>& vec,
                            const size_t cores = std::thread::hardware_concurrency()) {
    const auto lsd_blocks = sizeof(variable_t) * CHAR_BIT / D;  // assumed that the number of bits is integer-divisible by D
    int n_blocks = lsd_blocks;
    const auto n_buckets = (1 << D);
    bool insuff_key = true;

    variable_t mask = n_buckets - 1;

    std::array<std::deque<std::tuple<variable_t, variable_t, bool>>, n_buckets> buckets;
    std::deque<size_t> bucks(n_buckets);
    const auto hist_size = n_buckets + 1;
    std::array<size_t, hist_size> hist;
    hist[0] = 0;
    std::array<size_t*, n_buckets> bounds;
#pragma omp parallel num_threads(cores)
    {
        auto thread_id = omp_get_thread_num();
        auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
        {
//            DLOG(INFO) << "Init bounds";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i] = new size_t[n_threads + 1];
                bounds[i][0] = 0;
            }
        }

#pragma omp barrier
        while (insuff_key && n_blocks > 0) {
            std::array<std::deque<std::tuple<variable_t, variable_t, bool>>, n_buckets> t_buckets;
//            DLOG(INFO) << "MSD radix sort step for key block " << n_blocks;
#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < vec.size(); ++i) {
//            auto bit_block = digits<variable_t, D>(vec[i].first, n_blocks - 1);
                t_buckets[digits<variable_t, D>(std::get<0>(vec[i]), n_blocks - 1, mask)].emplace_back(vec[i]);
            }

#ifdef DEBUG
//            for (size_t i = 0; i < t_buckets.size(); ++i) {
//                if (!t_buckets[i].empty()) {
//                    DLOG(INFO) << "Bucket " << i << " of thread " << thread_id << ": "
//                               << util::multiset_to_string(t_buckets[i]);
//                }
//            }
#endif

//            DLOG(INFO) << "Write lengths";
            for (size_t i = 0; i < bounds.size(); ++i) {
                bounds[i][thread_id + 1] = t_buckets[i].size();
            }

#pragma omp barrier
#pragma omp single
            {
//                DLOG(INFO) << "Computing prefix sums";
                size_t buck = 0;
                for (size_t i = 0; i < bounds.size(); ++i) {
                    for (size_t j = 1; j < n_threads + 1; ++j) {
#ifdef DEBUG
//                        std::cout << bounds[i][j] << " ";
#endif
                        bounds[i][j] += bounds[i][j - 1];
                    }
#ifdef DEBUG
//                    std::cout << std::endl;
#endif
                    buckets[i].resize(buckets[i].size() + bounds[i][n_threads]);
                    hist[i + 1] = hist[i] + bounds[i][n_threads];
                    if (hist[i] != hist[i+1]) {
//                        DLOG(INFO) << "Adding bucket " << i << ", hist[i]: " << (hist[i] + bounds[i][n_threads]);
                        bucks[buck++] = i;
                    }
                }
                bucks.resize(buck);
            }

//            DLOG(INFO) << "Computing global buckets";
            for (size_t i = 0; i < buckets.size(); ++i) {
                std::copy(t_buckets[i].begin(), t_buckets[i].end(), buckets[i].begin() + bounds[i][thread_id]);
            }

#pragma omp barrier
#pragma omp single
            {
                insuff_key = false;
                for (size_t i = 0; i < buckets.size(); ++i) {
                    if (buckets[i].size() == vec.size()) {
                        insuff_key = true;
                        if (n_blocks > 1) {
                            buckets[i].resize(0);
                            bucks.resize(n_buckets);
                        }
                    }
                }

                if (insuff_key) {
                    n_blocks--;
                }
//                DLOG(INFO) << "Key not sufficient: " << std::to_string(insuff_key);
            }
        }
    }
//    DLOG(INFO) << "Delete bounds arrays";
    for (size_t i = 0; i < bounds.size(); ++i) {
        delete[] bounds[i];
    }

#ifdef DEBUG
//    for (size_t i = 0; i < buckets.size(); ++i) {
//        if (!buckets[i].empty()) {
//            DLOG(INFO) << "Bucket " << i << " after MSD radix sort step(s): " << util::multiset_to_string(buckets[i]);
//        }
//    }
//
//    DLOG(INFO) << "Histogram: " << util::array_to_string(hist);
//    DLOG(INFO) << "Buckets to sort: " << util::pair_positions_to_string(bucks);
#endif

    // sort buckets in increaseing order via LSD radix sort first by the second coordinate than by the first with exchange radix sort
#pragma omp parallel num_threads(cores)
    {
#pragma omp for schedule(dynamic)
        for (size_t buck = 0; buck < bucks.size(); ++buck) {
            auto i = bucks[buck];
            std::array<std::queue<std::tuple<variable_t, variable_t, bool>>, n_buckets> queues;
            std::array<size_t, n_buckets> sizes;

            // Fill buckets with first exchange radix sort setp
            for (size_t b = 0; b < buckets[i].size(); ++b) {
                queues[digits<variable_t, D>(std::get<1>(buckets[i][b]), 0, mask)].push(buckets[i][b]);
            }
            buckets[i].resize(0);  // release memory of the buckets used for the MSD radix sort step

            // Store sizes of each bucket
            for (size_t b = 0; b < queues.size(); ++b) {
                sizes[b] = queues[b].size();
            }

            // Exchange radix sort for the remaining blocks
            for (std::uint8_t j = 1; j < lsd_blocks; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[digits<variable_t, D>(std::get<1>(elem), j, mask)].push(elem);
                        queues[b].pop();
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            for (std::uint8_t j = 0; j < n_blocks - 1; ++j) {
                for (size_t b = 0; b < queues.size(); ++b) {
                    for (size_t k = 0; k < sizes[b]; ++k) {
                        auto elem = queues[b].front();
                        queues[b].pop();
                        queues[digits<variable_t, D>(std::get<0>(elem), j, mask)].push(elem);
                    }
                }

                for (size_t b = 0; b < queues.size(); ++b) {
                    sizes[b] = queues[b].size();
                }
            }

            // Copy back sorted tuples to their correct position in the vector
            size_t pos = hist[i];
            for (size_t j = 0; j < queues.size(); ++j) {
                for (size_t b = 0; b < sizes[j]; ++b) {
                    vec[pos++] = queues[j].front();
                    queues[j].pop();
                }
            }
        }
    }
}

template<typename adj_list_t>
void bucket_sort(adj_list_t& adj_list, const size_t cores = std::thread::hardware_concurrency()) {
    std::vector<size_t> bounds;
#pragma omp parallel num_threads(cores)
    {

        auto thread_id = omp_get_thread_num();
        auto n_threads = static_cast<size_t>(omp_get_num_threads());
        auto bounds_len = (n_threads << 1);

#pragma omp single
        {
            bounds.reserve(bounds_len + 1);
            bounds.resize(bounds_len + 1);
            bounds[0] = 0;
        }

        std::array<adj_list_t, 2> buckets;
#pragma omp for schedule(static) nowait
        for (size_t i = 0; i < adj_list.size(); ++i) {
            buckets[std::get<0>(adj_list[i])].push_back(adj_list[i]);
        }

        bounds[thread_id + 1] = buckets[0].size();
        bounds[thread_id + n_threads + 1] = buckets[1].size();

#pragma omp barrier
#pragma omp single
        {
            for (size_t i = 1; i < bounds_len + 1; ++i) {
                bounds[i] += bounds[i - 1];
            }
//            positions.resize(positions.size() + bounds[n_threads]);
        }
        std::copy(buckets[0].begin(), buckets[0].end(), adj_list.begin() + bounds[thread_id]);
        std::copy(buckets[1].begin(), buckets[1].end(), adj_list.begin() + bounds[thread_id + n_threads]);
    }
}

}  // namespace parallel

}  // namespace recomp
