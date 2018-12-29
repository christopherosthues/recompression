
#pragma once

#include <omp.h>

#include <stack>
#include <unordered_map>
#include <vector>

#include <glog/logging.h>

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
size_t find_next(const rlslp<variable_t, terminal_count_t>& rlslp,
                 size_t i,
                 size_t j,
                 variable_t nt_i,
                 variable_t nt_j,
                 lceq<variable_t>& i_visited,
                 lceq<variable_t>& j_visited,
                 size_t len,
//                 variable_t eq,
                 size_t traverse) {
    auto parent_i = nt_i;
    auto comp_i_len = len;// rlslp.len(nt_i) - i; // TODO(Chris): still not correct!!!
    i += comp_i_len;
//    while (parent_i < eq) {
//        i_visited.visited.erase(parent_i);
//        parent_i = i_visited.parents.top();
//        i_visited.parents.pop();
//        i = i_visited.visited[parent_i].pos;
//    }
    while (i >= rlslp.len(parent_i) && parent_i != rlslp.root + rlslp.terminals) {
        DLOG(INFO) << "Traverse up for i: " << parent_i << ", " << i << ", " << comp_i_len;
        i_visited.visited.erase(parent_i);
//        comp_i_len = rlslp.len(parent_i) - i;// - i;  // TODO(Chris): check correctness
        auto child_pos = i;
        auto child = parent_i;
        parent_i = i_visited.parents.top();
        i_visited.parents.pop();
        i = i_visited.visited[parent_i].pos;
        if (i_visited.visited[parent_i].n_trav < traverse - 1) {
            DLOG(INFO) << "Variable of previous traverse: " << i_visited.visited[parent_i].n_trav << ", " << traverse;
            size_t p_i = i;
            variable_t child_i = parent_i;
            subtree(rlslp, p_i, child_i);
            //comp_i_len = rlslp.len(child) + child_pos + len;
            comp_i_len = /*len + */child_pos - p_i; // TODO
//            i_visited.visited[parent_i].pos = comp_i_len;
            DLOG(INFO) << "Values i: pos_old: " << p_i << ", actual_pos: " << child_pos << ", len: " << len << ", comp_len: " << comp_i_len;
        } else {
            comp_i_len = len;
        }
//        i_visited.visited[parent_i].pos = i + comp_i_len;
        i += comp_i_len;
    }
    i_visited.visited[parent_i].pos = i;
    i_visited.visited[parent_i].n_trav = traverse;
    DLOG(INFO) << "i: " << parent_i << ", " << i << ", " << comp_i_len;

    auto parent_j = nt_j;
    auto comp_j_len = len;//rlslp.len(nt_j) - j;
    j += comp_j_len;
//    while (parent_j < eq) {
//        j_visited.visited.erase(parent_j);
//        parent_j = j_visited.parents.top();
//        j_visited.parents.pop();
//        j = j_visited.visited[parent_j].pos;
//    }
    while (j >= rlslp.len(parent_j) && parent_j != rlslp.root + rlslp.terminals) {
        DLOG(INFO) << "Traverse up for j: " << parent_j << ", " << j << ", " << comp_j_len;
        j_visited.visited.erase(parent_j);
//        if (j_visited.visited[])
//        comp_j_len = rlslp.len(parent_j) - j;// - j;  // TODO(Chris): check correctness
        auto child_pos = j;
        auto child = parent_j;
        parent_j = j_visited.parents.top();
        j_visited.parents.pop();
        j = j_visited.visited[parent_j].pos;
        if (j_visited.visited[parent_j].n_trav < traverse - 1) {
            DLOG(INFO) << "Variable of previous traverse: " << j_visited.visited[parent_j].n_trav << ", " << traverse;
//            comp_j_len = rlslp.len(child) + child_pos + len;
            size_t p_j = j;
            variable_t child_j = parent_j;
            subtree(rlslp, p_j, child_j);
            comp_j_len = /*len + */child_pos - p_j; // TODO
//            j_visited.visited[parent_j].pos = comp_j_len;
            DLOG(INFO) << "Values j: pos_old: " << p_j << ", actual_pos: " << child_pos << ", len: " << len << ", comp_len: " << comp_j_len;
        } else {
            comp_j_len = len;
        }
//        j_visited.visited[parent_j].pos = j + comp_j_len;
        j += comp_j_len;
    }
    j_visited.visited[parent_j].pos = j;
    j_visited.visited[parent_j].n_trav = traverse;
    DLOG(INFO) << "j: " << parent_j << ", " << j << ", " << comp_j_len;

    if ((parent_i == rlslp.root + rlslp.terminals && i >= rlslp.len(parent_i)) ||
        (parent_j == rlslp.root + rlslp.terminals && j >= rlslp.len(parent_j))) {
        return 0;
    } else {
        return lce_query(rlslp, i, j, parent_i, parent_j, i_visited, j_visited, traverse);
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
    DLOG(INFO) << "Comparing " << nt_i << ", " << i << " with " << nt_j << ", " << j;

    if (nt_i == nt_j && i == j) {
        DLOG(INFO) << "same level: " << nt_i << " at pos " << i << " and " << nt_j << " at pos " << j;
        size_t len = rlslp[nt_i].len;
        return len + find_next(rlslp, i, j, nt_i, nt_j, i_visited, j_visited, len, traverse + 1);
    } else {
        variable_t child_i = nt_i;
        variable_t child_j = nt_j;
        size_t pos_i = i;
        size_t pos_j = j;

        subtree(rlslp, pos_i, child_i);
        subtree(rlslp, pos_j, child_j);

        DLOG(INFO) << "Child of nt_i: " << child_i << " at pos " << pos_i;
        DLOG(INFO) << "Child of nt_j: " << child_j << " at pos " << pos_j;

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

        auto found_j = i_visited.visited.find(child_j);
        auto found_i = j_visited.visited.find(child_i);
        if (rlslp.is_block(child_i) && rlslp.is_block(child_j) && rlslp[child_i - rlslp.terminals].first() == rlslp[child_j - rlslp.terminals].first() && pos_i % rlslp.len(rlslp[child_i - rlslp.terminals].first()) == pos_j % rlslp.len(rlslp[child_j - rlslp.terminals].first())) {  // access of len is save because terminals cannot derive blocks
            DLOG(INFO) << "Blocks of same symbol found." << child_i << " at pos " << pos_i << " and " << child_j << " at pos " << pos_j
                       << " of symbol " << rlslp[child_i - rlslp.terminals].first();
            size_t len = std::min(rlslp.len(child_i) - pos_i, rlslp.len(child_j) - pos_j);
            return len + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, len, traverse + 1);
        } else if (found_i != j_visited.visited.end() && (*found_i).second.pos == pos_i && (*found_i).second.n_trav == traverse) {
            // TODO(Chris): check if j_visited[child_i] exists
            DLOG(INFO) << "Found same non-terminal at different level for nt_i: " << child_i << " at pos " << pos_i;
            size_t len = rlslp.len(child_i) - pos_i;
            return len + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, len, traverse + 1);
        } else if (found_j != j_visited.visited.end() && (*found_j).second.pos == pos_j && (*found_j).second.n_trav == traverse) {
            // TODO(Chris): check if i_visited[child_j] exists
            DLOG(INFO) << "Found same non-terminal at different level for nt_j: " << child_j << " at pos " << pos_j;
            size_t len = rlslp.len(child_j) - pos_j;
            return len + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, len, traverse + 1);
        } else if (child_i < rlslp.terminals && child_j < rlslp.terminals) {
            DLOG(INFO) << "Both are terminals";
            if (child_i == child_j) {
                DLOG(INFO) << "Matching terminals found: " << child_i << " == " << child_j;
                return 1 + find_next(rlslp, pos_i, pos_j, child_i, child_j, i_visited, j_visited, 1, traverse);
//                return 1 + find_next(rlslp, i, j, nt_i, nt_j, i_visited, j_visited, traverse);
            } else {
                DLOG(INFO) << "Missmatch of " << child_i << " and " << child_j;
                return 0;
            }
        } else {
            DLOG(INFO) << "No matching non-terminals found. Traverse down further";
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
        DLOG(INFO) << "Empty rlslp";
        return 0;
    }
    if (rlslp[rlslp.root].len <= i || rlslp[rlslp.root].len <= j) {
        DLOG(INFO) << "Position out of bounds";
        return 0;
    }
    if (i == j) {
        DLOG(INFO) << "Equal position";
        return rlslp[rlslp.root].len - i;
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
