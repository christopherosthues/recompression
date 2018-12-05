
#pragma once

#include <array>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "rlslp.hpp"

namespace recomp {

typedef std::vector<variable_t> text_t;
typedef std::vector<variable_t> alphabet_t;
typedef std::array<std::vector<std::pair<std::pair<variable_t, variable_t>, size_t>>, 2> multiset_t;
typedef std::unordered_map<variable_t, bool> partition_t;

struct pair_hash {
    template<typename P1, typename P2>
    size_t operator()(const std::pair<P1, P2>& pair) const {
        std::hash<P1> h1;  // (pair.first);
        std::hash<P2> h2;  // (pair.second);
        return h1(pair.first) ^ (h2(pair.second) << 1);
    }
};

/**
 * @brief Replaces all block in the text with new non-terminals.
 *
 * @param text The text
 * @param rlslp The rlslp
 */
void bcomp(text_t& text, rlslp& rlslp);

/**
 * @brief Replaces all pairs in the text based on a partition of the symbols with new non-terminals.
 *
 * @param text The text
 * @param rlslp The rlslp
 */
void pcomp(text_t& text, rlslp& rlslp);

/**
 * @brief
 *
 * @param text The text
 * @param multiset The multiset
 */
inline void compute_multiset(const text_t& text, multiset_t& multiset);

/**
 * @brief
 *
 * @param multiset
 * @param alphabet
 * @param partition
 */
inline void compute_partition(const multiset_t& multiset, const alphabet_t& alphabet, partition_t& partition);

/**
 * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
 *
 * @param text The text
 * @param rlslp The rlslp
 */
void recomp(text_t& text, rlslp& rlslp);

/**
 * @brief Builds a context free grammar in Chomsky normal form using the recompression technique.
 *
 * @param text The text
 * @param rlslp The rlslp
 * @param alphabet_size The size of the alphabet (minimum biggest symbol used in the text)
 */
void recomp(text_t& text, rlslp& rlslp, const terminal_count_t& alphabet_size);

}  // namespace recomp
