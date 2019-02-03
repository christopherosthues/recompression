
#pragma once

#include <omp.h>

#include <stack>
#include <unordered_map>
#include <vector>

#include "rlslp.hpp"

namespace recomp {
namespace lce_query {

/**
 * @brief Struct storing the position of a variable and the number of the traverse.
 *
 */
struct vis_node {
    size_t pos;
    size_t n_trav;

    vis_node() = default;

    vis_node(size_t pos, size_t n_trav) {
        this->pos = pos;
        this->n_trav = n_trav;
    }
};

/**
 * @brief Struct storing data during a lce query like a stack of the visited ancestors.
 *
 * Besides the ancestors of the visited nodes data about the visited nodes itself are also store like the position
 * to look for in the derived substring by the variable and the number of the variables to compare (called traverse).
 *
 * @tparam variable_t The type of variable
 */
template<typename variable_t = var_t>
struct lceq {
    /**
     * Maps a non-terminal to the position of the non-terminal and the number of the traverse during the traverse.
     */
    std::unordered_map<variable_t, vis_node> visited;

    /**
     * Stack storing all ancestor variables passed.
     */
    std::stack<variable_t> parents;

    lceq() : visited(), parents() {}
};

template<typename variable_t = var_t, typename terminal_count_t = term_t>
size_t lce_query(const rlslp<variable_t, terminal_count_t>& rlslp, size_t i, size_t j, variable_t nt_i, variable_t nt_j,
                 lceq<variable_t>& i_visited, lceq<variable_t>& j_visited, size_t traverse);

/**
 * @brief Computes the subtree to be traverse downwards from the given variable.
 *
 * @tparam variable_t The type of variables
 * @tparam terminal_count_t The type of terminals
 * @param[in] rlslp The rlslp
 * @param[in,out] pos The position inside the variable to be looked for
 * @param[in,out] nt The variable to be compared
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
inline void subtree(const rlslp<variable_t, terminal_count_t>& rlslp, size_t& pos, variable_t& nt) {
    if (nt >= rlslp.terminals) {
        auto nt_index = nt - rlslp.terminals;  // compute index of production
        auto left_child = rlslp[nt_index].first();
        auto len = rlslp.len(left_child);
        if (rlslp.is_block(nt)) {  // if variable derives a block
            auto block_len = 0;
            while (block_len + len <= pos) {
                block_len += len;
            }
            pos -= block_len;
            nt = rlslp[nt_index].first();
        } else {
            if (pos < len) {  // follow left child
                nt = left_child;
            } else {  // follow right child
                pos -= len;
                nt = rlslp[nt_index].second();
            }
        }
    }
}

/**
 * @brief Finds the next node in the derivation tree to compare its children by possibly traversing upwards the tree.
 *
 * @tparam variable_t The type of variables
 * @tparam terminal_count_t The type of terminals
 * @param rlslp The rlslp
 * @param i The position inside the last visited variable
 * @param j The position inside the last visited variable
 * @param nt_i The last visited variable of the first suffix
 * @param nt_j The last visited variable of the second suffix
 * @param i_visited The visited variables of the first suffix
 * @param j_visited The visited variable of the second suffix
 * @param len The length of the last compared variables
 * @param traverse The number of the next downward traverse
 * @return The length of the longest common prefix of the two given suffices
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
size_t find_next(const rlslp<variable_t, terminal_count_t>& rlslp,
                 size_t i,
                 size_t j,
                 variable_t nt_i,
                 variable_t nt_j,
                 lceq<variable_t>& i_visited,
                 lceq<variable_t>& j_visited,
                 size_t len,
                 size_t traverse) {
    auto parent_i = nt_i;
    auto comp_i_len = len;
    i += comp_i_len;
    while (i >= rlslp.len(parent_i) && parent_i != rlslp.root + rlslp.terminals) {
//        DLOG(INFO) << "Traverse up for i: " << parent_i << ", " << i << ", " << comp_i_len;
        i_visited.visited.erase(parent_i);
        auto child_pos = i;
//        auto child = parent_i;
        parent_i = i_visited.parents.top();
        i_visited.parents.pop();
        i = i_visited.visited[parent_i].pos;
        if (i_visited.visited[parent_i].n_trav < traverse - 1) {
//            DLOG(INFO) << "Variable of previous traverse: " << i_visited.visited[parent_i].n_trav << ", " << traverse;
            size_t p_i = i;
            variable_t child_i = parent_i;
            subtree(rlslp, p_i, child_i);
            comp_i_len = child_pos - p_i;
//            DLOG(INFO) << "Values i: pos_old: " << p_i << ", actual_pos: " << child_pos << ", len: " << len
//                       << ", comp_len: " << comp_i_len;
        } else {
            comp_i_len = len;
        }
        i += comp_i_len;
    }
    i_visited.visited[parent_i].pos = i;
    i_visited.visited[parent_i].n_trav = traverse;
//    DLOG(INFO) << "i: " << parent_i << ", " << i << ", " << comp_i_len;

    auto parent_j = nt_j;
    auto comp_j_len = len;
    j += comp_j_len;
    while (j >= rlslp.len(parent_j) && parent_j != rlslp.root + rlslp.terminals) {
//        DLOG(INFO) << "Traverse up for j: " << parent_j << ", " << j << ", " << comp_j_len;
        j_visited.visited.erase(parent_j);
        auto child_pos = j;
//        auto child = parent_j;
        parent_j = j_visited.parents.top();
        j_visited.parents.pop();
        j = j_visited.visited[parent_j].pos;
        if (j_visited.visited[parent_j].n_trav < traverse - 1) {
//            DLOG(INFO) << "Variable of previous traverse: " << j_visited.visited[parent_j].n_trav << ", " << traverse;
            size_t p_j = j;
            variable_t child_j = parent_j;
            subtree(rlslp, p_j, child_j);
            comp_j_len = child_pos - p_j;
//            DLOG(INFO) << "Values j: pos_old: " << p_j << ", actual_pos: " << child_pos << ", len: " << len
//                       << ", comp_len: " << comp_j_len;
        } else {
            comp_j_len = len;
        }
        j += comp_j_len;
    }
    j_visited.visited[parent_j].pos = j;
    j_visited.visited[parent_j].n_trav = traverse;
//    DLOG(INFO) << "j: " << parent_j << ", " << j << ", " << comp_j_len;

    if ((parent_i == rlslp.root + rlslp.terminals && i >= rlslp.len(parent_i)) ||
        (parent_j == rlslp.root + rlslp.terminals && j >= rlslp.len(parent_j))) {
        return 0;
    } else {
        return lce_query(rlslp, i, j, parent_i, parent_j, i_visited, j_visited, traverse);
    }
}

/**
 * @brief Computes the length of the longest common prefix of the two given suffices (LCE).
 *
 * @tparam variable_t The type of variables
 * @tparam terminal_count_t The type of terminals
 * @param rlslp The rlslp
 * @param i The start position inside the subtree of the first variable to be compared
 * @param j The start position inside the subtree of the second variable to be compared
 * @param nt_i The actual variable of the first suffix
 * @param nt_j The actual variable of the second suffix
 * @param i_visited The visited variables of the first suffix
 * @param j_visited The visited variable of the second suffix
 * @param traverse The number of the traverse
 * @return The length of the longest common prefix of the two given suffices
 */
template<typename variable_t, typename terminal_count_t>
size_t lce_query(const rlslp <variable_t, terminal_count_t>& rlslp,
                 size_t i,
                 size_t j,
                 variable_t nt_i,
                 variable_t nt_j,
                 lceq<variable_t>& i_visited,
                 lceq<variable_t>& j_visited,
                 size_t traverse) {
//    DLOG(INFO) << "Comparing " << nt_i << ", " << i << " with " << nt_j << ", " << j;

    if (nt_i == nt_j && i == j) {  // Found two equal variables with the same position to be compared inside the subtree
//        DLOG(INFO) << "same level: " << nt_i << " at pos " << i << " and " << nt_j << " at pos " << j;
        size_t len = rlslp[nt_i].len;
        return len + find_next(rlslp, i, j, nt_i, nt_j, i_visited, j_visited, len, traverse + 1);
    } else {
        variable_t child_i = nt_i;
        variable_t child_j = nt_j;
        size_t pos_i = i;
        size_t pos_j = j;

        // Compute subtrees to be further traversed down
        subtree(rlslp, pos_i, child_i);
        subtree(rlslp, pos_j, child_j);

//        DLOG(INFO) << "Child of nt_i: " << child_i << " at pos " << pos_i;
//        DLOG(INFO) << "Child of nt_j: " << child_j << " at pos " << pos_j;

        if (nt_i != child_i) {
            i_visited.visited[child_i] = vis_node{pos_i, traverse};
            i_visited.parents.push(nt_i);
        }
        if (nt_j != child_j) {
            j_visited.visited[child_j] = vis_node{pos_j, traverse};
            j_visited.parents.push(nt_j);
        }

        auto found_j = i_visited.visited.find(child_j);
        auto found_i = j_visited.visited.find(child_i);
        if (rlslp.is_block(child_i) && rlslp.is_block(child_j) && rlslp[child_i - rlslp.terminals].first() == rlslp[child_j - rlslp.terminals].first() &&
            pos_i % rlslp.len(rlslp[child_i - rlslp.terminals].first()) == pos_j % rlslp.len(rlslp[child_j - rlslp.terminals].first())) {  // access of len is save because terminals cannot derive blocks
            // Found two blocks of same symbol and same position but possibly different lengths
            // => take minimum of the difference between the length of the derived substring and the position to be compared
//            DLOG(INFO) << "Blocks of same symbol found." << child_i << " at pos " << pos_i << " and " << child_j
//                       << " at pos " << pos_j
//                       << " of symbol " << rlslp[child_i - rlslp.terminals].first();
            size_t len = std::min(rlslp.len(child_i) - pos_i, rlslp.len(child_j) - pos_j);
            return len + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, len, traverse + 1);
        } else if (found_i != j_visited.visited.end() && (*found_i).second.pos == pos_i && (*found_i).second.n_trav == traverse) {
            // Found a variable for the first suffix that was traversed by the second before
//            DLOG(INFO) << "Found same non-terminal at different level for nt_i: " << child_i << " at pos " << pos_i;
            size_t len = rlslp.len(child_i) - pos_i;
            while (child_j != child_i) {
                child_j = j_visited.parents.top();
                j_visited.parents.pop();
            }
            pos_j = j_visited.visited[child_j].pos;
            return len + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, len, traverse + 1);
        } else if (found_j != j_visited.visited.end() && (*found_j).second.pos == pos_j && (*found_j).second.n_trav == traverse) {
            // Found a variable for the second suffix that was traversed by the first before
//            DLOG(INFO) << "Found same non-terminal at different level for nt_j: " << child_j << " at pos " << pos_j;
            size_t len = rlslp.len(child_j) - pos_j;
            while (child_i != child_j) {
                child_i = i_visited.parents.top();
                i_visited.parents.pop();
            }
            pos_i = i_visited.visited[child_i].pos;
            return len + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, len, traverse + 1);
        } else if (child_i < rlslp.terminals && child_j < rlslp.terminals) {
            // Reached two terminals
//            DLOG(INFO) << "Both are terminals";
            if (child_i == child_j) {
                // Same terminals, we are not finished so we have to traverse possibly upwards and downwards the tree again
                // to find the next variable to compare
//                DLOG(INFO) << "Matching terminals found: " << child_i << " == " << child_j;
                return 1 + find_next(rlslp, i, j, nt_i, nt_j, i_visited, j_visited, 1, traverse);
            } else {
                // Found two terminals that are a mismatch, we are finished
//                DLOG(INFO) << "Mismatch of " << child_i << " and " << child_j;
                return 0;
            }
        } else {
            // We possibly found only one terminal, we have to traverse downwards the tree further
//            DLOG(INFO) << "No matching non-terminals found. Traverse down further";
            return lce_query(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, traverse);
        }
    }
}


