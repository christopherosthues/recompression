#pragma once

#include <cstddef>
#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>

#include "defs.hpp"

namespace recomp {

template<typename T>
class graph {
    std::unordered_map<T, std::vector<T>> edges;
    std::vector<T> nodes;
    size_t count = 0;

    struct edge {
        T v, u;

        edge(T v, T u) {
            this->v = v;
            this->u = u;
        }
    };

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

    inline void density() {
        size_t n_edges = 0;
        // Each edge is counted twice
        for (const auto& edge : edges) {
            n_edges += edge.second.size();
        }
        std::cout << " edges=" << n_edges;
        std::cout << " density=" << ((double)(n_edges)) / (double)(nodes.size() * (nodes.size() - 1));
    }

    inline void components() {
        std::unordered_map<T, bool> visited;
        for (size_t i = 0; i < nodes.size(); ++i) {
            visited[nodes[i]] = false;
        }
        size_t n_comps = 0;
        std::deque<size_t> comp_sizes;
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (!visited[nodes[j]]) {
                std::queue<T> queue;
                queue.push(nodes[j]);
                visited[nodes[j]] = true;
                size_t size = 1;
                while (!queue.empty()) {
                    auto v = queue.front();
                    queue.pop();
                    auto neighbors = edges[v];
                    for (size_t i = 0; i < neighbors.size(); ++i) {
                        if (!visited[neighbors[i]]) {
                            size++;
                            visited[neighbors[i]] = true;
                            queue.push(neighbors[i]);
                        }
                    }
                }
                comp_sizes.emplace_back(size);
                n_comps++;
            }
        }

        std::cout << " components=" << n_comps << " component_sizes=";
        for (size_t i = 0; i < comp_sizes.size() - 1; ++i) {
            std::cout << comp_sizes[i] << ",";
        }
        std::cout << comp_sizes[comp_sizes.size() - 1];
    }

    inline void components2(T u,
                            std::unordered_map<T, int>& disc,
                            std::unordered_map<T, int>& low,
                            std::stack<T>& st,
                            std::unordered_map<T, int>& parent) {
        // A static variable is used for simplicity, we can avoid use
        // of static variable by passing a pointer.
        static int time = 0;

        // Initialize discovery time and low value
        disc[u] = low[u] = ++time;
        int children = 0;

        // Go through all vertices adjacent to this
        list<int>::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i) {
            int v = *i; // v is current adjacent of 'u'

            // If v is not visited yet, then recur for it
            if (disc[v] == -1) {
                children++;
                parent[v] = u;
                // store the edge in stack
                st->push_back(Edge(u, v));
                BCCUtil(v, disc, low, st, parent);

                // Check if the subtree rooted with 'v' has a
                // connection to one of the ancestors of 'u'
                // Case 1 -- per Strongly Connected Components Article
                low[u] = min(low[u], low[v]);

                // If u is an articulation point,
                // pop all edges from stack till u -- v
                if ((disc[u] == 1 && children > 1) || (disc[u] > 1 && low[v] >= disc[u])) {
                    while (st->back().u != u || st->back().v != v) {
                        cout << st->back().u << "--" << st->back().v << " ";
                        st->pop_back();
                    }
                    cout << st->back().u << "--" << st->back().v;
                    st->pop_back();
                    cout << endl;
                    count++;
                }
            }

                // Update low value of 'u' only of 'v' is still in stack
                // (i.e. it's a back edge, not cross edge).
                // Case 2 -- per Strongly Connected Components Article
            else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
                if (disc[v] < disc[u]) {
                    st->push_back(Edge(u, v));
                }
            }
        }
    }

    //// A recursive function that finds and prints strongly connected
//// components using DFS traversal
//// u --> The vertex to be visited next
//// disc[] --> Stores discovery times of visited vertices
//// low[] -- >> earliest visited vertex (the vertex with minimum
//// discovery time) that can be reached from subtree
//// rooted with current vertex
//// *st -- >> To store visited edges
//        void Graph::BCCUtil(int u, int disc[], int low[], list<Edge>* st,
//                            int parent[])
//        {
//            // A static variable is used for simplicity, we can avoid use
//            // of static variable by passing a pointer.
//            static int time = 0;
//
//            // Initialize discovery time and low value
//            disc[u] = low[u] = ++time;
//            int children = 0;
//
//            // Go through all vertices adjacent to this
//            list<int>::iterator i;
//            for (i = adj[u].begin(); i != adj[u].end(); ++i) {
//                int v = *i; // v is current adjacent of 'u'
//
//                // If v is not visited yet, then recur for it
//                if (disc[v] == -1) {
//                    children++;
//                    parent[v] = u;
//                    // store the edge in stack
//                    st->push_back(Edge(u, v));
//                    BCCUtil(v, disc, low, st, parent);
//
//                    // Check if the subtree rooted with 'v' has a
//                    // connection to one of the ancestors of 'u'
//                    // Case 1 -- per Strongly Connected Components Article
//                    low[u] = min(low[u], low[v]);
//
//                    // If u is an articulation point,
//                    // pop all edges from stack till u -- v
//                    if ((disc[u] == 1 && children > 1) || (disc[u] > 1 && low[v] >= disc[u])) {
//                        while (st->back().u != u || st->back().v != v) {
//                            cout << st->back().u << "--" << st->back().v << " ";
//                            st->pop_back();
//                        }
//                        cout << st->back().u << "--" << st->back().v;
//                        st->pop_back();
//                        cout << endl;
//                        count++;
//                    }
//                }
//
//                    // Update low value of 'u' only of 'v' is still in stack
//                    // (i.e. it's a back edge, not cross edge).
//                    // Case 2 -- per Strongly Connected Components Article
//                else if (v != parent[u]) {
//                    low[u] = min(low[u], disc[v]);
//                    if (disc[v] < disc[u]) {
//                        st->push_back(Edge(u, v));
//                    }
//                }
//            }
//        }
//
//// The function to do DFS traversal. It uses BCCUtil()
//        void Graph::BCC()
//        {
//            int* disc = new int[V];
//            int* low = new int[V];
//            int* parent = new int[V];
//            list<Edge>* st = new list<Edge>[E];
//
//            // Initialize disc and low, and parent arrays
//            for (int i = 0; i < V; i++) {
//                disc[i] = NIL;
//                low[i] = NIL;
//                parent[i] = NIL;
//            }
//
//            for (int i = 0; i < V; i++) {
//                if (disc[i] == NIL)
//                    BCCUtil(i, disc, low, st, parent);
//
//                int j = 0;
//                // If stack is not empty, pop all edges from stack
//                while (st->size() > 0) {
//                    j = 1;
//                    cout << st->back().u << "--" << st->back().v << " ";
//                    st->pop_back();
//                }
//                if (j == 1) {
//                    cout << endl;
//                    count++;
//                }
//            }
//        }
};
}
