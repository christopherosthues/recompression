#include <gtest/gtest.h>

#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;

TEST(recomp, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {24};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 19;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
    exp_rlslp.non_terminals.emplace_back(5, 11, 4);
    exp_rlslp.non_terminals.emplace_back(6, 10, 4);
    exp_rlslp.non_terminals.emplace_back(7, 11, 5);
    exp_rlslp.non_terminals.emplace_back(13, 12, 8);
    exp_rlslp.non_terminals.emplace_back(15, 14, 7);
    exp_rlslp.non_terminals.emplace_back(15, 17, 9);
    exp_rlslp.non_terminals.emplace_back(18, 16, 12);
    exp_rlslp.non_terminals.emplace_back(16, 19, 11);
    exp_rlslp.non_terminals.emplace_back(21, 20, 21);
    exp_rlslp.non_terminals.emplace_back(23, 22, 32);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {5};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 2;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(4, 3, 16);
    exp_rlslp.blocks = {true, true, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {7};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(3, 4, 11);
    exp_rlslp.non_terminals.emplace_back(5, 6, 20);
    exp_rlslp.blocks = {true, true, true, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {9};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 6;
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(5, 3, 6);
    exp_rlslp.non_terminals.emplace_back(6, 4, 16);
    exp_rlslp.non_terminals.emplace_back(8, 7, 22);
    exp_rlslp.blocks = {true, true, true, true, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

//TEST(lce, recompression_empty) {
//    text_t t;
//    lce::recompression_vector lce;
//
//    lce.recomp(t);
//
//    ASSERT_EQ(true, lce.slp.non_terminals.empty());
//    ASSERT_EQ(0, lce.slp.terminals);
//    ASSERT_EQ(true, lce.slp.blocks.empty());
//}
//
//TEST(lce, recompression_complete_block) {
//    lce::recompression_vector lce;
//    text_t text = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
//    sdsl::bit_vector blk(2, 0);
//    blk[1] = true;
//
//    lce.recomp(text);
//
//    ASSERT_EQ(1, lce.slp.terminals);
//    ASSERT_EQ(blk, lce.slp.blocks);
//    ASSERT_EQ(2, lce.slp.non_terminals.size());
//    ASSERT_EQ(4, lce.slp.non_terminals[0].production[0]);
//    ASSERT_EQ(0, lce.slp.non_terminals[1].production[0]);
//    ASSERT_EQ(12, lce.slp.non_terminals[1].production[1]);
//}
//
//TEST(lce, recompression_two_blocks) {
//    lce::recompression_vector lce;
//    text_t text = {4, 4, 4, 4, 4, 4, 4, 5, 5, 5};
//    sdsl::bit_vector blk(5, 0);
//    blk[2] = true;
//    blk[3] = true;
//    std::vector<lce::rlslp<variable_t, std::uint32_t>::non_terminal> nts;
//    nts.emplace_back(4, 1);
//    nts.emplace_back(5, 1);
//    nts.emplace_back(0, 7, 7);
//    nts.emplace_back(1, 3, 3);
//    nts.emplace_back(2, 3, 10);
//
//    lce.recomp(text);
//
//    ASSERT_EQ(2, lce.slp.terminals);
//    ASSERT_EQ(blk, lce.slp.blocks);
//    ASSERT_EQ(5, lce.slp.non_terminals.size());
//    ASSERT_EQ(nts, lce.slp.non_terminals);
//}
