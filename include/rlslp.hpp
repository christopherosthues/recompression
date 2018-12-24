
#pragma once

#include <array>
#include <sstream>
#include <string>
#include <vector>

#include "defs.hpp"

namespace recomp {

//const terminal_count_t CHAR_ALPHABET = 256;
const term_t CHAR_ALPHABET = 256;

/**
 * @brief A structure to represent a rlslp.
 *
 * A run-length Straight-line program is a context free grammar in Chomsky normal form that is extended by production
 * rules of the form X^d where X is a non-terminal and d is the number of repeats of the non-terminal.
 */
template<typename variable_t = var_t, typename terminal_count_t = term_t>
struct rlslp {
 private:
    void derive(std::stringstream& sstream, variable_t& nt) {
        if (nt < static_cast<variable_t>(terminals)) {
            sstream << static_cast<char>(nt);
        } else {
            derive(sstream, non_terminals[nt - terminals].first());
            auto second = non_terminals[nt - terminals].second();
            // pair
            if (second >= 0) {
                derive(sstream, second);
            } else {  // block
                variable_t b_len = -second - 1;
                while (b_len--) {
                    derive(sstream, non_terminals[nt - terminals].first());
                }
            }
        }
    }

    inline size_t rest_len(const size_t i, const size_t len, const size_t derived_string_len) const {
        if (i + len > derived_string_len) {
            return len - derived_string_len + i;
        }
        return 0;
    }

    void extract(std::stringstream& sstream, size_t i, size_t len, variable_t nt) const {
        if (len > 0) {
            if (nt < static_cast<variable_t>(terminals)) {
                sstream << static_cast<char>(nt);
            } else {
                auto first = non_terminals[nt - terminals].first();
                auto second = non_terminals[nt - terminals].second();
                size_t first_len = 1;
                if (first >= static_cast<variable_t>(terminals)) {
                    first_len = non_terminals[first - terminals].len;
                }

                // Block
                if (second < 0) {
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
        non_terminal() = default;

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
        non_terminal(variable_t first, variable_t second, size_t len) : len(), production() {
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
        bool operator==(const non_terminal& nt) const {
            return this->production == nt.production && this->len == nt.len;
        }

        variable_t& first() {
            return this->production[0];
        }

        variable_t& second() {
            return this->production[1];
        }

        const variable_t& first() const {
            return this->production[0];
        }

        const variable_t& second() const {
            return this->production[1];
        }
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

    non_terminal& operator[](size_t i) {
        return this->non_terminals[i];
    }

    const non_terminal& operator[](size_t i) const {
        return this->non_terminals[i];
    }

    size_t size() const {
        return non_terminals.size();
    }

    bool empty() const {
        return size() == 0;
    }

    void reserve(size_t size) {
        non_terminals.reserve(size);
    }

    void resize(size_t size) {
        non_terminals.resize(size);
    }

    bool is_block(variable_t nt) const {
        if (nt < terminals) {
            return false;
        }
        return this->non_terminals[nt - terminals].second() < 0;
    }

    size_t len(variable_t nt) const {
        if (nt < terminals) {
            return 1;
        } else {
            return this->non_terminals[nt - terminals].len;
        }
    }

    std::string derive_text() {
        std::stringstream sstream;
        derive(sstream, root);
        return sstream.str();
    }

    std::string extract(size_t i, size_t len) const {
        std::stringstream sstream;
        if (!empty() && i < non_terminals[root].len && len > 0) {
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
std::string to_string(const typename recomp::rlslp<variable_t, terminal_count_t>::non_terminal& nt) {
    std::stringstream sstream("production: (");
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
    std::stringstream sstream("number of terminals: ");
    sstream << rlslp.terminals << std::endl;
    sstream << "non-terminals: " << std::endl;
    size_t i = 0;
    for (const auto& nt : rlslp.non_terminals) {
        sstream << i++ << ": " << to_string(nt) << std::endl;
    }

    return sstream.str();
}

}  // namespace std
