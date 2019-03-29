
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
#include <tlx/simple_vector.hpp>

//#include "recompression.hpp"
#include "parallel_recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class parallel_lp_recompression : public parallel_recompression<variable_t, terminal_count_t> {
 public:
    typedef typename recompression<variable_t, terminal_count_t>::text_t text_t;
    typedef typename parallel_recompression<variable_t, terminal_count_t>::adj_t adj_t;
    typedef typename parallel_recompression<variable_t, terminal_count_t>::adj_list_t adj_list_t;
    typedef typename parallel_recompression<variable_t, terminal_count_t>::partition_t partition_t;

    inline parallel_lp_recompression() {
        this->name = "parallel_lp";
    }

    inline parallel_lp_recompression(std::string& dataset) : parallel_recompression<variable_t, terminal_count_t>(dataset) {
        this->name = "parallel_lp";
    }

    using parallel_recompression<variable_t, terminal_count_t>::recomp;


 protected:

    /**
     * @brief
     *
     * @param text The text
     * @param partition[in,out] The partition
     * @param adj_list[in] The adjacency list of the text
     * @param part_l[in,out] Flag to
     */
    inline virtual void directed_cut(const text_t& text,
                                     partition_t& partition,
                                     adj_list_t& adj_list,
                                     bool& part_l) override {
#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
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
#endif
    }
};

}  // namespace parallel
}  // namespace recomp
