
#pragma once

#include <omp.h>

#ifdef BENCH
#include <iostream>
#endif

#include <algorithm>
#include <deque>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression/parallel_rnd_recompression.hpp"
#include "recompression/defs.hpp"
#include "recompression/util.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t>
class parallel_wrnd_recompression : public parallel_rnd_recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename parallel_wrnd_recompression<variable_t>::adj_t adj_t;
    typedef typename parallel_wrnd_recompression<variable_t>::adj_list_t adj_list_t;
    typedef typename parallel_wrnd_recompression<variable_t>::partition_t partition_t;

    inline parallel_wrnd_recompression() {
        this->name = "parallel_wrnd";
    }

    inline parallel_wrnd_recompression(std::string& dataset) : parallel_rnd_recompression<variable_t>(dataset) {
        this->name = "parallel_wrnd";
    }

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
    inline virtual void compute_partition(const text_t& text, partition_t& partition, bool& part_l, variable_t minimum) override {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
#endif
        adj_list_t adj_list(text.size() - 1);
#ifdef BENCH
        const auto endTimeAdjInit = recomp::timer::now();
        const auto timeSpanAdjInit = endTimeAdjInit - startTime;
        std::cout << " init_adj_vec=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAdjInit).count();
#endif
        this->compute_adj_list(text, adj_list);

#ifdef BENCH
        const auto startTimeHist = recomp::timer::now();
//        std::cout << std::endl;
#endif
//        variable_t minimum = std::numeric_limits<variable_t>::max();
//        variable_t maximum = 0;
//#pragma omp parallel for schedule(static) num_threads(this->cores) reduction(min:minimum)
//        for (size_t i = 0; i < text.size(); ++i) {
//            minimum = std::min(minimum, text[i]);
//            maximum = std::max(maximum, text[i]);
//        }
//        std::cout << minimum << ", " << maximum << std::endl;

        const auto max_letters = partition.size();//  maximum - minimum + 1;
        i_vector<ui_vector<variable_t>> hists;
#pragma omp parallel num_threads(this->cores)
        {
            auto n_threads = (size_t)omp_get_num_threads();
            auto thread_id = (size_t)omp_get_thread_num();
#pragma omp single
            {
                hists.resize(n_threads);
                for (size_t i = 0; i < n_threads; ++i) {
                    hists[i].resize(max_letters);
                }
            }

            hists[thread_id].fill(0);

#pragma omp barrier
#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                hists[thread_id][text[i]]++;
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < max_letters; ++i) {
                for (size_t j = 1; j < n_threads; ++j) {
                    hists[0][i] += hists[j][i];
                }
            }

#pragma omp single
            {
                for (size_t i = 1; i < n_threads; ++i) {
                    hists[i].resize(1);
                }
                hists.resize(1);
//                for (size_t i = 1; i < max_letters; ++i) {
//                    hists[0][i] += hists[0][i - 1];
//                }
            }
        }
//        hists[0].resize(1);
//        hists.resize(1);
#ifdef BENCH
        const auto endTimeHist = recomp::timer::now();
        const auto timeSpanHist = endTimeHist - startTimeHist;
        std::cout << " hist=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanHist).count();
        const auto startTimePar = recomp::timer::now();
//        std::cout << std::endl;
#endif

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> distribution(0, 1);
#pragma omp parallel num_threads(this->cores)
        {
            size_t sum_l = 0;
            size_t sum_r = 0;
#pragma omp for schedule(static)
            for (size_t i = 0; i < partition.size(); ++i) {
                if (sum_r < sum_l) {
                    partition[i] = true;
                    sum_r += hists[0][i];
                } else {
                    partition[i] = false;
                    sum_l += hists[0][i];
                }
            }
        }
        partition[0] = 0;  // ensure, that minimum one symbol is in the left partition and one in the right
        partition[partition.size() - 1] = 1;
        hists[0].resize(1);
        hists.resize(1);

#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTimePar;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
        const auto startTimeCount = recomp::timer::now();
//        std::cout << std::endl;
#endif
        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
        std::vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, adj_list.size());
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
