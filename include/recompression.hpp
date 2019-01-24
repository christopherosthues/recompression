
#pragma once

#include <algorithm>
#include <chrono>
#include <climits>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef DEBUG

#include <sstream>

#endif

#include <glog/logging.h>
#include <ips4o.hpp>

#include "defs.hpp"
#include "util.hpp"
#include "rlslp.hpp"
#include "radix_sort.hpp"

namespace recomp {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression {
 public:
    typedef std::vector<variable_t> text_t;
    typedef std::vector<variable_t> alphabet_t;
    typedef std::vector<std::tuple<variable_t, variable_t, bool>> adj_list_t;
    typedef std::unordered_map<variable_t, bool> partition_t;

    typedef std::pair<variable_t, variable_t> block_t;
    typedef block_t pair_t;
    typedef std::pair<variable_t, size_t> position_t;
    typedef size_t pair_position_t;

    std::string dataset = "data";
    size_t level = 0;

    recompression() = default;

    recompression(std::string& dataset) : dataset(dataset) {}

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
     */
    void recomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp, const terminal_count_t& alphabet_size) {
//        DLOG(INFO) << "recomp input - text size: " << text.size() << " - alphabet size: "
//                   << std::to_string(alphabet_size);
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
#endif
        rlslp.terminals = alphabet_size;

        while (text.size() > 1) {
            bcomp(text, rlslp);
            level++;

            if (text.size() > 1) {
                pcomp(text, rlslp);
                level++;
            }
        }

        if (!rlslp.empty()) {
            rlslp.root = static_cast<variable_t>(rlslp.size() - 1);
        }
#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "RESULT algo=seq_recompression dataset=" << dataset << " time="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count())
                  << " production=" << rlslp.size() << " terminals=" << rlslp.terminals << " level=" << level
                  << std::endl;
#endif
    }

    /**
     * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void recomp(text_t& text, rlslp <variable_t, terminal_count_t>& rlslp) {
        recomp(text, rlslp, recomp::CHAR_ALPHABET);
    }


 private:
    const variable_t DELETED = UINT_MAX;

    /**
     * @brief Replaces all block in the text with new non-terminals.
     *
     * @param text The text
     * @param rlslp The rlslp
     */
    void bcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
        // TODO(Chris): Benchmark output
//        DLOG(INFO) << "BComp input - text size: " << text.size();
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
        std::cout << "RESULT algo=seq_bcomp dataset=" << dataset << " text=" << text.size()
                  << " level=" << level;
#endif

        size_t block_count = 0;
        size_t substr_len = 0;

#ifdef BENCH
        const auto startTimeBlocks = std::chrono::system_clock::now();
#endif
        std::unordered_map<block_t, variable_t, pair_hash> blocks;
        std::vector<position_t> positions;

        variable_t block_len = 1;

        for (size_t i = 0; i < text.size() - 1; ++i) {
            while (i < text.size() - 1 && text[i] == text[i + 1]) {
                block_len++;
                i++;
            }
            if (block_len > 1) {
                substr_len += block_len - 1;
//                DLOG(INFO) << "Block (" << text[i] << "," << block_len << ") found at " << (i - block_len + 1);
                positions.emplace_back(block_len, i - block_len + 1);
                block_t block = std::make_pair(text[i], block_len);
                blocks[block] = 1;
                block_count++;
                block_len = 1;
            }
        }
#ifdef BENCH
        const auto endTimeBlocks = std::chrono::system_clock::now();
        const auto timeSpanBlocks = endTimeBlocks - startTimeBlocks;
        std::cout << " find_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanBlocks).count());
#endif

//        DLOG(INFO) << "Blocks found: " << block_count;
//        DLOG(INFO) << "Blocks are " << recomp::util::blocks_to_string<block_t, variable_t>(blocks);

#ifdef BENCH
        const auto startTimeCopy = std::chrono::system_clock::now();
#endif
        std::vector<block_t> sort_blocks(blocks.size());

