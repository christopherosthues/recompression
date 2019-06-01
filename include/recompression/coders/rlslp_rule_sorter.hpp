#pragma once

#include <iostream>
#include <queue>
#include <stack>
#include <thread>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"

namespace recomp {

/**
 * @brief Computes a permutation and renames the variables that the first symbols of each pair is sorted.
 *
 * @tparam variable_t The type of non-terminals
 * @param rlslp The rlslp to permute and rename
 * @param cores The number of cores to use
 */
template<typename variable_t = recomp::var_t>
inline void sort_rlslp_rules(rlslp<variable_t>& rlslp, size_t cores = std::thread::hardware_concurrency()) {
    ui_vector<variable_t> first(rlslp.size() + rlslp.terminals);
    ui_vector<variable_t> next(rlslp.size() + rlslp.terminals);

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

    size_t new_index = 0;
    std::vector<variable_t> renamed(rlslp.size());
    std::stack<variable_t> stack;
    std::stack<variable_t> block_stack;
    // Rename variable until a block is found on the path
    while (!queue.empty()) {
        auto var = queue.front();
        queue.pop();

        if (first[var] != var) {
            auto child = first[var];
            if (rlslp.is_block(child)) {
                stack.push(child);
                block_stack.push(child);
            } else {
                queue.push(child);
            }
            while (next[child] != child) {
                child = next[child];
                if (rlslp.is_block(child)) {
                    stack.push(child);
                    block_stack.push(child);
                } else {
                    queue.push(child);
                }
            }
        }

        if (var >= rlslp.terminals) {
            renamed[var - rlslp.terminals] = new_index++;
        }
    }
    std::queue<variable_t> block_queue;
    while (!stack.empty()) {
        auto var = stack.top();
        stack.pop();
        block_queue.push(var);
    }

    std::stack<variable_t> tmp_block_stack;
    std::stack<variable_t> tmp_stack;
    while (!block_queue.empty()) {
        auto var = block_queue.front();
        block_queue.pop();

        if (first[var] != var) {
            auto child = first[var];
            if (rlslp.is_block(child)) {
                tmp_block_stack.push(child);
            }
            tmp_stack.push(child);
            while (next[child] != child) {
                child = next[child];
                if (rlslp.is_block(child)) {
                    tmp_block_stack.push(child);
                }
                tmp_stack.push(child);
            }
            while (!tmp_block_stack.empty()) {
                stack.push(tmp_block_stack.top());
                tmp_block_stack.pop();
            }
            while (!tmp_stack.empty()) {
                block_queue.push(tmp_stack.top());
                tmp_stack.pop();
            }
        }
    }

    while (!stack.empty()) {
        block_stack.push(stack.top());
        stack.pop();
    }

    auto block_idx = rlslp.size() - 1;
    while (!block_stack.empty()) {
        auto var = block_stack.top();
        block_queue.push(var);
        block_stack.pop();
        renamed[var - rlslp.terminals] = block_idx--;
    }

    new_index = rlslp.blocks - 1;

    while (!block_queue.empty()) {
        auto var = block_queue.front();
        block_queue.pop();
        if (first[var] != var) {
            auto child = first[var];
            if (!rlslp.is_block(child)) {
                block_stack.push(child);
            }
            while (next[child] != child) {
                child = next[child];
                if (!rlslp.is_block(child)) {
                    block_stack.push(child);
                }
            }
            while (!block_stack.empty()) {
                block_queue.push(block_stack.top());
                block_stack.pop();
            }
        }

        if (var >= rlslp.terminals && !rlslp.is_block(var)) {
            renamed[var - rlslp.terminals] = new_index--;
        }
    }

    ui_vector<recomp::non_terminal<variable_t>> renamed_rules = std::move(rlslp.non_terminals);
    rlslp.non_terminals.resize(renamed_rules.size());
    for (size_t i = 0; i < renamed_rules.size(); ++i) {
        auto rule = renamed_rules[i];
        if (!rlslp.is_block(i + rlslp.terminals) && !rlslp.is_terminal(rule.second())) {
            rule.second() = renamed[rule.second() - rlslp.terminals] + rlslp.terminals;
        }
        if (!rlslp.is_terminal(rule.first())) {
            rule.first() = renamed[rule.first() - rlslp.terminals] + rlslp.terminals;
        }
        rlslp[renamed[i]] = rule;
    }

    if (rlslp.root >= rlslp.terminals && !rlslp.empty()) {
        rlslp.root = renamed[rlslp.root - rlslp.terminals] + rlslp.terminals;
    }
}

}  // namespace recomp
