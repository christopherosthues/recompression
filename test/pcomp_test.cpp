#include <gtest/gtest.h>

#define private public

#include "fast_recompression.hpp"
#include "recompression.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;
typedef recompression_fast<var_t, term_t>::text_t fast_text_t;
typedef parallel::recompression<var_t, term_t>::text_t par_text_t;

TEST(parallel_pcomp, 212181623541741623541321) {
    par_text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.blocks = {true, true, true, true};
    recomp.pcomp(text, rlslp);

    par_text_t exp_text{9, 9, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_pcomp, 131261051171161051139) {
    par_text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp);

    par_text_t exp_text{18, 16, 15, 17, 16, 15, 14};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_pcomp, 18161517161514) {
    par_text_t text{18, 16, 15, 17, 16, 15, 14};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp);

    par_text_t exp_text{21, 20, 16, 19};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_pcomp, 21201619) {
    par_text_t text{21, 20, 16, 19};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false};
    recomp.pcomp(text, rlslp);

    par_text_t exp_text{23, 22};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_pcomp, 2322) {
    par_text_t text{23, 22};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    rlslp.non_terminals.emplace_back(16, 19, 11);
    rlslp.non_terminals.emplace_back(21, 20, 21);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false, false, false};
    recomp.pcomp(text, rlslp);

    par_text_t exp_text{24};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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



TEST(pcomp, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.blocks = {true, true, true, true};
    recomp.pcomp(text, rlslp);

    text_t exp_text{9, 9, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp);

    text_t exp_text{18, 16, 15, 17, 16, 15, 14};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp);

    text_t exp_text{21, 20, 16, 19};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp, 21201619) {
    text_t text{21, 20, 16, 19};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false};
    recomp.pcomp(text, rlslp);

    text_t exp_text{23, 22};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp, 2322) {
    text_t text{23, 22};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    rlslp.non_terminals.emplace_back(16, 19, 11);
    rlslp.non_terminals.emplace_back(21, 20, 21);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false, false, false};
    recomp.pcomp(text, rlslp);

    text_t exp_text{24};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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



TEST(fast_pcomp, 212181623541741623541321) {
    fast_text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    std::vector<var_t> mapping = {1, 2, 3, 4, 5, 6, 7, 8};
    term_t alphabet_size = 8;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.blocks = {true, true, true, true};
    recomp.pcomp(text, rlslp, alphabet_size, mapping);

//    text_t text{9, 9, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    fast_text_t exp_text{8, 8, 11, 5, 9, 4, 10, 6, 10, 5, 9, 4, 10, 2, 8};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    term_t exp_alphabet_size = 12;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {4, 4, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    std::vector<var_t> exp_mapping_alpha = {3, 5, 6, 7, 9, 10, 11, 12};
    term_t exp_alphabet_size_alpha = 8;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_pcomp, 131261051171161051139) {
//    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    fast_text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    std::vector<var_t> mapping = {3, 5, 6, 7, 9, 10, 11, 12, 13};
    term_t alphabet_size = 9;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true};

    recomp.pcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text{13, 11, 10, 12, 11, 10, 9};
    std::vector<var_t> exp_mapping = {3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    term_t exp_alphabet_size = 14;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha{4, 2, 1, 3, 2, 1, 0};
    std::vector<var_t> exp_mapping_alpha = {14, 15, 16, 17, 18};
    term_t exp_alphabet_size_alpha = 5;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_pcomp, 18161517161514) {
    fast_text_t text{4, 2, 1, 3, 2, 1, 0};
    std::vector<var_t> mapping = {14, 15, 16, 17, 18};
    term_t alphabet_size = 5;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text{7, 6, 2, 5};
    std::vector<var_t> exp_mapping = {14, 15, 16, 17, 18, 19, 20, 21};
    term_t exp_alphabet_size = 8;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha{3, 2, 0, 1};
    std::vector<var_t> exp_mapping_alpha = {16, 19, 20, 21};
    term_t exp_alphabet_size_alpha = 4;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_pcomp, 21201619) {
//    text_t text{21, 20, 16, 19};
    fast_text_t text{3, 2, 0, 1};
    std::vector<var_t> mapping = {16, 19, 20, 21};
    term_t alphabet_size = 4;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false};
    recomp.pcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text{5, 4};
    std::vector<var_t> exp_mapping = {16, 19, 20, 21, 22, 23};
    term_t exp_alphabet_size = 6;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha{1, 0};
    std::vector<var_t> exp_mapping_alpha = {22, 23};
    term_t exp_alphabet_size_alpha = 2;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_pcomp, 2322) {
//    text_t text{23, 22};
    fast_text_t text{1, 0};
    std::vector<var_t> mapping = {22, 23};
    term_t alphabet_size = 2;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    rlslp.non_terminals.emplace_back(16, 19, 11);
    rlslp.non_terminals.emplace_back(21, 20, 21);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false, false, false};
    recomp.pcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text{2};
    std::vector<var_t> exp_mapping = {22, 23, 24};
    term_t exp_alphabet_size = 3;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

//    text_t exp_text{24};
    fast_text_t exp_text_alpha{0};
    std::vector<var_t> exp_mapping_alpha = {24};
    term_t exp_alphabet_size_alpha = 1;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
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

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}