        auto iter = blocks.begin();

        for (size_t i = 0; i < blocks.size(); ++i) {
//            DLOG(INFO) << "Adding block (" << (*iter).first.first << "," << (*iter).first.second << ") at index "
//                       << i;
            sort_blocks[i] = (*iter).first;
            ++iter;
        }
#ifdef BENCH
        const auto endTimeCopy = std::chrono::system_clock::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
#endif

#ifdef BENCH
        const auto startTimeSort = std::chrono::system_clock::now();
#endif
//        lsd_radix_sort(sort_blocks);
        ips4o::sort(sort_blocks.begin(), sort_blocks.end());
#ifdef BENCH
        const auto endTimeSort = std::chrono::system_clock::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_blocks.size() << " blocks=" << block_count;;
#endif

//        DLOG(INFO) << "Sorted blocks are " << recomp::util::vector_blocks_to_string<block_t>(sort_blocks);

#ifdef BENCH
        const auto startTimeAss = std::chrono::system_clock::now();
#endif
        block_count = sort_blocks.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + block_count);
        rlslp.resize(nt_count + block_count, true);
        rlslp.block_count += block_count;

        auto next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);


        for (size_t i = 0; i < sort_blocks.size(); ++i) {
//            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_blocks[i].first << ","
//                       << sort_blocks[i].second << ") at index " << nt_count + i;
            blocks[sort_blocks[i]] = next_nt + static_cast<variable_t>(i);
            auto len = static_cast<size_t>(sort_blocks[i].second);
            if (sort_blocks[i].first >= static_cast<variable_t>(rlslp.terminals)) {
                len *= rlslp[sort_blocks[i].first - rlslp.terminals].len;
            }
            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_blocks[i].first, sort_blocks[i].second, len);
        }
#ifdef BENCH
        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " block_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif
//        DLOG(INFO) << "Time for block nts: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count()
//                   << "[ms]";


#ifdef BENCH
        const auto startTimeRep = std::chrono::system_clock::now();
#endif

//        DLOG(INFO) << "Positions are " << recomp::util::block_positions_to_string<position_t>(positions);

        for (size_t i = 0; i < positions.size(); ++i) {
            auto block = std::make_pair(text[positions[i].second], positions[i].first);
            text[positions[i].second] = blocks[block];

            auto length = static_cast<size_t>(positions[i].first);
            for (size_t j = 1; j < length; ++j) {
                text[j + positions[i].second] = DELETED;
            }
        }
#ifdef BENCH
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_blocks="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif

#ifdef BENCH
        const auto startTimeCompact = std::chrono::system_clock::now();
#endif
        size_t new_text_size = text.size() - substr_len;
        if (new_text_size > 1 && block_count > 0) {
            size_t copy_i = positions[0].second + 1;
            size_t i = positions[0].second + positions[0].first;  // jump to first position to copy

            for (; i < text.size(); ++i) {
                if (text[i] != DELETED) {
                    text[copy_i++] = text[i];
                }
            }
        }
#ifdef BENCH
        const auto endTimeCompact = std::chrono::system_clock::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

//        DLOG(INFO) << "Shrinking text by " << substr_len << " to length " << new_text_size;

        text.resize(new_text_size);
        text.shrink_to_fit();

#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
    }

    /**
     * @brief
     *
     * @param text The text
     * @param adj_list The adj_list
     */
    inline void compute_adj_list(const text_t& text, adj_list_t& adj_list) {
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
#endif

        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (text[i] > text[i + 1]) {
                adj_list[i] = std::make_tuple(text[i], text[i + 1], false);
            } else {
                adj_list[i] = std::make_tuple(text[i + 1], text[i], true);
            }
        }

