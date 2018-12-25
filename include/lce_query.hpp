
#pragma once

#include <omp.h>

#include <stack>
#include <unordered_map>
#include <vector>

#include "rlslp.hpp"

namespace recomp {
namespace lce_query {

/**
 * @brief
 *
 * @tparam variable_t
 */
//template<typename variable_t = var_t>
struct vis_node {
//    variable_t nt;
    size_t pos;
    size_t n_trav;

    vis_node() = default;
//    vis_node(/*variable_t nt, */size_t pos, size_t n_trav) {
////        this->nt = nt;
//        this->pos = pos;
//        this->n_trav = n_trav;
//    }
    vis_node(size_t pos, size_t n_trav) {
        this->pos = pos;
        this->n_trav = n_trav;
    }
};

/**
 * @brief
 *
 * @tparam variable_t
 */
template<typename variable_t = var_t>
struct lceq {
    /**
     * Maps a non-terminal to its parent, the position in the parent and the number of the traverse.
     */
//    std::unordered_map<variable_t, vis_node<variable_t>> visited;
    std::unordered_map<variable_t, vis_node> visited;
    std::stack<variable_t> parents;
//    std::stack<vis_node<variable_t>> stack;

    lceq() : visited(), parents() {}//: visited(size, 0) {}//, stack() {}
};

template<typename variable_t = var_t, typename terminal_count_t = term_t>
size_t lce_query(const rlslp<variable_t, terminal_count_t>& rlslp, size_t i, size_t j, variable_t nt_i, variable_t nt_j, lceq<variable_t>& i_visited, lceq<variable_t>& j_visited, size_t traverse);

/**
 * @brief
 *
 * @tparam variable_t
 * @tparam terminal_count_t
 * @param rlslp
 * @param i
 * @param j
 * @param nt_i
 * @param nt_j
 * @param i_visited
 * @param j_visited
 * @param traverse
 * @return
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
size_t find_next(const rlslp<variable_t, terminal_count_t>& rlslp, size_t i, size_t j, variable_t nt_i, variable_t nt_j, lceq<variable_t>& i_visited, lceq<variable_t>& j_visited, size_t traverse) {
    auto parent_i = nt_i;
    auto comp_i_len = rlslp.len(nt_i) - i;
    while (i + comp_i_len >= rlslp.len(parent_i) && parent_i != rlslp.root + rlslp.terminals) {
        i_visited.visited.erase(parent_i);
        comp_i_len = rlslp.len(parent_i) - i;  // TODO(Chris): check correctness
        parent_i = i_visited.parents.top();
        i_visited.parents.pop();
        i = i_visited.visited[parent_i].pos;
    }
    i_visited.visited[parent_i].pos = i + comp_i_len;
    i_visited.visited[parent_i].n_trav = traverse;

    auto parent_j = nt_j;
    auto comp_j_len = rlslp.len(nt_j) - j;
    while (j + comp_j_len >= rlslp.len(parent_j) && parent_j != rlslp.root + rlslp.terminals) {
        j_visited.visited.erase(parent_j);
        comp_j_len = rlslp.len(parent_j) - j;  // TODO(Chris): check correctness
        parent_j = j_visited.parents.top();
        j_visited.parents.pop();
        j = j_visited.visited[parent_j].pos;
    }
    j_visited.visited[parent_j].pos = j + comp_j_len;
    j_visited.visited[parent_j].n_trav = traverse;

    if ((parent_i == rlslp.root + rlslp.terminals && i + comp_i_len >= rlslp.len(parent_i)) || (parent_j == rlslp.root + rlslp.terminals && j + comp_j_len >= rlslp.len(parent_j))) {
        return 0;
    } else {
        return lce_query(rlslp, i + comp_i_len, j + comp_j_len, parent_i, parent_j, i_visited, j_visited, traverse);
    }
}

/**
 * @brief
 *
 * @tparam variable_t
 * @tparam terminal_count_t
 * @param rlslp
 * @param pos
 * @param nt
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
inline void subtree(const rlslp<variable_t, terminal_count_t>& rlslp, size_t& pos, variable_t& nt) {
    if (nt >= rlslp.terminals) {
        auto nt_index = nt - rlslp.terminals;  // compute index of production
        auto left_child = rlslp[nt_index].first();
//        size_t len = 1;
//        if (left_child >= rlslp.terminals) {
//            len = rlslp[left_child - rlslp.terminals].len;
//        }
        auto len = rlslp.len(left_child);
//        if (rlslp[nt_index].second() < 0) {  // if production derives a block
        if (rlslp.is_block(nt)) {  // if production derives a block
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
 * @brief
 *
 * @tparam variable_t
 * @tparam terminal_count_t
 * @param rlslp
 * @param i
 * @param j
 * @param nt_i
 * @param nt_j
 * @param i_visited
 * @param j_visited
 * @param traverse
 * @return
 */
template<typename variable_t, typename terminal_count_t>
size_t lce_query(const rlslp<variable_t, terminal_count_t>& rlslp, size_t i, size_t j, variable_t nt_i, variable_t nt_j, lceq<variable_t>& i_visited, lceq<variable_t>& j_visited, size_t traverse) {

    if (nt_i == nt_j && i == j) {
        return rlslp[nt_i].len + find_next(rlslp, i, j, nt_i, nt_j, i_visited, j_visited, traverse + 1);
    } else {
        variable_t child_i = nt_i;
        variable_t child_j = nt_j;
        size_t pos_i = i;
        size_t pos_j = j;

        subtree(rlslp, pos_i, child_i);
        subtree(rlslp, pos_j, child_j);

        if (nt_i != child_i) {
            i_visited.visited[child_i] = vis_node{/*nt_i, */pos_i, traverse};
//            i_visited.visited.insert(child_i, pos_i, traverse);
            i_visited.parents.push(nt_i);
//            if (child_i >= rlslp.terminals) {
//                i_visited.visited[child_i - rlslp.terminals] = traverse;
//            }
//            i_visited.stack.push(child_i, pos_i);//, traverse);
        }
        if (nt_j != child_j) {
            j_visited.visited[child_j] = vis_node{/*nt_j, */pos_j, traverse};
//            j_visited.visited.insert(child_j, pos_j, traverse);//[child_j] = vis_node<variable_t>(/*nt_j, */pos_j, traverse);
            j_visited.parents.push(nt_j);
//            if (child_j >= rlslp.terminals) {
//                j_visited.visited[child_j - rlslp.terminals] = traverse;
//            }
//            j_visited.stack.push(child_j, pos_j);//, traverse);
        }

        // TODO(Chris): lce queries
        auto found_j = i_visited.visited.find(child_j);
        auto found_i = j_visited.visited.find(child_i);
        if (rlslp.is_block(child_i) && rlslp.is_block(child_j) && rlslp[child_i - rlslp.terminals].first() == rlslp[child_j - rlslp.terminals].first() && pos_i % rlslp.len(rlslp[child_i - rlslp.terminals].first()) == pos_j % rlslp.len(rlslp[child_j - rlslp.terminals].first())) {  // access of len is save because terminals cannot derive blocks
            return std::min(rlslp.len(child_i) - pos_i, rlslp.len(child_j) - pos_j) + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, traverse + 1);
        } else if (found_i != j_visited.visited.end() && (*found_i).second.pos == pos_i && (*found_i).second.n_trav == traverse) {
            // TODO(Chris): check if j_visited[child_i] exists
            return rlslp.len(child_i) - pos_i + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, traverse + 1);
        } else if (found_j != j_visited.visited.end() && (*found_j).second.pos == pos_j && (*found_j).second.n_trav == traverse) {
            // TODO(Chris): check if i_visited[child_j] exists
            return rlslp.len(child_j) - pos_j + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, traverse + 1);
        } else if (child_i < rlslp.terminals && child_j < rlslp.terminals) {
            if (child_i == child_j) {
                return 1 + find_next(rlslp, i, j, nt_i, nt_j, i_visited, j_visited, traverse);
            } else {
                return 0;
            }
        } else {
            return lce_query(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, traverse);
        }
    }
}


