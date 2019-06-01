
#pragma once

#include <omp.h>

#include "recompression/parallel_rnd_recompression.hpp"
#include "recompression/defs.hpp"
#include "recompression/util.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {

namespace parallel {

/**
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class parallel_gr_alternate_recompression : public parallel_rnd_recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename parallel_rnd_recompression<variable_t>::adj_t adj_t;
    typedef typename parallel_rnd_recompression<variable_t>::adj_list_t adj_list_t;
    typedef typename parallel_rnd_recompression<variable_t>::partition_t partition_t;

    inline parallel_gr_alternate_recompression() {
        this->name = "parallel_gr_alternate";
    }

    inline parallel_gr_alternate_recompression(std::string& dataset) : parallel_rnd_recompression<variable_t>(dataset) {
        this->name = "parallel_gr_alternate";
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
    inline virtual void compute_partition(const text_t& text, partition_t& partition, bool& part_l, variable_t minimum) override {
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
        partition[0] = 0;  // ensure, that minimum one symbol is in the left partition and one in the right
        partition[partition.size() - 1] = 1;

        if (text.size() > 10000000) {
#pragma omp parallel num_threads(this->cores)
            {
#pragma omp for schedule(static)
                for (size_t i = 1; i < partition.size() - 1; ++i) {
                    if (i % 2 == 0) {
                        partition[i] = 0;
                    } else {
                        partition[i] = 1;
                    }
                }
            }
        } else {
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
        }
#ifdef BENCH
        const auto endTimePar = recomp::timer::now();
        const auto timeSpanPar = endTimePar - startTimePar;
        std::cout << " random_undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
        const auto startTimeGreedy = recomp::timer::now();
#endif

        std::vector<size_t> bounds;
#pragma omp parallel num_threads(this->cores)
        {
            auto n_threads = (size_t) omp_get_num_threads();
            auto thread_id = (size_t) omp_get_thread_num();

#pragma omp single
            {
                bounds.reserve(n_threads + 1);
                bounds.resize(n_threads + 1, adj_list_size);
            }

#pragma omp for schedule(static)
            for (size_t i = 0; i < adj_list_size; ++i) {
                bounds[thread_id] = i;
                i = adj_list_size;
            }

            size_t i = bounds[thread_id];
            variable_t val;
            if (i == 0) {
                val = std::max(text[adj_list[i]] - minimum, text[adj_list[i] + 1] - minimum);
            } else if (i < bounds[thread_id + 1]) {
                auto comp_val = std::max(text[adj_list[i - 1]] - minimum, text[adj_list[i - 1] + 1] - minimum);

                auto text_i = text[adj_list[i]] - minimum;
                auto text_i1 = text[adj_list[i] + 1] - minimum;
                val = std::max(text_i, text_i1);
                while (i < bounds[thread_id + 1] && comp_val == val) {
                    i++;
                    if (i < bounds[thread_id + 1]) {
                        text_i = text[adj_list[i]] - minimum;
                        text_i1 = text[adj_list[i] + 1] - minimum;
                        val = std::max(text_i, text_i1);
                    }
                }
            }

            int l_count = 0;
            int r_count = 0;
            for (; i < bounds[thread_id + 1]; ++i) {
                auto text_i = text[adj_list[i]] - minimum;
                auto text_i1 = text[adj_list[i] + 1] - minimum;
                if (text_i < text_i1) {
                    std::swap(text_i, text_i1);
                }
                while (i < adj_list_size && val == text_i) {
                    if (partition[text_i1]) {
                        r_count++;
                    } else {
                        l_count++;
                    }
                    i++;
                    if (i < adj_list_size) {
                        text_i = text[adj_list[i]] - minimum;
                        text_i1 = text[adj_list[i] + 1] - minimum;
                        if (text_i < text_i1) {
                            std::swap(text_i, text_i1);
                        }
                    }
                }
                if ((val < text_i || i == adj_list_size) && (l_count > 0 || r_count > 0)) {
                    partition[val] = l_count > r_count;
                    l_count = 0;
                    r_count = 0;
                    val = text_i;
                }
            }
        }
#ifdef BENCH
        const auto endTimeGreedy = recomp::timer::now();
        const auto timeSpanGreedy = endTimeGreedy - startTimeGreedy;
        std::cout << " greedy_undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanGreedy).count();
        const auto startTimeCount = recomp::timer::now();
#endif

        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();

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
