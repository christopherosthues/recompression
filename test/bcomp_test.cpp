#include <gtest/gtest.h>

#define private public

#include "fast_recompression.hpp"
#include "recompression.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression_fast<var_t, term_t>::text_t fast_text_t;
typedef recompression<var_t, term_t>::text_t text_t;
typedef parallel::recompression<var_t, term_t>::text_t par_text_t;

TEST(parallel_bcomp, no_block) {
    par_text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    par_text_t exp_text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_bcomp, 21214441332311413334133231141321) {
    par_text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    par_text_t exp_text = {2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_bcomp, 222222222222222222222) {
    par_text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    par_text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_bcomp, 22222222211111112222) {
    par_text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    par_text_t exp_text = {5, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_bcomp, 2222222221111111222200) {
    par_text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    par_text_t exp_text = {6, 4, 5, 3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}


TEST(bcomp, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(bcomp, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(bcomp, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(bcomp, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {5, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(bcomp, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {6, 4, 5, 3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}


TEST(fast_bcomp, no_block) {
    fast_text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    std::vector<var_t> mapping;

    rlslp.terminals = 5;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 3, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4};
    term_t exp_alphabet_size = 4;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 3, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping_alpha = {1, 2, 3, 4};
    term_t exp_alphabet_size_alpha = 4;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_bcomp, 21214441332311413334133231141321) {
    fast_text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    std::vector<var_t> mapping;

    rlslp.terminals = 5;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text = {1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 5, 6, 7, 8};
    term_t exp_alphabet_size = 8;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping_alpha = {1, 2, 3, 4, 5, 6, 7, 8};
    term_t exp_alphabet_size_alpha = 8;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_bcomp, 991261051171161051139) {
    fast_text_t text = {4, 4, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    std::vector<var_t> mapping{3, 5, 6, 7, 9, 10, 11, 12};
    rlslp<var_t, term_t> rlslp;
    rlslp.terminals = 5;
    rlslp.root = 0;
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(8, 1, 4);
    rlslp.blocks = {true, true, true, true, false, false, false, false};
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 8;

    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text = {8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    std::vector<var_t> exp_mapping{3, 5, 6, 7, 9, 10, 11, 12, 13};
    term_t exp_alphabet_size = 9;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    std::vector<var_t> exp_mapping_alpha = {3, 5, 6, 7, 9, 10, 11, 12, 13};
    term_t exp_alphabet_size_alpha = 9;
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
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_bcomp, 222222222222222222222) {
    fast_text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    std::vector<var_t> mapping;

    rlslp.terminals = 3;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text = {1};
    std::vector<var_t> exp_mapping = {2, 3};
    term_t exp_alphabet_size = 2;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {0};
    std::vector<var_t> exp_mapping_alpha = {3};
    term_t exp_alphabet_size_alpha = 1;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_bcomp, 22222222211111112222) {
    fast_text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    std::vector<var_t> mapping;

    rlslp.terminals = 3;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text = {4, 2, 3};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 5};
    term_t exp_alphabet_size = 5;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {2, 0, 1};
    std::vector<var_t> exp_mapping_alpha = {3, 4, 5};
    term_t exp_alphabet_size_alpha = 3;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = {true, true, true};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_bcomp, 2222222221111111222200) {
    fast_text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    std::vector<var_t> mapping;

    rlslp.terminals = 3;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    fast_text_t exp_text = {6, 4, 5, 3};
    std::vector<var_t> exp_mapping = {0, 1, 2, 3, 4, 5, 6};
    term_t exp_alphabet_size = 7;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    fast_text_t exp_text_alpha = {3, 1, 2, 0};
    std::vector<var_t>exp_mapping_alpha = {3, 4, 5, 6};
    term_t exp_alphabet_size_alpha = 4;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}