/**
 * @brief Computes the length of the longest common prefix of two given suffices (longest common extension, LCE).
 *
 * @tparam variable_t The type of variables
 * @tparam terminal_count_t The type of terminals
 * @param rlslp The rlslp
 * @param i The start position of the first suffix
 * @param j The start position of the second suffix
 * @return The length of the longest common prefix of the two given suffices
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
size_t lce_query(const rlslp <variable_t, terminal_count_t>& rlslp, size_t i, size_t j) {
    if (rlslp.empty()) {
//        DLOG(INFO) << "Empty rlslp";
        return 0;
    }
    if (rlslp[rlslp.root].len <= i || rlslp[rlslp.root].len <= j) {
//        DLOG(INFO) << "Position out of bounds";
        return 0;
    }
    if (i == j) {
//        DLOG(INFO) << "Equal position";
        return rlslp[rlslp.root].len - i;
    }

    lceq<variable_t> i_visited;
    lceq<variable_t> j_visited;

    return lce_query<variable_t, terminal_count_t>(rlslp, i, j, static_cast<variable_t>(rlslp.root + rlslp.terminals),
                                                   static_cast<variable_t>(rlslp.root + rlslp.terminals), i_visited,
                                                   j_visited, 1);
}


/**
 * @brief Naive computation of a longest common extension query for two suffices.
 *
 * @tparam text_t The type of text
 * @param text The text
 * @param i The start position of the first suffix
 * @param j The start position of the second suffix
 * @return The length of the longest common prefix of the two given suffices
 */
template<typename text_t>
size_t lce_query_naive(text_t& text, size_t i, size_t j) {
    if (text.size() <= i || text.size() <= j) {
        return 0;
    }

    if (i == j) {
        return text.size() - i;
    }

    size_t lce = 0;
    while (i < text.size() && j < text.size() && text[i++] == text[j++]) {
        lce++;
    }
    return lce;
}

}  // namespace lce_query
}  // namespace recomp
