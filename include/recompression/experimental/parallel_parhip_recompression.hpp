
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression/parallel_rnd_recompression.hpp"
#include "recompression/defs.hpp"
#include "recompression/util.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {

namespace parallel {

/**
 * @brief This class is a parallel implementation of the recompression computing a parallel undirected maximum cut based
 * on the partition computed by parhip.
 *
 * @tparam variable_t The type of non-terminals
 */
template<typename variable_t = var_t>
class parallel_parhip_recompression : public parallel_rnd_recompression<variable_t> {
 public:
    typedef typename recompression<variable_t>::text_t text_t;
    typedef typename parallel_rnd_recompression<variable_t>::adj_t adj_t;
    typedef typename parallel_rnd_recompression<variable_t>::adj_list_t adj_list_t;
    typedef typename parallel_rnd_recompression<variable_t>::partition_t partition_t;
    typedef typename parallel_rnd_recompression<variable_t>::bv_t bv_t;
    typedef size_t pair_position_t;
    typedef size_t idxtype;

    std::string parhip;
    std::string dir;

    inline parallel_parhip_recompression() {
        this->name = "parallel_parhip";
    }

    inline parallel_parhip_recompression(std::string& dataset) : parallel_rnd_recompression<variable_t>(dataset) {
        this->name = "parallel_parhip";
    }

    inline parallel_parhip_recompression(std::string& dataset, std::string parhip, std::string dir)
            : parallel_rnd_recompression<variable_t>(dataset) {
        this->name = "parallel_parhip";
        this->parhip = parhip;
        this->dir = dir;
    }

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
        std::cout << "RESULT algo=" << this->name << "_recompression dataset=" << this->dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << this->level
                  << " cores=" << this->cores << " size=" << text_size << std::endl;
#endif
    }

    using recompression<variable_t>::recomp;


 protected:
    const variable_t DELETED = std::numeric_limits<variable_t>::max();

    /**
     * @brief Compacts the text by copying the symbols.
     *
     * @param text[in,out] The text
     * @param compact_bounds[in] The bounds for the cores to copy from
     * @param copy_bounds[in] The bounds for the cores to copy to
     * @param count[in] The number of found blocks/pairs
     * @param mapping[in] The mapping of the effective alphabet to the replaced symbols
     */
    inline void compact(text_t& text,
                        const ui_vector<size_t>& compact_bounds,
                        const ui_vector<size_t>& copy_bounds,
                        size_t count,
                        const ui_vector<variable_t>& mapping) {
#ifdef BENCH
        const auto startTimeCompact = recomp::timer::now();
#endif
        size_t new_text_size = copy_bounds[copy_bounds.size() - 1];
        if (new_text_size > 1 && count > 0) {
#ifdef BENCH
            const auto startTimeNewText = recomp::timer::now();
#endif
            text_t new_text(new_text_size);
#ifdef BENCH
            const auto endTimeNT = recomp::timer::now();
            const auto timeSpanNT = endTimeNT - startTimeNewText;
            std::cout << " init_new_text="
                      << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanNT).count();
            const auto startTimeCopy = recomp::timer::now();
#endif

#pragma omp parallel num_threads(this->cores)
            {
                auto thread_id = omp_get_thread_num();
                size_t copy_i = copy_bounds[thread_id];
                for (size_t i = compact_bounds[thread_id]; i < compact_bounds[thread_id + 1]; ++i) {
                    if (text[i] != DELETED) {
                        if (text[i] >= mapping.size()) {
                            new_text[copy_i++] = text[i];
                        } else {
                            new_text[copy_i++] = mapping[text[i]];
                        }
                    }
                }
            }
#ifdef BENCH
            const auto endTimeCopy = recomp::timer::now();
            const auto timeSpanCopy = endTimeCopy - startTimeCopy;
            std::cout << " copy="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
            const auto startTimeMove = recomp::timer::now();
#endif

            std::swap(text, new_text);
#ifdef BENCH
            const auto endTimeMove = recomp::timer::now();
            const auto timeSpanMove = endTimeMove - startTimeMove;
            std::cout << " move="
                      << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMove).count());
#endif
        } else if (new_text_size == 1) {
            text.resize(new_text_size);
        }
#ifdef BENCH
        const auto endTimeCompact = recomp::timer::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif
    }

    /**
     * @brief Computes for each symbol in the text its rank in the alphabet and replaces all symbols with it.
     *
     * @param text[in,out] The text
     * @param rlslp[in] The rlslp
     * @param mapping[out] The mapping from the rank to the symbol
     */
    inline void compute_mapping(text_t& text,
                                rlslp<variable_t>& rlslp,
                                ui_vector<variable_t>& mapping) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
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
        const auto startTimeAlpha = recomp::timer::now();
#endif

        const auto max_letters = rlslp.size() + rlslp.terminals - minimum;
        ui_vector<variable_t> hist(max_letters);
        ui_vector<size_t> bounds;
        ui_vector<size_t> hist_bounds;
