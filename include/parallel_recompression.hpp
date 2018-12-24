
#pragma once

#include <omp.h>

#include <parallel/algorithm>
#include <chrono>
#include <thread>

#ifdef DEBUG
#include <sstream>
#endif

#include <glog/logging.h>
#include <ips4o.hpp>

#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"

#ifndef THREAD_COUNT
#define THREAD_COUNT std::thread::hardware_concurrency()
#endif

namespace recomp {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression {
 public:
    typedef std::vector<variable_t> text_t;
    typedef std::vector<variable_t> alphabet_t;
    typedef std::array<std::vector<std::pair<std::pair<variable_t, variable_t>, size_t>>, 2> multiset_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
     */
    void recomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, const terminal_count_t& alphabet_size) {
        DLOG(INFO) << "recomp input - text size: " << text.size() << " - alphabet size: " << std::to_string(alphabet_size);
        const auto startTime = std::chrono::system_clock::now();

        bool not_finished = text.size() > 1;
        rlslp.terminals = alphabet_size;

        while (not_finished) {
            bcomp(text, rlslp);
            not_finished = text.size() > 1;

            if (not_finished) {
                pcomp(text, rlslp);
                not_finished = text.size() > 1;
            }
        }

        rlslp.root = static_cast<variable_t>(rlslp.non_terminals.size() - 1);

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for recomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void recomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
        recomp(text, rlslp, recomp::CHAR_ALPHABET);
    }


