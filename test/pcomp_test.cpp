#include <gtest/gtest.h>

#define private public
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;

TEST(pcomp, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, -2, 2);
    rlslp.non_terminals.emplace_back(3, -2, 2);
    rlslp.non_terminals.emplace_back(3, -3, 3);
    rlslp.non_terminals.emplace_back(4, -3, 3);
    recomp.pcomp(text, rlslp);

    text_t exp_text{9, 9, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    ASSERT_EQ(exp_text, text);
}

TEST(pcomp, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, -2, 2);
    rlslp.non_terminals.emplace_back(3, -2, 2);
    rlslp.non_terminals.emplace_back(3, -3, 3);
    rlslp.non_terminals.emplace_back(4, -3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, -2, 4);
    recomp.pcomp(text, rlslp);

    text_t exp_text{18, 16, 15, 17, 16, 15, 14};
    ASSERT_EQ(exp_text, text);
}

TEST(pcomp, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, -2, 2);
    rlslp.non_terminals.emplace_back(3, -2, 2);
    rlslp.non_terminals.emplace_back(3, -3, 3);
    rlslp.non_terminals.emplace_back(4, -3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, -2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    recomp.pcomp(text, rlslp);

    text_t exp_text{21, 20, 16, 19};
    ASSERT_EQ(exp_text, text);
}

TEST(pcomp, 21201619) {
    text_t text{21, 20, 16, 19};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, -2, 2);
    rlslp.non_terminals.emplace_back(3, -2, 2);
    rlslp.non_terminals.emplace_back(3, -3, 3);
    rlslp.non_terminals.emplace_back(4, -3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, -2, 4);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(5, 11, 4);
    rlslp.non_terminals.emplace_back(6, 10, 4);
    rlslp.non_terminals.emplace_back(7, 11, 5);
    rlslp.non_terminals.emplace_back(13, 12, 8);
    rlslp.non_terminals.emplace_back(15, 14, 7);
    rlslp.non_terminals.emplace_back(15, 17, 9);
    rlslp.non_terminals.emplace_back(18, 16, 12);
    recomp.pcomp(text, rlslp);

    text_t exp_text{23, 22};
    ASSERT_EQ(exp_text, text);
}

TEST(pcomp, 2322) {
    text_t text{23, 22};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(1, -2, 2);
    rlslp.non_terminals.emplace_back(3, -2, 2);
    rlslp.non_terminals.emplace_back(3, -3, 3);
    rlslp.non_terminals.emplace_back(4, -3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.non_terminals.emplace_back(9, -2, 4);
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
    recomp.pcomp(text, rlslp);

    text_t exp_text{24};
    ASSERT_EQ(exp_text, text);
}