#pragma omp parallel num_threads(this->cores)
        {
            auto n_threads = (size_t) omp_get_num_threads();
            auto thread_id = omp_get_thread_num();

#pragma omp single
            {
                hist_bounds.resize(n_threads + 1);
                hist_bounds[n_threads] = hist.size();
                bounds.resize(n_threads + 1);
                bounds[n_threads] = 0;
            }
            bounds[thread_id] = 0;
            hist_bounds[thread_id] = hist.size();

#pragma omp for schedule(static)
            for (size_t i = 0; i < hist.size(); ++i) {
                hist_bounds[thread_id] = i;
                i = hist.size();
            }

            for (size_t i = hist_bounds[thread_id]; i < hist_bounds[thread_id + 1]; ++i) {
                hist[i] = 0;
            }

#pragma omp barrier
#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                hist[text[i] - minimum] = 1;
            }

            for (size_t i = hist_bounds[thread_id]; i < hist_bounds[thread_id + 1]; ++i) {
                if (hist[i] == 1) {
                    bounds[thread_id + 1]++;
                }
            }

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < bounds.size(); ++i) {
                    bounds[i] += bounds[i - 1];
                }
                mapping.resize(bounds[n_threads]);
            }

            size_t j = bounds[thread_id];
            for (size_t i = hist_bounds[thread_id]; i < hist_bounds[thread_id + 1]; ++i) {
                if (hist[i] == 1) {
                    hist[i] = j;
                    mapping[j++] = i + minimum;
                }
            }

#pragma omp barrier
#pragma omp for schedule(static)
            for (size_t i = 0; i < text.size(); ++i) {
                text[i] = hist[text[i] - minimum];
            }
        }
        hist.resize(1);
        bounds.resize(1);
        hist_bounds.resize(1);
#ifdef BENCH
        const auto endAlphaTime = recomp::timer::now();
        const auto timeSpanAlpha = endAlphaTime - startTimeAlpha;
        std::cout << " mapping=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAlpha).count();
        const auto endTimeMapping = recomp::timer::now();
        const auto timeSpanMapping = endTimeMapping - startTime;
        std::cout << " compute_mapping=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMapping).count();
#endif
    }


    /**
     * @brief Computes a partitioning (Sigma_l, Sigma_r) of the symbols in the text.
     *
     * @param text[in] The text
     * @param partition[out] The partition
     * @param part_l[out] Indicates which partition set is the first one (@code{false} if symbol with value false
     *                    are in Sigma_l, otherwise all symbols with value true are in Sigma_l)
     */
    inline void compute_partition(const text_t& text, partition_t& partition, bool& part_l) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        const auto startTimeGraph = recomp::timer::now();
#endif

        const size_t n = partition.size();
        auto *xadj = new idxtype[n + 1];
        xadj[0] = 0;

        size_t m = 0;
        std::vector<std::unordered_map<variable_t, variable_t>> adjncys(n);
        for (size_t i = 0; i < text.size() - 1; ++i) {
            auto c = text[i];
            auto c1 = text[i + 1];
            if (c < c1) {
                std::swap(c, c1);
            }
            if (adjncys[c].find(c1) == adjncys[c].end()) {
                adjncys[c].insert(std::make_pair(c1, 1));
                m++;
            } else {
                adjncys[c][c1]++;
            }
        }

        std::string data = this->dataset;
        util::replace_all(data, "\\", "");
        std::ofstream out_file;
        const auto exec = "mpirun -n " + std::to_string(this->cores) + " " + parhip + " " + dir + data + ".graph --k=" +
                          std::to_string(this->cores) +
                          " --preconfiguration=ultrafastsocial --imbalance=3 --save_partition";
        out_file.open(data + ".graph", std::ofstream::out | std::ofstream::trunc);
        out_file << n << " " << ((n * n - (2 * m) - n) / 2) << " \n";

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i != j) {
                    bool in_complement;
                    if (i < j) {
                        in_complement = adjncys[j].find(i) == adjncys[j].end();
                    } else {
                        in_complement = adjncys[i].find(j) == adjncys[i].end();
                    }
                    if (in_complement) {
                        out_file << std::to_string(j + 1) << " ";
                    }
                }
            }
            out_file << "\n";
        }
        out_file.close();

#ifdef BENCH
        const auto endTimeGraph = recomp::timer::now();
        const auto timeSpanGraph = endTimeGraph - startTimeGraph;
        std::cout << " build_graph=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanGraph).count() << std::endl;
        const auto startTimeKahip = recomp::timer::now();
#endif
        auto status = system(exec.c_str());
        if (status < 0) {
            std::cerr << "Parhip failed" << std::endl;
            exit(-1);
        }

        std::ifstream in_file(dir + "tmppartition.txtp", std::ifstream::in | std::ios::binary | std::ifstream::ate);
        std::string part_str;
        in_file.seekg(0, std::ios::beg);

        auto *part = new idxtype[n];
        for (size_t i = 0; i < n; ++i) {
            std::getline(in_file, part_str);
            part[i] = util::str_to_int(part_str);
        }

