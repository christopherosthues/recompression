#include <gtest/gtest.h>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/coders/rlslp_rule_sorter.hpp"

using namespace recomp;

TEST(sort_rlslp_rules, empty) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 0;
    rlslp.terminals = alphabet_size;
    rlslp.root = 0;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, terminal) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 112;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 112;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, pair) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 113;
    rlslp.non_terminals.emplace_back(112, 111, 2);
    rlslp.is_empty = false;
    rlslp.blocks = 1;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.non_terminals.emplace_back(112, 111, 2);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 1;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, short_block2) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 113;
    rlslp.non_terminals.emplace_back(112, 2, 2);
    rlslp.is_empty = false;
    rlslp.blocks = 0;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.non_terminals.emplace_back(112, 2, 2);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, short_block3) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 113;
    rlslp.non_terminals.emplace_back(112, 3, 3);
    rlslp.is_empty = false;
    rlslp.blocks = 0;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.non_terminals.emplace_back(112, 3, 3);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, recompression) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    rlslp.root = 19;
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(23, 1, 4);
    rlslp.non_terminals.emplace_back(3, 5, 3);
    rlslp.non_terminals.emplace_back(20, 7, 4);
    rlslp.non_terminals.emplace_back(21, 6, 4);
    rlslp.non_terminals.emplace_back(22, 7, 5);
    rlslp.non_terminals.emplace_back(24, 8, 8);
    rlslp.non_terminals.emplace_back(10, 9, 7);
    rlslp.non_terminals.emplace_back(10, 12, 9);
    rlslp.non_terminals.emplace_back(13, 11, 12);
    rlslp.non_terminals.emplace_back(11, 14, 11);
    rlslp.non_terminals.emplace_back(16, 15, 21);
    rlslp.non_terminals.emplace_back(18, 17, 32);
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(5, 2, 4);
    rlslp.blocks = 15;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 9;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(3, 5, 3);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(10, 13, 32);
    exp_rlslp.non_terminals.emplace_back(14, 12, 21);
    exp_rlslp.non_terminals.emplace_back(15, 7, 7);
    exp_rlslp.non_terminals.emplace_back(15, 17, 9);
    exp_rlslp.non_terminals.emplace_back(16, 11, 11);
    exp_rlslp.non_terminals.emplace_back(19, 16, 12);
    exp_rlslp.non_terminals.emplace_back(20, 8, 4);
    exp_rlslp.non_terminals.emplace_back(21, 6, 4);
    exp_rlslp.non_terminals.emplace_back(22, 8, 5);
    exp_rlslp.non_terminals.emplace_back(23, 1, 4);
    exp_rlslp.non_terminals.emplace_back(24, 18, 8);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(5, 2, 4);
    exp_rlslp.blocks = 15;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, one_block) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 3;
    rlslp.non_terminals.emplace_back(2, 21, 21);
    rlslp.blocks = 0;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, two_blocks) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 3;
    rlslp.non_terminals.emplace_back(5, 4, 16);
    rlslp.non_terminals.emplace_back(1, 7, 7);
    rlslp.non_terminals.emplace_back(2, 9, 9);
    rlslp.blocks = 1;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.non_terminals.emplace_back(5, 4, 16);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, three_blocks) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 4;
    rlslp.non_terminals.emplace_back(5, 6, 11);
    rlslp.non_terminals.emplace_back(7, 3, 20);
    rlslp.non_terminals.emplace_back(1, 7, 7);
    rlslp.non_terminals.emplace_back(2, 4, 4);
    rlslp.non_terminals.emplace_back(2, 9, 9);
    rlslp.blocks = 2;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.non_terminals.emplace_back(5, 6, 11);
    exp_rlslp.non_terminals.emplace_back(7, 3, 20);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, four_blocks) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 5;
    rlslp.non_terminals.emplace_back(8, 6, 6);
    rlslp.non_terminals.emplace_back(9, 7, 16);
    rlslp.non_terminals.emplace_back(4, 3, 22);
    rlslp.non_terminals.emplace_back(0, 2, 2);
    rlslp.non_terminals.emplace_back(1, 7, 7);
    rlslp.non_terminals.emplace_back(2, 4, 4);
    rlslp.non_terminals.emplace_back(2, 9, 9);
    rlslp.blocks = 3;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.non_terminals.emplace_back(5, 4, 22); // 5    3
    exp_rlslp.non_terminals.emplace_back(8, 6, 6);  // 3    4
    exp_rlslp.non_terminals.emplace_back(9, 7, 16); // 4    5
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);  // 6    6
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);  // 7    7
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);  // 8    8
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);  // 9    9
    exp_rlslp.blocks = 3;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, repeated_pair) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 4;
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(3, 11, 22);
    rlslp.blocks = 1;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, repeated_pair_same_occ) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 4;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(2, 5, 23);
    rlslp.non_terminals.emplace_back(3, 11, 22);
    rlslp.blocks = 2;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(2, 5, 23);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, left_end) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 4;
    rlslp.terminals = alphabet_size;
    rlslp.root = 9;
    rlslp.non_terminals.emplace_back(1, 2, 2);  // 4    4
    rlslp.non_terminals.emplace_back(1, 3, 2);  // 5    5
    rlslp.non_terminals.emplace_back(5, 1, 3);  // 8    6
    rlslp.non_terminals.emplace_back(11, 3, 11);  // 9   7
    rlslp.non_terminals.emplace_back(10, 6, 11);  // 10  8
    rlslp.non_terminals.emplace_back(7, 8, 22);  // 11 9
    rlslp.non_terminals.emplace_back(4, 4, 8);  // 6    10
    rlslp.non_terminals.emplace_back(4, 5, 10); // 7    11
    rlslp.blocks = 6;
    rlslp.is_empty = false;

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 7;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);   // 4   4
    exp_rlslp.non_terminals.emplace_back(1, 3, 2);   // 5   5
    exp_rlslp.non_terminals.emplace_back(5, 1, 3);   // 6   6
    exp_rlslp.non_terminals.emplace_back(9, 8, 22);  // 9   7
    exp_rlslp.non_terminals.emplace_back(10, 6, 11); // 8   8
    exp_rlslp.non_terminals.emplace_back(11, 3, 11); // 7   9
    exp_rlslp.non_terminals.emplace_back(4, 4, 8);   // 10  10
    exp_rlslp.non_terminals.emplace_back(4, 5, 10);  // 11  11
    exp_rlslp.blocks = 6;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, less_productions) {
    rlslp<var_t, term_t> rlslp;
    term_t alphabet_size = 4;
    rlslp.terminals = alphabet_size;
    rlslp.root = 7;
    rlslp.non_terminals.emplace_back(1, 2, 2);  // 4    4
    rlslp.non_terminals.emplace_back(1, 3, 2);  // 5    5
    rlslp.non_terminals.emplace_back(2, 8, 7);  // 7    6
    rlslp.non_terminals.emplace_back(6, 5, 9);  // 8    7
    rlslp.non_terminals.emplace_back(4, 3, 6);  // 6    8
    rlslp.blocks = 4;
    rlslp.is_empty = false;

    // 2, 4, 4, 4, 5
    // 2, 6, 5
    // 7, 5
    // 8

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 7;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);  // 4    4
    exp_rlslp.non_terminals.emplace_back(1, 3, 2);  // 5    5
    exp_rlslp.non_terminals.emplace_back(2, 8, 7);  // 6    6
    exp_rlslp.non_terminals.emplace_back(6, 5, 9);  // 7    7
    exp_rlslp.non_terminals.emplace_back(4, 3, 6);  // 8    8
    exp_rlslp.blocks = 4;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}