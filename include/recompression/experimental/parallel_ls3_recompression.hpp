
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

#include "recompression/parallel_ls_recompression.hpp"
#include "recompression/defs.hpp"
#include "recompression/util.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {

namespace parallel {

/**
 * @brief This class is a parallel implementation of the recompression computing the undirected maximum cut using local
 * search based on a random partition. The local search is repeated 3 times. Counts also the number of new generated
 * production rules like parallel_lp_recompression.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class parallel_ls3_recompression : public parallel_ls_recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename parallel_ls_recompression<variable_t>::adj_t adj_t;
    typedef typename parallel_ls_recompression<variable_t>::adj_list_t adj_list_t;
    typedef typename parallel_ls_recompression<variable_t>::partition_t partition_t;

    inline parallel_ls3_recompression() {
        this->name = "parallel_ls3";
    }

    inline parallel_ls3_recompression(std::string& dataset) : parallel_ls_recompression<variable_t>(dataset) {
        this->name = "parallel_ls3";
    }

    using parallel_rnd_recompression<variable_t>::recomp;


 protected:
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
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        const size_t adj_list_size = text.size() - 1;
        adj_list_t adj_list(adj_list_size);
#ifdef BENCH
        const auto endTimeAdjInit = recomp::timer::now();
        const auto timeSpanAdjInit = endTimeAdjInit - startTime;
        std::cout << " init_adj_vec=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAdjInit).count();
#endif
        this->compute_adj_list(text, adj_list);

#ifdef BENCH
        const auto startTimePar = recomp::timer::now();
#endif
        partition[0] = false;  // ensure, that minimum one symbol is in the left partition and one in the right
        partition[partition.size() - 1] = true;
#pragma omp parallel num_threads(this->cores)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint8_t> distribution(0, 1);
#pragma omp for schedule(static)
            for (size_t i = 1; i < partition.size() - 1; ++i) {
                partition[i] = (distribution(gen) == 1);
            }
        }
#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTimePar;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
        const auto startTimeLocalSearch = recomp::timer::now();
#endif

        i_vector<ui_vector<std::int64_t>> flip;
        std::vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores)
        {
            auto n_threads = (size_t) omp_get_num_threads();
#pragma omp single
            {
                flip.resize(n_threads);
                for (size_t i = 0; i < n_threads; ++i) {
                    flip[i].resize(partition.size());
                }
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, adj_list.size() - 1);
            }
        }

        for (size_t k = 0; k < 3; ++k) {
#pragma omp parallel num_threads(this->cores)
            {
                auto n_threads = (size_t) omp_get_num_threads();
                auto thread_id = (size_t) omp_get_thread_num();

                flip[thread_id].fill(0);

#pragma omp for schedule(static)
                for (size_t i = 0; i < adj_list_size; ++i) {
                    auto char_i = text[adj_list[i]];
                    auto char_i1 = text[adj_list[i] + 1];
                    if (partition[char_i] != partition[char_i1]) {
                        flip[thread_id][char_i]++;
                        flip[thread_id][char_i1]++;
                    } else {
                        flip[thread_id][char_i]--;
                        flip[thread_id][char_i1]--;
                    }
                }

#pragma omp for schedule(static)
                for (size_t i = 0; i < partition.size(); ++i) {
                    for (size_t j = 1; j < n_threads; ++j) {
                        flip[0][i] += flip[j][i];
                    }
                    if (flip[0][i] < 0) {
                        if (partition[i] == 0) {
                            partition[i] = 1;
                        } else {
                            partition[i] = 0;
                        }
                    }
                }
            }
        }
        for (size_t i = 0; i < flip.size(); ++i) {
            flip[i].resize(1);
        }
        flip.resize(1);
#ifdef BENCH
        const auto endTimeLocalSearch = recomp::timer::now();
        const auto timeSpanLocalSearch = endTimeLocalSearch - startTimeLocalSearch;
        std::cout << " local_search=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanLocalSearch).count();
        const auto startTimeDiff = recomp::timer::now();
#endif

        bool different = false;
#pragma omp parallel for schedule(static) num_threads(this->cores) reduction(|:different)
        for (size_t i = 0; i < partition.size() - 1; ++i) {
            if (partition[i] != partition[i + 1]) {
                different = true;
            }
        }
        if (!different) {
            partition[0] = 0;  // ensure, that minimum one symbol is in the left partition and one in the right
            partition[partition.size() - 1] = 1;
        }
#ifdef BENCH
        const auto endTimeDiff = recomp::timer::now();
        const auto timeSpanDiff = endTimeDiff - startTimeDiff;
        std::cout << " diff_check=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanDiff).count()
                  << " different=" << std::to_string(different);
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