#ifdef BENCH
        const auto endTimeKahip = recomp::timer::now();
        const auto timeSpanKahip = endTimeKahip - startTimeKahip;
        std::cout << " parhip=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanKahip).count();
        const auto startTimeCopyPart = recomp::timer::now();
#endif
        i_vector<std::deque<variable_t>> nodes(this->cores);
        for (size_t i = 0; i < partition.size(); ++i) {
            nodes[part[i]].emplace_back(i);
        }

#pragma omp parallel num_threads(this->cores)
        {
#pragma omp for schedule(static)
            for (size_t i = 0; i < partition.size(); ++i) {
                partition[i] = false;
            }

            int l_count = 0;
            int r_count = 0;

            auto thread_id = omp_get_thread_num();
            size_t p_size = nodes[thread_id].size();
            auto t_nodes = nodes[thread_id];
            for (size_t i = 0; i < p_size; ++i) {
                for (const auto& adj_node : adjncys[t_nodes[i]]) {
                    if (!partition[adj_node.first]) {
                        l_count += adj_node.second;
                    } else {
                        r_count += adj_node.second;
                    }
                }
                partition[t_nodes[i]] = l_count > r_count;
                l_count = 0;
                r_count = 0;
            }
        }
        adjncys.resize(1);
        adjncys.shrink_to_fit();
        adjncys[0].clear();
        bool diff = false;

#pragma omp parallel for num_threads(this->cores) schedule(static) reduction(|:diff)
        for (size_t i = 1; i < partition.size(); ++i) {
            diff |= partition[i] != partition[i - 1];
        }

        if (!diff) {
            partition[0] = false;  // ensure, that minimum one symbol is in the left partition and one in the right
            partition[partition.size() - 1] = true;
        }
        delete[] xadj;
        delete[] part;
#ifdef BENCH
        const auto endTimeCopyPart = recomp::timer::now();
        const auto timeSpanCopyPart = endTimeCopyPart - startTimeCopyPart;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopyPart).count();
        const auto startTimeAdjInit = recomp::timer::now();
#endif

        const size_t adj_list_size = text.size() - 1;
        adj_list_t adj_list(adj_list_size);
#ifdef BENCH
        const auto endTimeAdjInit = recomp::timer::now();
        const auto timeSpanAdjInit = endTimeAdjInit - startTimeAdjInit;
        std::cout << " init_adj_vec=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAdjInit).count();
#endif
        this->compute_adj_list(text, adj_list);

        ui_vector<size_t> bounds;
#ifdef BENCH
        const auto startTimeCount = recomp::timer::now();
#endif

        int lr_count = 0;
        int rl_count = 0;
        int prod_l = 0;
        int prod_r = 0;
#pragma omp parallel num_threads(this->cores) reduction(+:lr_count) reduction(+:rl_count) reduction(+:prod_r) reduction(+:prod_l)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = (size_t) omp_get_num_threads();
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

    /**
     * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param bv[in,out] The bitvector to indicate which rules derive blocks
     */
    inline void pcomp(text_t& text, rlslp<variable_t>& rlslp, bv_t& bv) {
#ifdef BENCH
        const auto startTime = recomp::timer::now();
        std::cout << "RESULT algo=" << this->name << "_pcomp dataset=" << this->dataset << " text=" << text.size()
                  << " level=" << this->level << " cores=" << this->cores;
#endif
        ui_vector<variable_t> mapping;
        compute_mapping(text, rlslp, mapping);

        partition_t partition(mapping.size());

        size_t pair_count = 0;
        bool part_l = false;
        compute_partition(text, partition, part_l);

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
                if (part_l == partition[text[i]] && part_l != partition[text[i + 1]]) {
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
                    pair_overlaps[thread_id] = (partition[text[cb - 1]] == part_l && partition[text[cb]] != part_l) ? 1
                                                                                                                    : 0;
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
                last_char1 = mapping[text[positions[i - 1]]];
                last_char2 = mapping[text[positions[i - 1] + 1]];
            }

#pragma omp barrier
            size_t j = 0;
            if (thread_id == 0) {
                last_char1 = mapping[text[positions[i]]];
                last_char2 = mapping[text[positions[i] + 1]];
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
                productions[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(last_char1, last_char2,
                                                                                                 len);
                j++;
                last_var++;
                text[positions[i]] = last_var;
                text[positions[i] + 1] = DELETED;
                i++;
            }

            for (; i < assign_bounds[thread_id + 1]; ++i) {
                auto char_i1 = mapping[text[positions[i]]];
                auto char_i2 = mapping[text[positions[i] + 1]];
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
                    productions[nt_count + distinct_pairs[thread_id] + j] = non_terminal<variable_t>(char_i1, char_i2,
                                                                                                     len);
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

        this->compact(text, compact_bounds, pair_counts, pair_count, mapping);

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
