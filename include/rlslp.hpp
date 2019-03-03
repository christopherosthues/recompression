
#pragma once

#include <array>
#include <sstream>
#include <string>
#include <vector>

#include "defs.hpp"

namespace recomp {

const term_t CHAR_ALPHABET = 256;

/**
     * @brief A structure to represent a non-terminal.
     *
     * A non-terminal consists of the length of the string that is derived by the non-terminal and its production rule
     */
template<typename variable_t, typename terminal_count_t>
struct non_terminal {
 public:
    typedef variable_t value_t;
    typedef terminal_count_t terminals_t;

    /**
     * The length of the (sub)string derived by this non-terminal.
     */
    size_t len;

    /**
     * The production rule
     */
    std::array<value_t, 2> production;

    /**
     * Default constructor
     */
    non_terminal() = default;

    /**
     * @brief Constructs a non-terminal that maps to a pair or a block of non-terminals.
     *
     * @param first The first non-terminal
     * @param second The second non-terminal or the size of the block
     */
    non_terminal(value_t first, value_t second) : production() {
        this->production[0] = first;
        this->production[1] = second;
        this->len = 1;
    }

    /**
     * @brief Constructs a non-terminal that maps to a pair or a block of non-terminals.
     *
     * @param first The first non-terminal
     * @param second The second non-terminal or the size of the block
     * @param len The length of the derived (sub)string
     */
    non_terminal(value_t first, value_t second, size_t len) : production() {
        this->production[0] = first;
        this->production[1] = second;
        this->len = len;
    }

    /**
     * @brief Compares this non-terminal with the given non-terminal.
     *
     * @param nt The non-terminal to compare with
     * @return @code{true} if the production rules are equal and the lengths of the derived (sub)strings
     */
    bool operator==(const non_terminal<value_t, terminals_t >& nt) const {
        return this->production == nt.production && this->len == nt.len;
    }

    non_terminal<value_t, terminals_t>& operator=(const non_terminal<value_t, terminals_t>& nt) {
        this->production[0] = nt.production[0];
        this->production[1] = nt.production[1];
        this->len = nt.len;
        return *this;
    }

    value_t& first() {
        return this->production[0];
    }

    value_t& second() {
        return this->production[1];
    }

    const value_t& first() const {
        return this->production[0];
    }

    const value_t& second() const {
        return this->production[1];
    }
};

/**
 * @brief A structure to represent a rlslp.
 *
 * A run-length Straight-line program is a context free grammar in Chomsky normal form that is extended by production
 * rules of the form X^d where X is a non-terminal and d is the number of repeats of the non-terminal.
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
class rlslp {
 public:
    typedef variable_t value_t;
    typedef terminal_count_t terminals_t;

 private:
    size_t compute_length(const value_t nt) {
        if (nt < terminals) {
            return 1;
        } else {
            auto first = non_terminals[nt - terminals].first();
            auto second = non_terminals[nt - terminals].second();

            if (is_block(nt)) {
                size_t len = compute_length(first);
                non_terminals[nt - terminals].len = len * second;
                return len * second;
            } else {
                size_t first_len = compute_length(first);
                size_t second_len = compute_length(second);
//                non_terminals[first - terminals].len = first_len;
//                non_terminals[second - terminals].len = second_len;
                non_terminals[nt - terminals].len = first_len + second_len;
                return first_len + second_len;
            }
        }
    }

    inline size_t rest_len(const size_t i, const size_t len, const size_t derived_string_len) const {
        if (i + len > derived_string_len) {
            return len - derived_string_len + i;
        }
        return 0;
    }

    void extract(std::stringstream& sstream, size_t i, size_t len, value_t nt) const {
        if (len > 0) {
            if (nt < static_cast<value_t>(terminals)) {
                sstream << static_cast<char>(nt);
            } else {
                auto first = non_terminals[nt - terminals].first();
                auto second = non_terminals[nt - terminals].second();
                size_t first_len = 1;
                if (first >= static_cast<value_t>(terminals)) {
                    first_len = non_terminals[first - terminals].len;
                }

                if (is_block(nt)) {  // Block
                    auto b_len = first_len;
                    auto idx = 0;

                    while (i >= first_len) {
                        first_len += b_len;
                        idx++;
                    }
                    auto r_len = rest_len(i, len, first_len);

                    extract(sstream, i - idx * b_len, len - r_len, first);

                    while (r_len > 0) {
                        if (r_len > b_len) {
                            r_len = b_len;
                        }
                        extract(sstream, 0, r_len, first);
                        first_len += b_len;
                        r_len = rest_len(i, len, first_len);
                    }
                } else {  // Pair
                    auto r_len = rest_len(i, len, first_len);

                    if (i < first_len) {
                        extract(sstream, i, len - r_len, first);

                        if (r_len > 0) {
                            extract(sstream, 0, r_len, second);
                        }
                    } else {
                        extract(sstream, i - first_len, len, second);
                    }
                }
            }
        }
    }

 public:
    /**
     * The root variable.
     */
    value_t root = 0;

