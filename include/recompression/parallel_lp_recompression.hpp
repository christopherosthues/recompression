
#pragma once

#include <omp.h>

#ifdef BENCH
#include <iostream>
#endif

#include <string>
#include <vector>

#include <ips4o.hpp>

#include "parallel_recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"

namespace recomp {

namespace parallel {

/**
 * This class is a parallel implementation of the recompression computing an undirected maximum cut using the sequential
 * greedy approximation algorithm. Additionally the directed maximum cut algorithm counts the number of new generated
 * production rules to choose the partition which generates less if the cut values of the two partition of the directed
 * maximum cut are equal.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class parallel_lp_recompression : public parallel_recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename parallel_recompression<variable_t>::adj_t adj_t;
    typedef typename parallel_recompression<variable_t>::adj_list_t adj_list_t;
    typedef typename parallel_recompression<variable_t>::partition_t partition_t;

    inline parallel_lp_recompression() {
        this->name = "parallel_lp";
    }

    inline parallel_lp_recompression(std::string& dataset) : parallel_recompression<variable_t>(dataset) {
        this->name = "parallel_lp";
    }

    using parallel_recompression<variable_t>::recomp;


 protected:
    /**
     * @brief Computes and sorts the adjacency list of the text in ascending order.
     *
     * The adjacency list is stored as a list of text positions.
     *
     * @param text The text
     * @param adj_list[out] The adjacency list (represented as text positions)
     */
    inline virtual void compute_adj_list(const text_t& text, adj_list_t& adj_list) override {
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
                    // char_j1 > char_j -> (char_i, char_i1, 0) (char_j1, char_j, 1)
                    bool less = char_i < char_j1;
                    if (char_i == char_j1) {
                        if (char_i1 == char_j) {
                            return true;
                        }
                        less = char_i1 < char_j;
                    }
                    return less;
                }
            } else {
                if (char_j > char_j1) {
                    // char_i1 > char_i -> (char_i1, char_i, 1) (char_j, char_j1, 0)
                    bool less = char_i1 < char_j;
                    if (char_i1 == char_j) {
                        if (char_i == char_j1) {
                            return false;
                        }
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
     * @brief Computes a directed cut from the given partition defining an undirected cut.
     *
     * If the number of pairs of the partition (left,right) and (right,left) in the text are the same the partition
     * that generated fewer productions is chosen.
     *
     * @param text The text
     * @param partition[in,out] The partition
     * @param adj_list[in] The adjacency list of the text
     * @param part_l[in,out] Flag to indicate which partition set is the left set
     * @param minimum[in] The smallest symbol in the text
     */
    inline virtual void directed_cut(const text_t& text,
                                     partition_t& partition,
                                     adj_list_t& adj_list,
                                     bool& part_l,
                                     variable_t minimum) override {
#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
        ui_vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                bounds.resize(n_threads + 1);
                bounds[n_threads] = adj_list.size();
            }
            bounds[thread_id] = adj_list.size();

#pragma omp for schedule(static)
            for (size_t i = 0; i < adj_list.size(); ++i) {
                bounds[thread_id] = i;
                i = adj_list.size();
            }

            variable_t last_i = 0;  // avoid more random access than necessary
            variable_t last_i1 = 0;
            size_t i = bounds[thread_id];
            if (i == 0) {
                last_i = text[adj_list[i]] - minimum;
                last_i1 = text[adj_list[i] + 1] - minimum;
                if (!partition[last_i] && partition[last_i1]) {
                    lr_count++;
                    prod_l++;
                } else if (partition[last_i] && !partition[last_i1]) {
                    rl_count++;
                    prod_r++;
                }
                i++;
            } else if (i < adj_list.size()) {
                last_i = text[adj_list[i - 1]] - minimum;
                last_i1 = text[adj_list[i - 1] + 1] - minimum;
            }

            for (; i < bounds[thread_id + 1]; ++i) {
                variable_t char_i = text[adj_list[i]] - minimum;
                variable_t char_i1 = text[adj_list[i] + 1] - minimum;
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
        // If number of pairs are the same, choose that one that generates fewer productions
        if (rl_count == lr_count) {
            part_l = prod_r < prod_l;
        }
#ifdef BENCH
        const auto endTimeCount = recomp::timer::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " lr=" << lr_count << " rl=" << rl_count << " dir_cut="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif
    }
};

}  // namespace parallel
}  // namespace recomp
