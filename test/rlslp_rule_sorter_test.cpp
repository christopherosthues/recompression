#include <gtest/gtest.h>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/coders/rlslp_rule_sorter.hpp"

using namespace recomp;

TEST(sort_rlslp_rules, empty) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 0;
    rlslp.terminals = alphabet_size;
    rlslp.root = 0;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, terminal) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 112;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 112;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, pair) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 113;
    rlslp.resize(1, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{112, 111, 2};
    rlslp.is_empty = false;
    rlslp.blocks = 1;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 111, 2};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 1;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, short_block2) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 113;
    rlslp.resize(1, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{112, 2, 2};
    rlslp.is_empty = false;
    rlslp.blocks = 0;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 2, 2};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, short_block3) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 113;
    rlslp.terminals = alphabet_size;
    rlslp.root = 113;
    rlslp.resize(1, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{112, 3, 3};
    rlslp.is_empty = false;
    rlslp.blocks = 0;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 3, 3};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, recompression) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    rlslp.root = 19;
    rlslp.resize(20, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[3] = non_terminal<var_t>{23, 1, 4};
    rlslp.non_terminals[4] = non_terminal<var_t>{3, 5, 3};
    rlslp.non_terminals[5] = non_terminal<var_t>{20, 7, 4};
    rlslp.non_terminals[6] = non_terminal<var_t>{21, 6, 4};
    rlslp.non_terminals[7] = non_terminal<var_t>{22, 7, 5};
    rlslp.non_terminals[8] = non_terminal<var_t>{24, 8, 8};
    rlslp.non_terminals[9] = non_terminal<var_t>{10, 9, 7};
    rlslp.non_terminals[10] = non_terminal<var_t>{10, 12, 9};
    rlslp.non_terminals[11] = non_terminal<var_t>{13, 11, 12};
    rlslp.non_terminals[12] = non_terminal<var_t>{11, 14, 11};
    rlslp.non_terminals[13] = non_terminal<var_t>{16, 15, 21};
    rlslp.non_terminals[14] = non_terminal<var_t>{18, 17, 32};
    rlslp.non_terminals[15] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[16] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[17] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[18] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[19] = non_terminal<var_t>{5, 2, 4};
    rlslp.blocks = 15;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 9;
    exp_rlslp.resize(20, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 5, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{10, 13, 32};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{14, 12, 21};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{15, 7, 7};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{16, 11, 11};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{19, 16, 12};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{20, 8, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{21, 6, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{22, 8, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{23, 1, 4};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{24, 18, 8};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{5, 2, 4};
    exp_rlslp.blocks = 15;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, one_block) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 3;
    rlslp.resize(1, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
    rlslp.blocks = 0;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, two_blocks) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 3;
    rlslp.resize(3, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{5, 4, 16};
    rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
    rlslp.blocks = 1;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(3, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 4, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, three_blocks) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 4;
    rlslp.resize(5, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{5, 6, 11};
    rlslp.non_terminals[1] = non_terminal<var_t>{7, 3, 20};
    rlslp.non_terminals[2] = non_terminal<var_t>{1, 7, 7};
    rlslp.non_terminals[3] = non_terminal<var_t>{2, 4, 4};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 9, 9};
    rlslp.blocks = 2;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(5, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 6, 11};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{7, 3, 20};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, four_blocks) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 5;
    rlslp.resize(7, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{8, 6, 6};
    rlslp.non_terminals[1] = non_terminal<var_t>{9, 7, 16};
    rlslp.non_terminals[2] = non_terminal<var_t>{4, 3, 22};
    rlslp.non_terminals[3] = non_terminal<var_t>{0, 2, 2};
    rlslp.non_terminals[4] = non_terminal<var_t>{1, 7, 7};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 4, 4};
    rlslp.non_terminals[6] = non_terminal<var_t>{2, 9, 9};
    rlslp.blocks = 3;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(7, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 4, 22}; // 5    3
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{8, 6, 6};  // 3    4
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{9, 7, 16}; // 4    5
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{0, 2, 2};  // 6    6
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{1, 7, 7};  // 7    7
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 4, 4};  // 8    8
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 9, 9};  // 9    9
    exp_rlslp.blocks = 3;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, repeated_pair) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 4;
    rlslp.resize(2, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 11, 22};
    rlslp.blocks = 1;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(2, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 11, 22};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, repeated_pair_same_occ) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    rlslp.root = 4;
    rlslp.resize(3, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{2, 5, 23};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 11, 22};
    rlslp.blocks = 2;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(3, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 5, 23};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 11, 22};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, left_end) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 4;
    rlslp.terminals = alphabet_size;
    rlslp.root = 9;
    rlslp.resize(8, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};  // 4    4
    rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};  // 5    5
    rlslp.non_terminals[2] = non_terminal<var_t>{5, 1, 3};  // 8    6
    rlslp.non_terminals[3] = non_terminal<var_t>{11, 3, 11};  // 9   7
    rlslp.non_terminals[4] = non_terminal<var_t>{10, 6, 11};  // 10  8
    rlslp.non_terminals[5] = non_terminal<var_t>{7, 8, 22};  // 11 9
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 4, 8};  // 6    10
    rlslp.non_terminals[7] = non_terminal<var_t>{4, 5, 10}; // 7    11
    rlslp.blocks = 6;
    rlslp.is_empty = false;

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 7;
    exp_rlslp.resize(8, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};   // 4   4
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};   // 5   5
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{5, 1, 3};   // 6   6
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{9, 8, 22};  // 9   7
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{10, 6, 11}; // 8   8
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{11, 3, 11}; // 7   9
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 4, 8};   // 10  10
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 5, 10};  // 11  11
    exp_rlslp.blocks = 6;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(sort_rlslp_rules, less_productions) {
    rlslp<var_t> rlslp;
    term_t alphabet_size = 4;
    rlslp.terminals = alphabet_size;
    rlslp.root = 7;
    rlslp.resize(5, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};  // 4    4
    rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};  // 5    5
    rlslp.non_terminals[2] = non_terminal<var_t>{2, 8, 7};  // 7    6
    rlslp.non_terminals[3] = non_terminal<var_t>{6, 5, 9};  // 8    7
    rlslp.non_terminals[4] = non_terminal<var_t>{4, 3, 6};  // 6    8
    rlslp.blocks = 4;
    rlslp.is_empty = false;

    // 2, 4, 4, 4, 5
    // 2, 6, 5
    // 7, 5
    // 8

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 7;
    exp_rlslp.resize(5, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};  // 4    4
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};  // 5    5
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 8, 7};  // 6    6
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{6, 5, 9};  // 7    7
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{4, 3, 6};  // 8    8
    exp_rlslp.blocks = 4;
    exp_rlslp.is_empty = false;

    sort_rlslp_rules(rlslp);

    ASSERT_EQ(exp_rlslp, rlslp);
}