#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " adj_list=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
//        DLOG(INFO) << "Time for computing adj_list: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
    }

    /**
     * @brief
     *
     * @param adj_list
     * @param alphabet
     * @param partition
     */
    inline void compute_partition(const adj_list_t& adj_list, /*const alphabet_t& alphabet,*/ partition_t& partition) {
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
#endif

//        DLOG(INFO) << util::text_vector_to_string(alphabet);
        int l_count = 0;
        int r_count = 0;
        if (adj_list.size() > 0) {
            if (partition[std::get<0>(adj_list[0])]) {
                r_count++;
            } else {
                l_count++;
            }
        }
        for (size_t i = 1; i < adj_list.size(); ++i) {
            if (std::get<0>(adj_list[i - 1]) < std::get<0>(adj_list[i])) {
//            LOG(INFO) << "Setting " << std::get<0>(adj_list[i - 1]) << " to " << (l_count > r_count) << " ; "
//                      << l_count << ", " << r_count;
                partition[std::get<0>(adj_list[i - 1])] = l_count > r_count;
                l_count = 0;
                r_count = 0;
            }
            if (partition[std::get<1>(adj_list[i])]) {
                r_count++;
            } else {
                l_count++;
            }
        }
        partition[std::get<0>(adj_list[adj_list.size() - 1])] = l_count > r_count;
//    LOG(INFO) << "Setting " << std::get<0>(adj_list[adj_list.size() - 1]) << " to " << (l_count > r_count) << " ; "
//              << l_count << ", " << r_count;
#ifdef BENCH
        const auto endTimePar = std::chrono::system_clock::now();
        const auto timeSpanPar = endTimePar - startTime;
        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
#endif
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
//        int l_count = 0;
//        int r_count = 0;
//        size_t j = 0;
//        for (size_t i = 0; i < adj_list.size(); ++i) {
//            while (j < alphabet.size() && std::get<0>(adj_list[i]) > alphabet[j]) {
//                partition[alphabet[j]] = l_count > r_count;
//                DLOG(INFO) << "Setting " << alphabet[j] << " to " << (l_count > r_count);
//                j++;
//                l_count = 0;
//                r_count = 0;
//            }
//            if (partition[std::get<1>(adj_list[i])]) {
//                r_count++;
//            } else {
//                l_count++;
//            }
//        }
//        partition[alphabet[j]] = l_count > r_count;
//        const auto endTimePar = std::chrono::system_clock::now();
//        const auto timeSpanPar = endTimePar - startTime;
//        std::cout << " undir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPar).count();
//        DLOG(INFO) << "j: " << j;
//        DLOG(INFO) << "Partition: " << util::partition_to_string(partition);

#ifdef BENCH
        const auto startTimeCount = std::chrono::system_clock::now();
#endif
        int lr_count = 0;
        int rl_count = 0;
        for (size_t i = 0; i < adj_list.size(); ++i) {
            if (std::get<2>(adj_list[i])) {
                if (!partition[std::get<0>(adj_list[i])] &&
                    partition[std::get<1>(adj_list[i])]) {  // bc in text and b in right set and c in left
                    rl_count++;
                } else if (partition[std::get<0>(adj_list[i])] &&
                           !partition[std::get<1>(adj_list[i])]) {  // bc in text and b in left set and c in right
                    lr_count++;
                }
            } else {
                if (!partition[std::get<0>(adj_list[i])] &&
                    partition[std::get<1>(adj_list[i])]) {  // cb in text and c in left set and b in right
                    lr_count++;
                } else if (partition[std::get<0>(adj_list[i])] &&
                           !partition[std::get<1>(adj_list[i])]) {  // cb in text and c in right set and b in left
                    rl_count++;
                }
            }
        }
#ifdef BENCH
        const auto endTimeCount = std::chrono::system_clock::now();
        const auto timeSpanCount = endTimeCount - startTimeCount;
        std::cout << " dir_cut=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCount).count();
#endif

        if (rl_count > lr_count) {
//            DLOG(INFO) << "Swap partition sets";
            for (auto iter = partition.begin(); iter != partition.end(); ++iter) {
                (*iter).second = !(*iter).second;
            }
//            DLOG(INFO) << "Partition: " << util::partition_to_string(partition);
        }
#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " partition=" << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
#endif
//        DLOG(INFO) << "Time for computing partition: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
    }

