
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

#include "parallel_lp_recompression.hpp"
#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"

namespace recomp {

namespace parallel {

template<typename variable_t = var_t>
class parallel_rnd_recompression : public parallel_lp_recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename recompression<variable_t>::bv_t bv_t;
    typedef typename parallel_recompression<variable_t>::adj_t adj_t;
    typedef typename parallel_recompression<variable_t>::adj_list_t adj_list_t;
    typedef ui_vector<bool> partition_t;
    typedef size_t pair_position_t;

    inline parallel_rnd_recompression() {
        this->name = "parallel_rnd";
        this->k = 1;
    }

    inline parallel_rnd_recompression(int k) : k(k) {
        if (k < 1) {
            this->k = 1;
        }
        if (k > 1) {
            this->name = "parallel_rnd" + std::to_string(k);
        } else {
            this->name = "parallel_rnd";
        }
    }

    inline parallel_rnd_recompression(std::string& dataset) : parallel_lp_recompression<variable_t>(dataset) {
        this->name = "parallel_rnd";
        this->k = 1;
    }

    inline parallel_rnd_recompression(std::string& dataset, int k) : k(k), parallel_lp_recompression<variable_t>(dataset) {
        if (k < 1) {
            this->k = 1;
        }
        if (k > 1) {
            this->name = "parallel_rnd" + std::to_string(k);
        } else {
            this->name = "parallel_rnd";
        }
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
     * @param cores The number of cores/threads to use
     */
    inline virtual void recomp(text_t& text,
                               rlslp<variable_t>& rlslp,
                               const size_t& alphabet_size,
                               const size_t cores) override {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        size_t text_size = text.size();
#endif
        this->cores = cores;
        rlslp.terminals = alphabet_size;
        bv_t bv;

        while (text.size() > 1) {
            this->bcomp(text, rlslp, bv);
            this->level++;

            if (text.size() > 1) {
                pcomp(text, rlslp, bv);
                this->level++;
            }
        }

        if (text.size() > 0) {
            rlslp.root = static_cast<variable_t>(text[0]);
            rlslp.is_empty = false;
            this->rename_rlslp(rlslp, bv);
        }

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=" << this->name << "_recompression dataset=" << this->dataset
                  << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << this->level
                  << " cores=" << this->cores << " size=" << text_size << std::endl;
#endif
    }

    using recompression<variable_t>::recomp;


 protected:
    const variable_t DELETED = std::numeric_limits<variable_t>::max();

    int k;

    /**
     * @brief Computes a partitioning (Sigma_l, Sigma_r) of the symbols in the text.
     *
     * @param text[in] The text
     * @param adj_list[in] The adjacency list of the text (text positions)
     * @param partition[out] The partition
     * @param part_l[out] Indicates which partition set is the first one (@code{false} if symbol with value false
     *                    are in Sigma_l, otherwise all symbols with value true are in Sigma_l)
     */
    virtual void compute_partition(const text_t& text, partition_t& partition, bool& part_l, variable_t minimum) {
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

        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
        if (k == 1) {
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
                    partition[i] = distribution(gen);
                }
            }

#ifdef BENCH
            const auto endTimePar = recomp::timer::now();
            const auto timeSpanPar = endTimePar - startTimePar;
            std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
        } else {

            int tmp_cut = 0;
            int cut = 0;
            for (size_t j = 0; j < this->k; ++j) {
#ifdef BENCH
                const auto startTimePar = recomp::timer::now();
#endif
                partition_t tmp_part(partition.size());
                tmp_part[0] = false;  // ensure, that minimum one symbol is in the left partition and one in the right
                tmp_part[tmp_part.size() - 1] = true;
#pragma omp parallel num_threads(this->cores)
                {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<uint8_t> distribution(0, 1);
#pragma omp for schedule(static)
                    for (size_t i = 1; i < tmp_part.size() - 1; ++i) {
                        tmp_part[i] = distribution(gen);
                    }

                    tmp_cut = 0;
#pragma omp for schedule(static) reduction(+:tmp_cut)
                    for (size_t i = 0; i < adj_list.size(); ++i) {
                        variable_t char_i = text[adj_list[i]] - minimum;
                        variable_t char_i1 = text[adj_list[i] + 1] - minimum;
                        if (tmp_part[char_i] != tmp_part[char_i1]) {
                            tmp_cut++;
                        }
                    }
                }
                if (cut < tmp_cut) {
                    cut = tmp_cut;
                    partition.swap(tmp_part);
                }
#ifdef BENCH
                const auto endTimePar = recomp::timer::now();
                const auto timeSpanPar = endTimePar - startTimePar;
                std::cout << " undir_cut" << j << "="
                          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
            }
        }
#ifdef BENCH
            const auto startTimeCount = recomp::timer::now();
#endif
//            int prod_l = 0;
//            int prod_r = 0;
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