 private:
    /**
     * @brief Replaces all block in the text with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void bcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
        DLOG(INFO) << "BComp input - text size: " << text.size();
        const auto startTime = std::chrono::system_clock::now();

        size_t block_count = 0;
        size_t substr_len = 0;

        std::unordered_map<block_t, variable_t, pair_hash> blocks;
        std::vector<position_t> positions;

        const auto startTimeBlock = std::chrono::system_clock::now();

        size_t *bounds;
#pragma omp parallel num_threads(THREAD_COUNT)
        {
            auto thread_id = omp_get_thread_num();
            auto n_threads = static_cast<size_t>(omp_get_num_threads());
            variable_t block_len = 1;

#pragma omp single
            {
                bounds = new size_t[n_threads + 1];
                bounds[0] = 0;
            }
            std::vector<position_t> t_positions;
            std::unordered_map<block_t, variable_t, pair_hash> t_blocks;
            size_t begin = 0;
            bool add = false;

#pragma omp for schedule(static) nowait reduction(+:block_count) reduction(+:substr_len)
            for (size_t i = 0; i < text.size() - 1; ++i) {
                if (begin == 0) {
                    DLOG(INFO) << "begin at " << i << " for thread " << thread_id;
                    begin = i;
                    if (i == 0) {
                        begin = 1;
                    }
                    add = !(begin > 1 && text[begin - 1] == text[begin]);
                }
                while (i < text.size() - 1 && text[i] == text[i + 1]) {
//            if (text[i - 1] == text[i]) {
                    block_len++;
                    i++;
//                subtr_len++;
////                new_text_size--;
                }
                if (!add) {
                    DLOG(INFO) << "skipping block (" << text[i] << "," << block_len << ")";
                    block_len = 1;
                    add = true;
                }
                if (block_len > 1) {
//            } else if (block_len > 1) {
                    substr_len += block_len - 1;
                    DLOG(INFO) << "Block (" << text[i] << "," << block_len << ") found at " << (i - block_len + 1)
                               << " by thread " << thread_id;
                    t_positions.emplace_back(block_len, i - block_len + 1);
                    block_t block = std::make_pair(text[i], block_len);
                    t_blocks[block] = 1;
                    block_count++;
                    block_len = 1;
                }
            }

            bounds[thread_id + 1] = t_positions.size();

#pragma omp barrier
#pragma omp single
            {
                for (size_t i = 1; i < n_threads + 1; ++i) {
                    bounds[i] += bounds[i - 1];
                }
                positions.resize(positions.size() + bounds[n_threads]);
            }
            std::copy(t_positions.begin(), t_positions.end(), positions.begin() + bounds[thread_id]);

            DLOG(INFO) << "Thread " << thread_id << " inserting blocks " << recomp::util::blocks_to_string<block_t, variable_t>(t_blocks);

#pragma omp critical
            blocks.insert(t_blocks.begin(), t_blocks.end());
        }
        delete[] bounds;

        DLOG(INFO) << "Blocks found: " << block_count;

        DLOG(INFO) << "Blocks are " << recomp::util::blocks_to_string<block_t, variable_t>(blocks);

        std::vector<block_t> sort_blocks(blocks.size());

#pragma omp parallel num_threads(THREAD_COUNT)
        {
            auto iter = blocks.begin();

#pragma omp for schedule(static) nowait
            for (size_t i = 0; i < blocks.size(); ++i) {
                if (iter == blocks.begin()) {
                    std::advance(iter, i);
                }
                DLOG(INFO) << "Adding block (" << (*iter).first.first << "," << (*iter).first.second << ") at index " << i;
                sort_blocks[i] = (*iter).first;
                ++iter;
            }
        }

//    ips4o::parallel::sort(sort_blocks.begin(), sort_blocks.end());
        __gnu_parallel::sort(sort_blocks.begin(), sort_blocks.end(), __gnu_parallel::multiway_mergesort_tag());

        DLOG(INFO) << "Sorted blocks are " << recomp::util::vector_blocks_to_string<block_t>(sort_blocks);

        const auto endTimeBlock = std::chrono::system_clock::now();
        const auto timeSpanBlock = endTimeBlock - startTimeBlock;
        DLOG(INFO) << "Time for finding blocks: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlock).count() << "[ms]";

        block_count = sort_blocks.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + block_count);
        rlslp.resize(nt_count + block_count);
        rlslp.block_count += block_count;

        auto next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);
        const auto startTimeAss = std::chrono::system_clock::now();

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
        for (size_t i = 0; i < sort_blocks.size(); ++i) {
            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_blocks[i].first << ","
                       << sort_blocks[i].second << ") at index " << nt_count + i;
            blocks[sort_blocks[i]] = next_nt + static_cast<variable_t>(i);
            auto len = static_cast<size_t>(sort_blocks[i].second);
            if (sort_blocks[i].first >= static_cast<variable_t>(rlslp.terminals)) {
                len *= rlslp[sort_blocks[i].first - rlslp.terminals].len;
            }
            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_blocks[i].first, -sort_blocks[i].second, len);
        }

        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        DLOG(INFO) << "Time for block nts: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count()
                   << "[ms]";


        const auto startTimeRep = std::chrono::system_clock::now();

        DLOG(INFO) << "Positions are " << recomp::util::positions_to_string<position_t>(positions);

#pragma omp parallel for schedule(static) num_threads(THREAD_COUNT)
        for (size_t i = 0; i < positions.size(); ++i) {
            auto block = std::make_pair(text[positions[i].second], positions[i].first);
            text[positions[i].second] = blocks[block];

            auto length = static_cast<size_t>(positions[i].first);
            for (size_t j = 1; j < length; ++j) {
                text[j + positions[i].second] = -1;
            }
        }

        size_t new_text_size = text.size() - substr_len;
        if (new_text_size > 1 && block_count > 0) {
            size_t copy_i = positions[0].second + 1;
            size_t i = positions[0].second + positions[0].first;  // jump to first position to copy

            for (; i < text.size(); ++i) {
                if (text[i] != -1) {
                    text[copy_i++] = text[i];
                }
            }
        }

        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        DLOG(INFO) << "Time for replacing blocks: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count() << "[ms]";

        DLOG(INFO) << "Shrinking text by " << substr_len << " to length " << new_text_size;

        text.resize(new_text_size);
        text.shrink_to_fit();

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for bcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";

#ifdef DEBUG
        if (text.size() < 30) {
            DLOG(INFO) << "Text: " << recomp::util::text_vector_to_string<text_t>(text);
        }
#endif

        DLOG(INFO) << "BComp ouput - text size: " << text.size() << " - distinct blocks: " << block_count
                   << " - string length reduce by: " << substr_len;
    }

    /**
     * @brief
     *
     * @param text The text
     * @param multiset The multiset
     */
    inline void compute_multiset(const std::vector<variable_t>& text, multiset_t& multiset) {
        const auto startTime = std::chrono::system_clock::now();

        // TODO(Chris): Implement parallel multiset

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for computing multiset: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
    }

    /**
     * @brief
     *
     * @param multiset
     * @param alphabet
     * @param partition
     */
    inline void compute_partition(const multiset_t& multiset, const alphabet_t& alphabet, partition_t& partition) {
        const auto startTime = std::chrono::system_clock::now();

        // TODO(Chris): have to wait for those symbols which are not yet assigned to a partition -> if first letter
        // is assigned to a partition all entries in the multiset that depend on this symbol can compute a partial
        // result and must wait for the next symbol that is not assigned to a partition -> locks only on symbols that
        // are not fully processed

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for computing partition: "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
    }

    /**
     * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void pcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
        DLOG(INFO) << "PComp input - text size: " << text.size();
        const auto startTime = std::chrono::system_clock::now();

        // TODO(Chris): Implement parallel BComp

        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        DLOG(INFO) << "Time for pcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                   << "[ms]";
    }

};

}  // namespace recomp