/**
 * @brief
 *
 * @tparam variable_t
 * @tparam terminal_count_t
 * @param rlslp
 * @param i
 * @param j
 * @return
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
size_t lce_query(const rlslp<variable_t, terminal_count_t>& rlslp, size_t i, size_t j) {
    if (rlslp.empty()) {
        return 0;
    }
    if (rlslp[rlslp.root].len <= i || rlslp[rlslp.root].len <= j) {
        return 0;
    }
    if (i == j) {
        return rlslp[rlslp.root].len;
    }

    lceq<variable_t> i_visited;
    lceq<variable_t> j_visited;
//    lceq<variable_t> i_visited(rlslp.size());
//    lceq<variable_t> j_visited(rlslp.size());
//    i_visited.stack.push(rlslp.root + rlslp.terminals, i);//, 1);
//    j_visited.stack.push(rlslp.root + rlslp.terminals, j);//, 1);
//    i_visited.visited[rlslp.root] = 1;
//    j_visited.visited[rlslp.root] = 1;

    return lce_query<variable_t, terminal_count_t>(rlslp, i, j, static_cast<variable_t>(rlslp.root + rlslp.terminals),
                     static_cast<variable_t>(rlslp.root + rlslp.terminals), i_visited, j_visited, 1);
}

}  // namespace lce_query
}  // namespace recomp