/**
 * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
 *
 * @param text The text
 * @param rlslp The rlslp
 */
    void pcomp(text_t& text, rlslp<variable_t, terminal_count_t>& rlslp) {
//        DLOG(INFO) << "PComp input - text size: " << text.size();
#ifdef BENCH
        const auto startTime = std::chrono::system_clock::now();
        std::cout << "RESULT algo=seq_pcomp dataset=" << dataset << " text=" << text.size() << " level=" << level;
#endif

        partition_t partition;
        for (size_t i = 0; i < text.size(); ++i) {
            partition[text[i]] = false;
        }
//        alphabet_t alphabet(partition.size());
//        auto part_iter = partition.begin();
//        for (size_t i = 0; i < partition.size(); ++i, ++part_iter) {
//            alphabet[i] = (*part_iter).first;
//        }
//        std::sort(alphabet.begin(), alphabet.end());

#ifdef BENCH
        std::cout << " alphabet=" << partition.size();
#endif
        adj_list_t adj_list(text.size() - 1);
        compute_adj_list(text, adj_list);

#ifdef BENCH
        const auto startTimeMult = std::chrono::system_clock::now();
#endif
//        std::sort(adj_list.begin(), adj_list.end());
        ips4o::sort(adj_list.begin(), adj_list.end());
#ifdef BENCH
        const auto endTimeMult = std::chrono::system_clock::now();
        const auto timeSpanMult = endTimeMult - startTimeMult;
        std::cout << " sort_adj_list="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanMult).count());
#endif

        size_t pair_count = 0;

        compute_partition(adj_list, /*alphabet,*/ partition);

#ifdef BENCH
        const auto startTimePairs = std::chrono::system_clock::now();
#endif
        std::unordered_map<std::pair<variable_t, variable_t>, variable_t, pair_hash> pairs;
        std::vector<pair_position_t> positions;

        for (size_t i = 0; i < text.size() - 1; ++i) {
            if (!partition[text[i]] && partition[text[i + 1]]) {
//                DLOG(INFO) << "Pair (" << text[i] << "," << text[i + 1] << ") found at " << i;
                auto pair = std::make_pair(text[i], text[i + 1]);
                pairs[pair] = 1;
                positions.emplace_back(i);
                pair_count++;
            }
        }
#ifdef BENCH
        const auto endTimePairs = std::chrono::system_clock::now();
        const auto timeSpanPairs = endTimePairs - startTimePairs;
        std::cout << " find_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanPairs).count());
#endif

//        DLOG(INFO) << "Pairs found: " << pair_count;
//        DLOG(INFO) << "Pairs are " << util::blocks_to_string(pairs);

#ifdef BENCH
        const auto startTimeCopy = std::chrono::system_clock::now();
#endif
        std::vector<std::pair<variable_t, variable_t>> sort_pairs(pairs.size());

        auto iter = pairs.begin();

        for (size_t i = 0; i < pairs.size(); ++i) {
//            DLOG(INFO) << "Adding pair (" << (*iter).first.first << "," << (*iter).first.second << ") at index " << i;
            sort_pairs[i] = (*iter).first;
            ++iter;
        }
#ifdef BENCH
        const auto endTimeCopy = std::chrono::system_clock::now();
        const auto timeSpanCopy = endTimeCopy - startTimeCopy;
        std::cout << " copy_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCopy).count());
#endif

#ifdef BENCH
        const auto startTimeSort = std::chrono::system_clock::now();
#endif
//        lsd_radix_sort(sort_pairs);
        ips4o::sort(sort_pairs.begin(), sort_pairs.end());
#ifdef BENCH
        const auto endTimeSort = std::chrono::system_clock::now();
        const auto timeSpanSort = endTimeSort - startTimeSort;
        std::cout << " sort="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanSort).count())
                  << " elements=" << sort_pairs.size() << " pairs=" << pair_count;