    /**
     * All production rules of the rlslp.
     */
    std::vector<non_terminal<value_t, terminals_t>> non_terminals;

    /**
     * The number of terminals.
     */
    terminals_t terminals = CHAR_ALPHABET;

    /**
     * @code{true} if the rlslp is empty, @code{false} if not
     */
    bool is_empty = true;

    /**
     * The number of blocks.
     */
    value_t blocks = 0;

//    /**
//     * Bitvector marking which productions derive blocks.
//     */
//    block_t blocks;

    non_terminal<value_t, terminals_t>& operator[](size_t i) {
        return this->non_terminals[i];
    }

    const non_terminal<value_t, terminals_t>& operator[](size_t i) const {
        return this->non_terminals[i];
    }

    size_t size() const {
        return non_terminals.size();
    }

    bool empty() const {
        return is_empty;
    }

    bool operator==(const rlslp<value_t, terminals_t>& rlslp) const {
        return terminals == rlslp.terminals && root == rlslp.root && non_terminals == rlslp.non_terminals &&
               is_empty == rlslp.is_empty && blocks == rlslp.blocks;
    }

    void reserve(size_t size) {
        non_terminals.reserve(size);
//        blocks.reserve(size);
    }

    void resize(size_t size/*, bool block = false*/) {
        non_terminals.resize(size);
//        blocks.resize(size, block);
    }

    void shrink_to_fit() {
        non_terminals.shrink_to_fit();
//        blocks.shrink_to_fit();
    }

    bool is_terminal(value_t nt) const {
        return nt < terminals;
    }

    bool is_block(value_t nt) const {
        if (nt < terminals) {
            return false;
        }
        return nt - terminals >= blocks;
//        if (nt < terminals) {
//            return false;
//        }
//        return this->blocks[nt - terminals];
    }

    void derive(std::stringstream& sstream, value_t nt) {
        if (nt < static_cast<value_t>(terminals)) {
            sstream << static_cast<char>(nt);
        } else {
            auto first = non_terminals[nt - terminals].first();
            auto second = non_terminals[nt - terminals].second();

            if (is_block(nt)) {  // block
                value_t b_len = second;
                while (b_len--) {
                    derive(sstream, first);
                }
            } else {  // pair
                derive(sstream, first);
                derive(sstream, second);
            }
        }
    }

    void compute_lengths() {
        compute_length(root);
    }

    size_t len(value_t nt) const {
        if (nt < terminals) {
            return 1;
        } else {
            return this->non_terminals[nt - terminals].len;
        }
    }

    std::string derive_text() {
        std::stringstream sstream;
        if (!empty()) {
            derive(sstream, root);
        }
        return sstream.str();
    }

    std::string extract(size_t i, size_t len) const {
        std::stringstream sstream;
        if (!empty() && i < this->len(root) && len > 0) {
            if (i + len > this->len(root)) {
                len = this->len(root) - i;
            }
            extract(sstream, i, len, root);
        }
        return sstream.str();
    }
};

}  // namespace recomp

namespace std {

/**
 * @brief Returns a string representing the content of a non-terminal.
 *
 * @param nt The non-terminal
 * @return A string representation of the non-terminal
 */
template<typename variable_t = recomp::var_t, typename terminal_count_t = recomp::term_t>
std::string to_string(const typename recomp::non_terminal<variable_t, terminal_count_t>& nt) {
    std::stringstream sstream;
    sstream << "production: (";
    sstream << std::to_string(nt.production[0]) << "," << std::to_string(nt.production[1])
            << ") len: " << std::to_string(nt.len);

    return sstream.str();
}

/**
 * @brief Returns a string representing the content of a rlslp.
 *
 * @param rlslp The non-terminal
 * @return A string representation of the rlslp
 */
template<typename variable_t = recomp::var_t, typename terminal_count_t = recomp::term_t>
std::string to_string(const typename recomp::rlslp<variable_t, terminal_count_t>& rlslp) {
    std::stringstream sstream;
    sstream << "root: " << rlslp.root << std::endl;
    sstream << "number of terminals: ";
    sstream << rlslp.terminals << std::endl;
    sstream << "non-terminals: " << std::endl;
    size_t i = 0;
    for (const auto& nt : rlslp.non_terminals) {
        sstream << i << " (" << (i + rlslp.terminals) << "): " << to_string(nt) << std::endl;
        i++;
    }
    sstream << "blocks: ";
    sstream << rlslp.blocks;
//    for (const auto& block : rlslp.blocks) {
//        sstream << block;
//    }
    sstream << std::endl;

    return sstream.str();
}

}  // namespace std