    /**
     * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    inline void pcomp(text_t& text, rlslp<variable_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_pcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " cores=" << this->cores;
#endif
        variable_t minimum = std::numeric_limits<variable_t>::max();
#pragma omp parallel for schedule(static) num_threads(this->cores) reduction(min:minimum)
        for (size_t i = 0; i < text.size(); ++i) {
            minimum = std::min(minimum, text[i]);
        }
#ifdef BENCH
        const auto endTimeInAl = recomp::timer::now();
        const auto timeSpanInAl = endTimeInAl - startTime;
        std::cout << " minimum=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanInAl).count();
#endif

        const auto max_letters = rlslp.size() + rlslp.terminals - minimum;
        partition_t partition(max_letters);

        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(text, partition, part_l, minimum);

#ifdef BENCH
        const auto startTimePairs = recomp::timer::now();
        std::cout << " alphabet=" << partition.size();
#endif
        ui_vector<pair_position_t> positions;

        ui_vector<size_t> bounds;
        ui_vector<size_t> pair_counts;
        ui_vector<size_t> compact_bounds;
        ui_vector<size_t> pair_overlaps;
#pragma omp parallel num_threads(this->cores) reduction(+:pair_count)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            size_t end_text = text.size() - 1;

#pragma omp single
            {
                bounds.resize(n_threads + 1);
                bounds[0] = 0;
                compact_bounds.resize(n_threads + 1);
                compact_bounds[n_threads] = end_text;
                pair_counts.resize(n_threads + 1);
                pair_counts[n_threads] = 0;
                pair_overlaps.resize(n_threads + 1);
                pair_overlaps[n_threads] = 0;
            }
            compact_bounds[thread_id] = end_text;
            pair_counts[thread_id] = 0;
            pair_overlaps[thread_id] = 0;
            std::deque<pair_position_t> t_positions;

#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                compact_bounds[thread_id] = i;
                i = text.size();
            }

            for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                if (part_l == partition[text[i] - minimum] && part_l != partition[text[i + 1] - minimum]) {
                    t_positions.emplace_back(i);
                    pair_count++;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                compact_bounds[n_threads] = text.size();
                for (size_t j = 1; j < n_threads + 1; ++j) {
                    bounds[j] += bounds[j - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);

                pair_counts[n_threads] = compact_bounds[n_threads] + pair_overlaps[n_threads] - bounds[n_threads];
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            if (compact_bounds[thread_id] == text.size() - 1) {
                compact_bounds[thread_id] = text.size();
            }

            size_t cb = compact_bounds[thread_id];
            if (cb > 0) {
                if (cb < text.size()) {
                    pair_overlaps[thread_id] = (partition[text[cb - 1] - minimum] == part_l && partition[text[cb] - minimum] != part_l) ? 1 : 0;
                }
                pair_counts[thread_id] = cb + pair_overlaps[thread_id] - bounds[thread_id];
            }
        }
        partition.resize(1);
        pair_overlaps.resize(1);
#ifdef BENCH
        const auto endTimePairs = recomp::timer::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());
        const auto startTimeSort = recomp::timer::now();
#endif
        auto sort_cond = [&](const pair_position_t& i, const pair_position_t& j) {
            auto char_i = text[i];
            auto char_i1 = text[i + 1];
            auto char_j = text[j];
            auto char_j1 = text[j + 1];
            if (char_i == char_j) {
                return char_i1 < char_j1;
            } else {
                return char_i < char_j;
            }
        };
        ips4o::parallel::sort(positions.begin(), positions.end(), sort_cond, this->cores);
#ifdef BENCH
        const auto endTimeSort = recomp::timer::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count());
        const auto startTimeRules = recomp::timer::now();
#endif
        auto nt_count = rlslp.non_terminals.size();
        auto next_nt = rlslp.terminals + nt_count;

        ui_vector<size_t> assign_bounds;
        ui_vector<size_t> distinct_pairs;
        typename recomp::rlslp<variable_t>::production_t productions;
#pragma omp parallel num_threads(this->cores)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());

#pragma omp single
            {
                assign_bounds.resize(n_threads + 1);
                assign_bounds[n_threads] = positions.size();
                distinct_pairs.resize(n_threads + 1);
                distinct_pairs[0] = 0;
            }
            assign_bounds[thread_id] = positions.size();
            distinct_pairs[thread_id + 1] = 0;

#pragma omp for schedule(static)
            for (size_t i = 0; i < positions.size(); ++i) {
                assign_bounds[thread_id] = i;
                i = positions.size();
            }

            size_t i = assign_bounds[thread_id];
            if (i == 0) {
                distinct_pairs[thread_id + 1]++;
                i++;
            }

            for (; i < assign_bounds[thread_id + 1]; ++i) {
                if (text[positions[i]] != text[positions[i - 1]] ||
                    text[positions[i] + 1] != text[positions[i - 1] + 1]) {
                    distinct_pairs[thread_id + 1]++;
                }
            }

#pragma omp barrier
#pragma omp single
            {
                for (size_t j = 1; j < distinct_pairs.size(); ++j) {
                    distinct_pairs[j] += distinct_pairs[j - 1];
                }

                auto pc = distinct_pairs[n_threads];
                auto rlslp_size = nt_count + pc;
                productions.resize(rlslp_size);
                bv.resize(rlslp_size, false);
            }
#pragma omp for schedule(static)
            for (size_t k = 0; k < rlslp.size(); ++k) {
                productions[k] = rlslp[k];
            }

            i = assign_bounds[thread_id];
            auto last_var = next_nt + distinct_pairs[thread_id] - 1;
            variable_t last_char1 = 0;
            variable_t last_char2 = 0;
            if (i > 0 && i < assign_bounds[thread_id + 1]) {
                last_char1 = text[positions[i - 1]];
                last_char2 = text[positions[i - 1] + 1];
            }

#pragma omp barrier
            size_t j = 0;
            if (thread_id == 0) {
                last_char1 = text[positions[i]];
                last_char2 = text[positions[i] + 1];
                size_t len = 0;
                if (last_char1 >= rlslp.terminals) {
                    len = rlslp[last_char1 - rlslp.terminals].len;
                } else {
                    len = 1;
                }
                if (last_char2 >= rlslp.terminals) {
                    len += rlslp[last_char2 - rlslp.terminals].len;
                } else {
                    len += 1;
                }
                productions[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(last_char1, last_char2, len);
                j++;
                last_var++;
                text[positions[i]] = last_var;
                text[positions[i] + 1] = DELETED;
                i++;
            }

            for (; i < assign_bounds[thread_id + 1]; ++i) {
                auto char_i1 = text[positions[i]];
                auto char_i2 = text[positions[i] + 1];
                if (char_i1 == last_char1 && char_i2 == last_char2) {
                    text[positions[i]] = last_var;
                } else {
                    size_t len = 0;
                    if (char_i1 >= rlslp.terminals) {
                        len = rlslp[char_i1 - rlslp.terminals].len;
                    } else {
                        len = 1;
                    }
                    if (char_i2 >= rlslp.terminals) {
                        len += rlslp[char_i2 - rlslp.terminals].len;
                    } else {
                        len += 1;
                    }
                    productions[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(char_i1, char_i2, len);
                    j++;
                    last_var++;
                    text[positions[i]] = last_var;
                    last_char1 = char_i1;
                    last_char2 = char_i2;
                }
                text[positions[i] + 1] = DELETED;
            }
        }
        std::swap(rlslp.non_terminals, productions);
        productions.resize(1);
        positions.resize(1);
#ifdef BENCH
        const auto endTimeRules = recomp::timer::now();
        const auto timeSpanRules = endTimeRules - startTimeRules;
        std::cout << " rules=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRules).count()
                  << " productions=" << distinct_pairs[distinct_pairs.size() - 1] << " elements=" << pair_count;
#endif

        this->compact(text, compact_bounds, pair_counts, pair_count);

#ifdef BENCH
        const auto endTime = recomp::timer::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }
};

}  // namespace parallel
}  // namespace recomp