#endif

//        DLOG(INFO) << "Sorted pairs are " << util::vector_blocks_to_string(sort_pairs);

#ifdef BENCH
        const auto startTimeAss = std::chrono::system_clock::now();
#endif
        pair_count = sort_pairs.size();
        auto nt_count = rlslp.non_terminals.size();
        rlslp.reserve(nt_count + pair_count);
        rlslp.resize(nt_count + pair_count);

        variable_t next_nt = rlslp.terminals + static_cast<variable_t>(nt_count);

        for (size_t i = 0; i < sort_pairs.size(); ++i) {
//            DLOG(INFO) << "Adding production rule " << next_nt + i << " -> (" << sort_pairs[i].first << ","
//                       << sort_pairs[i].second << ") at index " << nt_count + i;
            pairs[sort_pairs[i]] = next_nt + static_cast<variable_t>(i);
            size_t len = 0;
            if (sort_pairs[i].first >= static_cast<variable_t>(rlslp.terminals)) {
                len = rlslp[sort_pairs[i].first - rlslp.terminals].len;
            } else {
                len = 1;
            }
            if (sort_pairs[i].second >= static_cast<variable_t>(rlslp.terminals)) {
                len += rlslp[sort_pairs[i].second - rlslp.terminals].len;
            } else {
                len += 1;
            }
            rlslp[nt_count + i] = recomp::rlslp<>::non_terminal(sort_pairs[i].first, sort_pairs[i].second, len);
        }
#ifdef BENCH
        const auto endTimeAss = std::chrono::system_clock::now();
        const auto timeSpanAss = endTimeAss - startTimeAss;
        std::cout << " pair_rules="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count());
#endif
//        DLOG(INFO) << "Time for pair nts: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanAss).count() << "[ms]";


#ifdef BENCH
        const auto startTimeRep = std::chrono::system_clock::now();
#endif
//        DLOG(INFO) << "Positions are " << util::pair_positions_to_string(positions);

        for (size_t i = 0; i < positions.size(); ++i) {
            auto pos = positions[i];
            auto pair = std::make_pair(text[pos], text[pos + 1]);
            text[pos] = pairs[pair];

            text[pos + 1] = DELETED;
        }
#ifdef BENCH
        const auto endTimeRep = std::chrono::system_clock::now();
        const auto timeSpanRep = endTimeRep - startTimeRep;
        std::cout << " replace_pairs="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count());
#endif
//        DLOG(INFO) << "Time for replacing pairs: "
//                   << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanRep).count() << "[ms]";

#ifdef BENCH
        const auto startTimeCompact = std::chrono::system_clock::now();
#endif
        size_t new_text_size = text.size() - positions.size();
        if (new_text_size > 1 && pair_count > 0) {
            size_t copy_i = positions[0] + 1;
            size_t i = positions[0] + 2;  // jump to first position to copy

            for (; i < text.size(); ++i) {
                if (text[i] != DELETED) {
                    text[copy_i++] = text[i];
                }
            }
        }
#ifdef BENCH
        const auto endTimeCompact = std::chrono::system_clock::now();
        const auto timeSpanCompact = endTimeCompact - startTimeCompact;
        std::cout << " compact_text="
                  << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpanCompact).count());
#endif

//        DLOG(INFO) << "Shrinking text by " << positions.size() << " to length " << new_text_size;

        text.resize(new_text_size);
        text.shrink_to_fit();

#ifdef BENCH
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << " time="
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
                  << " compressed_text=" << text.size() << std::endl;
#endif
//        DLOG(INFO) << "Time for pcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
//                   << "[ms]";
//        std::cout << " comp_text=" << text.size();
//        DLOG(INFO) << "PComp ouput - text size: " << text.size() << " - distinct pairs: " << pair_count
//                   << " - string length reduce by: " << positions.size();
    }
};

}  // namespace recomp
