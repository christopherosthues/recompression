
#pragma once

#include <stack>
#include <vector>

#include "rlslp.hpp"

namespace recomp {
namespace lce_query {

/**
 * @brief
 *
 * @tparam variable_t
 */
template<typename variable_t = var_t>
struct vis_node {
    variable_t nt;
    size_t pos;
//    size_t n_trav;

    vis_node(variable_t nt, size_t pos/*, size_t n_trav*/) {
        this->nt = nt;
        this->pos = pos;
//        this->n_trav = n_trav;
    }
};

/**
 * @brief
 *
 * @tparam variable_t
 */
template<typename variable_t = var_t>
struct lceq {
    std::vector<size_t> visited;  // TODO(Chris): replace with bitvector. not important anymore
    std::stack<vis_node<variable_t>> stack;

    lceq(size_t size) : visited(size), stack() {}
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
size_t find_next(rlslp<variable_t, terminal_count_t>& rlslp, size_t i, size_t j, variable_t nt_i, variable_t nt_j, lceq<variable_t>& i_visited, lceq<variable_t>& j_visited, size_t traverse) {
    // TODO(Chris): find next variable, traverse possibly up and down again
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
inline void subtree(rlslp<variable_t, terminal_count_t>& rlslp, size_t& pos, variable_t& nt) {
    if (nt >= rlslp.terminals) {
        auto nt_index = nt - rlslp.terminals;  // compute index of production
        auto left_child = rlslp[nt_index].first();
        size_t len = 1;
        if (left_child >= rlslp.terminals) {
            len = rlslp[left_child - rlslp.terminals].len;
        }
        if (rlslp[nt_index].second() < 0) {  // if production derives a block
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
            if (child_i >= rlslp.terminals) {
                i_visited.visited[child_i - rlslp.terminals] = traverse;
            }
            i_visited.stack.push(child_i, pos_i);//, traverse);
        }
        if (nt_j != child_j) {
            if (child_j >= rlslp.terminals) {
                j_visited.visited[child_j - rlslp.terminals] = traverse;
            }
            j_visited.stack.push(child_j, pos_j);//, traverse);
        }

        // TODO(Chris): lce queries
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

    lceq<variable_t> i_visited(rlslp.size());
    lceq<variable_t> j_visited(rlslp.size());
    i_visited.stack.push(rlslp.root + rlslp.terminals, i);//, 1);
    j_visited.stack.push(rlslp.root + rlslp.terminals, j);//, 1);
    i_visited.visited[rlslp.root] = 1;
    j_visited.visited[rlslp.root] = 1;

    return lce_query(rlslp, i, j, rlslp.root, rlslp.root, i_visited, j_visited, 1);
}

}  // namespace lce_query
}  // namespace recomp
