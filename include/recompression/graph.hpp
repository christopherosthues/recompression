#pragma once

#include <cstddef>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "defs.hpp"

namespace recomp {

template<typename T>
class graph {
    std::unordered_map<T, std::vector<T>> edges;
    std::vector<T> nodes;

 public:
    inline graph(ui_vector<size_t>& adj_list, const ui_vector<var_t>& text) {
        for (size_t i = 0; i < adj_list.size(); ++i) {
            auto char_i = text[adj_list[i]];
            auto char_i1 = text[adj_list[i] + 1];
            if (edges.find(char_i) == edges.end()) {
                edges[char_i] = std::vector<T>{};
            }
            if (std::find(edges[char_i].begin(), edges[char_i].end(), char_i1) == edges[char_i].end()) {
                edges[char_i].push_back(char_i1);
            }
            if (edges.find(char_i1) == edges.end()) {
                edges[char_i1] = std::vector<T>{};
            }
            if (std::find(edges[char_i1].begin(), edges[char_i1].end(), char_i) == edges[char_i1].end()) {
                edges[char_i1].push_back(char_i);
            }
            if (std::find(nodes.begin(), nodes.end(), char_i) == nodes.end()) {
                nodes.push_back(char_i);
            }
            if (std::find(nodes.begin(), nodes.end(), char_i1) == nodes.end()) {
                nodes.push_back(char_i1);
            }
        }

        for (auto& e : edges) {
            std::sort(e.second.begin(), e.second.end());
        }
        std::sort(nodes.begin(), nodes.end());
    }

    inline void components() {
        std::unordered_map<T, bool> visited;
        for (size_t i = 0; i < nodes.size(); ++i) {
            visited[nodes[i]] = false;
        }
        std::queue<T> queue;
        auto root = nodes.front();
        queue.push(root);
        visited[root] = true;
        while (!queue.empty()) {
            auto v = queue.front();
            queue.pop();
            auto neighbors = edges[v];
            for (size_t i = 0; i < neighbors.size(); ++i) {
                if (!visited[neighbors[i]]) {
                    visited[neighbors[i]] = true;
                    queue.push(neighbors[i]);
                }
            }
        }

        bool all_visited = false;
        int count = 0;
        for (const auto& vis : visited) {
            if (vis.second) {
                count++;
            }
        }
        if (count == nodes.size()) {
            all_visited = true;
        }

        std::cout << " component=" << std::to_string(all_visited) << " component_size=" << nodes.size();
    }
//
//    int Graph::NumberOfconnectedComponents()
//    {
//
//        // Mark all the vertices as not visited
//        bool* visited = new bool[V];
//
//        // To store the number of connected components
//        int count = 0;
//        for (int v = 0; v < V; v++)
//            visited[v] = false;
//
//        for (int v = 0; v < V; v++) {
//            if (visited[v] == false) {
//                DFSUtil(v, visited);
//                count += 1;
//            }
//        }
//
//        return count;
//    }

    inline void components2() {

    }
};
}
