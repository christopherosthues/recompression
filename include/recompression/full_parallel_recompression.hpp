
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "parallel_rnd_recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class full_parallel_recompression : public parallel_rnd_recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename parallel_rnd_recompression<variable_t, terminal_count_t>::adj_list_t adj_list_t;
    typedef typename parallel_rnd_recompression<variable_t, terminal_count_t>::partition_t partition_t;

    inline full_parallel_recompression() {
        this->name = "full_parallel";
    }

    inline full_parallel_recompression(std::string& dataset) : parallel_rnd_recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "full_parallel";
    }

    using parallel_rnd_recompression<variable_t, terminal_count_t>::recomp;


 protected:
    /**
     * @brief Computes and sorts the adjacency list of the text in ascending order.
     *
     * The adjacency list is stored as a list of text positions.
     *
     * @param text The text
     * @param adj_list[out] The adjacency list (represented as text positions)
     */
    inline void compute_adj_list_reverse(const text_t& text, adj_list_t& adj_list) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif

#pragma omp parallel for schedule(static) num_threads(this->cores)
        for (size_t i = 0; i < adj_list.size(); ++i) {
            adj_list[i] = i;
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
        const auto startTimeMult = recomp::timer::now();
#endif
//        partitioned_radix_sort(adj_list);
        auto sort_adj = [&](size_t i, size_t j) {
            auto char_i = text[i];
            auto char_i1 = text[i + 1];
            auto char_j = text[j];
            auto char_j1 = text[j + 1];
            if (char_i < char_i1) {
                if (char_j < char_j1) {
                    bool lt = char_i < char_j;
                    if (char_i == char_j) {
                        lt = char_i1 < char_j1;
                    }
                    return lt;
                } else {
                    // char_j1 < char_j -> (char_i, char_i1, 0) (char_j1, char_j, 1)
                    bool greater = char_i < char_j1;
                    if (char_i == char_j1) {
                        if (char_i1 == char_j) {
                            return true;
                        }
                        greater = char_i1 < char_j;
                    }
                    return greater;
                }
            } else {
                if (char_j < char_j1) {
                    // char_i1 > char_i -> (char_i1, char_i, 1) (char_j, char_j1, 0)
                    bool greater = char_i1 < char_j;
                    if (char_i1 == char_j) {
                        if (char_i == char_j1) {
                            return false;
                        }
                        greater = char_i < char_j1;
                    }
                    return greater;
                } else {
                    bool greater = char_i1 < char_j1;
                    if (char_i1 == char_j1) {
                        greater = char_i < char_j;
                    }
                    return greater;
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
     * @brief Computes a partitioning (Sigma_l, Sigma_r) of the symbols in the text.
     *
     * @param text[in] The text
     * @param adj_list[in] The adjacency list of the text (text positions)
     * @param partition[out] The partition
     * @param part_l[out] Indicates which partition set is the first one (@code{false} if symbol with value false
     *                    are in Sigma_l, otherwise all symbols with value true are in Sigma_l)
     */
    inline virtual void compute_partition(const text_t& text, partition_t& partition, bool& part_l) override {
        adj_list_t adj_list(text.size() - 1);
        this->compute_adj_list(text, adj_list);
        adj_list_t reverse_adj_list(text.size() - 1);
        compute_adj_list_reverse(text, reverse_adj_list);
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> distribution(0, 1);
        partition[0] = 0;  // ensure, that minimum one symbol is in the left partition and one in the right
        partition[partition.size() - 1] = 1;
#pragma omp parallel num_threads(this->cores)
        {
#pragma omp for schedule(static)
            for (size_t i = 1; i < partition.size() - 1; ++i) {
                partition[i] = distribution(gen);
            }
        }
#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif

        std::cout << "Partition: " << std::endl;
        for (size_t i = 0; i < partition.size(); ++i) {
            std::cout << "i: " << (int)partition[i] << "\n";
        }

        std::cout << "AdjList: " << std::endl;
        for (size_t i = 0; i < adj_list.size(); ++i) {
            std::cout << "(" << text[adj_list[i]] << ", " << text[adj_list[i] + 1] << "), ";
        }
        std::cout << std::endl;


#ifdef BENCH
        const auto startTimeLocalSearch = recomp::timer::now();
#endif
        std::vector<size_t> adj_bounds(partition.size() + 1, adj_list.size());
        std::vector<size_t> reverse_adj_bounds(partition.size() + 1, reverse_adj_list.size());
        std::vector<size_t> bounds;
        std::vector<std::uint8_t> flip(partition.size());
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = (size_t)omp_get_num_threads();

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, adj_list.size() - 1);
            }
            for (size_t i = 0; i < adj_list.size() - 1; ++i) {
                bounds[thread_id] = i;
                i = adj_list.size();
            }

            size_t i = bounds[thread_id];
            variable_t val = 0;
            variable_t rev_val = 0;
            if (i == 0) {
                if (text[adj_list[0]] > text[adj_list[0] + 1]) {
                    val = text[adj_list[0]];
                } else {
                    val = text[adj_list[0] + 1];
                }
                if (text[reverse_adj_list[0]] < text[reverse_adj_list[0] + 1]) {
                    rev_val = text[reverse_adj_list[0]];
                } else {
                    rev_val = text[reverse_adj_list[0] + 1];
                }
                adj_bounds[val] = 0;
                reverse_adj_bounds[rev_val] = 0;
            } else if (i < bounds[thread_id + 1]) {
                if (text[adj_list[i - 1]] > text[adj_list[i - 1] + 1]) {
                    val = text[adj_list[i - 1]];
                } else {
                    val = text[adj_list[i - 1] + 1];
                }
                if (text[reverse_adj_list[i - 1]] < text[reverse_adj_list[i - 1] + 1]) {
                    rev_val = text[reverse_adj_list[i - 1]];
                } else {
                    rev_val = text[reverse_adj_list[i - 1] + 1];
                }
            }

            for (; i < bounds[thread_id + 1]; ++i) {
                auto char_i = text[adj_list[i]];
                auto char_i1 = text[adj_list[i] + 1];
                if (char_i < char_i1) {
                    char_i = char_i1;
                }
                if (char_i > val) {
                    adj_bounds[char_i] = i;
                    val = char_i;
                }
                char_i = text[reverse_adj_list[i]];
                char_i1 = text[reverse_adj_list[i] + 1];
                if (char_i < char_i1) {
                    char_i = char_i1;
                }
                if (char_i > rev_val) {
                    reverse_adj_bounds[char_i] = i;
                    rev_val = char_i;
                }
            }

#pragma omp barrier
#pragma omp single
            {
                std::cout << "Bounds" << std::endl;
#pragma omp task
                {
                    for (size_t j = adj_bounds.size() - 1; j > 0; --j) {
                        if (adj_bounds[j - 1] == adj_list.size()) {  // TODO(Chris): check correctness
                            adj_bounds[j - 1] = adj_bounds[j];
                        }
                    }
                }
#pragma omp task
                {
                    for (size_t j = reverse_adj_bounds.size() - 1; j > 0; --j) {
                        if (reverse_adj_bounds[j - 1] == reverse_adj_bounds.size()) {  // TODO(Chris): check correctness
                            reverse_adj_bounds[j - 1] = reverse_adj_bounds[j];
                        }
                    }
                }

                std::cout << "Bounds finished" << std::endl;
            }

            int w_l = 0;
            int w_r = 0;
#pragma omp barrier
#pragma omp for schedule(static)
            for (size_t j = 0; j < partition.size(); ++j) {
                for (size_t k = adj_bounds[j]; k < adj_bounds[j + 1]; ++k) {  // TODO(Chris): check correctness
                    auto char_i = text[adj_list[k]];
                    auto char_i1 = text[adj_list[k] + 1];
                    if (char_i < char_i1) {
                        char_i = char_i1;
                    }
                    if (partition[char_i]) {
                        w_r++;
                    } else {
                        w_l++;
                    }
                }
                for (size_t k = reverse_adj_bounds[j]; k < reverse_adj_bounds[j + 1]; ++k) {  // TODO(Chris): check correctness
                    auto char_i = text[reverse_adj_list[k]];
                    auto char_i1 = text[reverse_adj_list[k] + 1];
                    if (char_i < char_i1) {
                        char_i = char_i1;
                    }
                    if (partition[char_i]) {
                        w_r++;
                    } else {
                        w_l++;
                    }
                }
                if (partition[j]) {
                    flip[j] = (std::uint8_t)((w_r > w_l)? 1 : 0);
                } else {
                    flip[j] = (std::uint8_t)((w_l > w_r)? 1 : 0);
                }
            }

#pragma omp single
            {
                std::cout << "Flipping" << std::endl;
                std::cout << "Flip: " << std::endl;
                for (size_t i = 0; i < flip.size(); ++i) {
                    std::cout << "i: " << (int)flip[i] << "\n";
                }
            }
#pragma omp for schedule(static)
            for (size_t j = 0; j < partition.size(); ++j) {
                if (flip[j]) {
                    if (partition[j]) {
                        partition[j] = 0;
                    } else {
                        partition[j] = 1;
                    }
                }
            }
        }
        std::cout << "Partition: " << std::endl;
        for (size_t i = 0; i < partition.size(); ++i) {
            std::cout << "i: " << (int)partition[i] << "\n";
        }

        std::cout << std::endl;
        adj_bounds.resize(0);
        adj_bounds.shrink_to_fit();
        reverse_adj_list.resize(0);
        flip.resize(0);
        flip.shrink_to_fit();
#ifdef BENCH
        const auto endTimeLocalSearch = recomp::timer::now();
        const auto timeSpanLocalSearch = endTimeLocalSearch - startTimeLocalSearch;
        std::cout << " local_search=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanLocalSearch).count();
        std::cout << std::endl;
        const auto startTimeCount = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();

#pragma omp single
            {
                std::cout << "Bounds" << std::endl;
                std::fill(bounds.begin(), bounds.end(), adj_list.size());
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < adj_list.size(); ++i) {
                bounds[thread_id] = i;
                i = adj_list.size();
            }

            variable_t last_i = 0;  // avoid more random access than necessary
            variable_t last_i1 = 0;
            size_t i = bounds[thread_id];
            if (i == 0) {
                last_i = text[adj_list[i]];
                last_i1 = text[adj_list[i] + 1];
                if (!partition[last_i] && partition[last_i1]) {
                    lr_count++;
                    prod_l++;
                } else if (partition[last_i] && !partition[last_i1]) {
                    rl_count++;
                    prod_r++;
                }
                i++;
            } else if (i < adj_list.size()) {
                last_i = text[adj_list[i - 1]];
                last_i1 = text[adj_list[i - 1] + 1];
            }

            for (; i < bounds[thread_id + 1]; ++i) {
                variable_t char_i = text[adj_list[i]];
                variable_t char_i1 = text[adj_list[i] + 1];
                if (!partition[char_i] && partition[char_i1]) {
                    lr_count++;
                    if (char_i != last_i || char_i1 != last_i1) {
                        prod_l++;
                    }
                } else if (partition[char_i] && !partition[char_i1]) {
                    rl_count++;
                    if (char_i != last_i || char_i1 != last_i1) {
                        prod_r++;
                    }
                }
                last_i = char_i;
                last_i1 = char_i1;
            }
        }
        part_l = rl_count > lr_count;
        if (rl_count == lr_count) {
            part_l = prod_r < prod_l;
        }
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
    }
};

}  // namespace parallel
}  // namespace recomp
