#include <gtest/gtest.h>

#define private public
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;

TEST(bcomp, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};

    ASSERT_EQ(exp_text, text);
}

TEST(bcomp, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};

    ASSERT_EQ(exp_text, text);
}

TEST(bcomp, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {3};

    ASSERT_EQ(exp_text, text);
}

TEST(bcomp, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {5, 3, 4};

    ASSERT_EQ(exp_text, text);
}

TEST(bcomp, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {6, 4, 5, 3};

    ASSERT_EQ(exp_text, text);
}
