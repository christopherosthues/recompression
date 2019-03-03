#pragma once

#include <queue>
#include <thread>

#include "defs.hpp"
#include "rlslp.hpp"

namespace recomp {

/**
 * @brief
 *
 * @tparam variable_t
 * @tparam terminal_count_t
 * @param rlslp
 */
template<typename variable_t = recomp::var_t, typename terminal_count_t = recomp::term_t>
void sort_rlslp_rules(rlslp<variable_t, terminal_count_t>& rlslp, size_t cores = std::thread::hardware_concurrency()) {
    std::vector<variable_t> first(rlslp.size() + rlslp.terminals);
    std::vector<variable_t> next(rlslp.size() + rlslp.terminals);

#pragma omp parallel for num_threads(cores) schedule(static)
    for (size_t i = 0; i < first.size(); ++i) {
        first[i] = i;
        next[i] = i;
    }

    variable_t size = rlslp.size() - 1;
    for (size_t i = 0; i < rlslp.size(); ++i) {
        variable_t j = size - i;
        variable_t variable = j + rlslp.terminals;
        variable_t left = rlslp[j].first();

        if (first[left] == left) {
            first[left] = variable;
        } else {
            next[variable] = first[left];
            first[left] = variable;
        }
    }

    std::queue<variable_t> queue;
    for (size_t i = 0; i < rlslp.terminals; ++i) {
        if (first[i] != i) {
            queue.push(i);
        }
    }

    size_t new_index = rlslp.terminals;
    std::vector<variable_t> renamed(rlslp.size());
    std::queue<variable_t> block_queue;
    while (!queue.empty()) {
        auto var = queue.front();
        queue.pop();

        if (first[var] != var) {
            auto child = first[var];
            queue.push(child);
            while (next[child] != child) {
                child = next[child];
                queue.push(child);
            }
        }

        if (var >= rlslp.terminals) {
            renamed[var - rlslp.terminals] = new_index++ - rlslp.terminals;
        }
    }
}

}  // namespace recomp
