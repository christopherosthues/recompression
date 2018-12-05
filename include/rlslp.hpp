
#pragma once

#include <array>
#include <sstream>
#include <string>
#include <vector>

#include "defs.hpp"

namespace recomp {

const terminal_count_t CHAR_ALPHABET = 256;

/**
 * @brief A structure to represent a rlslp.
 *
 * A run-length Straight-line program is a context free grammar in Chomsky normal form that is extended by production
 * rules of the form X^d where X is a non-terminal and d is the number of repeats of the non-terminal.
 */
struct rlslp {
 private:
    void derive(std::stringstream& sstream, variable_t& nt);

    inline size_t rest_len(const size_t i, const size_t len, const size_t derived_string_len) const {
        if (i + len > derived_string_len) {
            return len - derived_string_len + i;
        }
        return 0;
    }

    void extract(std::stringstream& sstream, size_t i, size_t len, variable_t nt) const;

 public:
    /**
     * @brief A structure to represent a non-terminal.
     *
     * A non-terminal consists of the length of the string that is derived by the non-terminal and its production rule
     */
    struct non_terminal {
        /**
         * The length of the (sub)string derived by this non-terminal.
         */
        size_t len;

        /**
         * The production rule
         */
        std::array<variable_t, 2> production;

        /**
         * Default constructor
         */
        non_terminal();

//        /**
//         * @brief Constructs a non-terminal that derives a single terminal.
//         *
//         * @param terminal
//         */
//        inline non_terminal(variable_t terminal);

        /**
         * @brief Constructs a non-terminal that maps to a pair or a block of non-terminals.
         *
         * @param first The first non-terminal
         * @param second The second non-terminal or the size of the block
         * @param len The length of the derived (sub)string
         */
        non_terminal(variable_t first, variable_t second, size_t len);

        /**
         * @brief Compares this non-terminal with the given non-terminal.
         *
         * @param nt The non-terminal to compare with
         * @return @code{true} if the production rules are equal and the lengths of the derived (sub)strings
         */
        bool operator==(const non_terminal& nt) const;

        variable_t& first();

        variable_t& second();

        const variable_t& first() const;

        const variable_t& second() const;
    };

    /**
     * The root variable (valid index).
     */
    variable_t root = 0;

    /**
     * All production rules of the rlslp.
     */
    std::vector<non_terminal> non_terminals;

    /**
     * The number of terminals.
     */
    terminal_count_t terminals = 0;

    /**
     * The number of blocks.
     */
    size_t block_count = 0;

    non_terminal& operator[](size_t i);

    const non_terminal& operator[](size_t i) const;

    size_t size() const;

    bool empty() const;

    void reserve(size_t size);

    void resize(size_t size);

    std::string derive_text();

    std::string extract(size_t i, size_t len) const;
};

}  // namespace recomp

namespace std {

/**
 * @brief Returns a string representing the content of a non-terminal.
 *
 * @param nt The non-terminal
 * @return A string representation of the non-terminal
 */
std::string to_string(const recomp::rlslp::non_terminal& nt);

/**
 * @brief Returns a string representing the content of a rlslp.
 *
 * @param rlslp The non-terminal
 * @return A string representation of the rlslp
 */
std::string to_string(const recomp::rlslp& rlslp);

}  // namespace std
