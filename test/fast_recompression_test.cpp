#include <gtest/gtest.h>

#define private public

#include "fast_recompression.hpp"

using namespace recomp;

typedef recompression_fast<var_t, term_t>::text_t text_t;
typedef recompression_fast<var_t, term_t>::adj_list_t adj_list_t;
typedef recompression_fast<var_t, term_t>::partition_t partition_t;
typedef recompression_fast<var_t, term_t>::alphabet_t alphabet_t;


TEST(replace_letters, 212141323141341323141321) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = 5;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 3, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 4;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 212141323181341323141321) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 8, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 9;
    rlslp.terminals = 9;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 4, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 8};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 9;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 5;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = 5;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 3, 3, 0, 2, 2, 1, 2, 0, 0, 3, 0, 2, 2, 2, 3, 0, 2, 2, 1, 2, 0, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 4;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = 3;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<var_t> exp_mapping = {2};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 1;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = 3;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
    std::vector<var_t> exp_mapping = {1, 2};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 2;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = 3;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    std::vector<var_t> exp_mapping = {0, 1, 2};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 3;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}


TEST(fast_bcomp, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    std::vector<var_t> mapping;

    rlslp.terminals = 5;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 3, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4};
    term_t exp_alphabet_size = 4;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 3, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
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
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    std::vector<var_t> mapping;

    rlslp.terminals = 5;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 5, 6, 7, 8};
    term_t exp_alphabet_size = 8;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
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
    text_t text = {4, 4, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
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

    text_t exp_text = {8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    std::vector<var_t> exp_mapping{3, 5, 6, 7, 9, 10, 11, 12, 13};
    term_t exp_alphabet_size = 9;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
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
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    std::vector<var_t> mapping;

    rlslp.terminals = 3;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text = {1};
    std::vector<var_t> exp_mapping = {2, 3};
    term_t exp_alphabet_size = 2;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {0};
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
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    std::vector<var_t> mapping;

    rlslp.terminals = 3;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text = {4, 2, 3};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 5};
    term_t exp_alphabet_size = 5;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {2, 0, 1};
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
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    std::vector<var_t> mapping;

    rlslp.terminals = 3;
    recomp.replace_letters(text, alphabet_size, mapping);
    recomp.bcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text = {6, 4, 5, 3};
    std::vector<var_t> exp_mapping = {0, 1, 2, 3, 4, 5, 6};
    term_t exp_alphabet_size = 7;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {3, 1, 2, 0};
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


TEST(fast_adj_list, 212181623541741623541321) {
    text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(3, 1);
    exp_adj_list[2][0] = std::make_pair(0, 1);
    exp_adj_list[2][1] = std::make_pair(1, 2);
    exp_adj_list[3][0] = std::make_pair(3, 0);
    exp_adj_list[4][2] = std::make_pair(0, 2);
    exp_adj_list[4][3] = std::make_pair(2, 0);
    exp_adj_list[5][0] = std::make_pair(0, 2);
    exp_adj_list[5][1] = std::make_pair(2, 0);
    exp_adj_list[6][0] = std::make_pair(0, 1);
    exp_adj_list[6][3] = std::make_pair(1, 0);
    exp_adj_list[7][0] = std::make_pair(1, 1);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}

TEST(fast_adj_list, 131261051171161051139) {
    text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[4][0] = std::make_pair(0, 1);
    exp_adj_list[5][1] = std::make_pair(2, 0);
    exp_adj_list[5][2] = std::make_pair(0, 2);
    exp_adj_list[6][0] = std::make_pair(1, 0);
    exp_adj_list[6][1] = std::make_pair(0, 2);
    exp_adj_list[6][2] = std::make_pair(1, 0);
    exp_adj_list[6][3] = std::make_pair(1, 1);
    exp_adj_list[7][2] = std::make_pair(1, 0);
    exp_adj_list[8][7] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}

TEST(fast_adj_list, 18161517161514) {
    text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(1, 0);
    exp_adj_list[2][1] = std::make_pair(2, 0);
    exp_adj_list[3][1] = std::make_pair(0, 1);
    exp_adj_list[3][2] = std::make_pair(1, 0);
    exp_adj_list[4][2] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}

TEST(fast_adj_list, 21201619) {
    text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(0, 1);
    exp_adj_list[2][0] = std::make_pair(1, 0);
    exp_adj_list[3][2] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}

TEST(fast_adj_list, 2322) {
    text_t text{1, 0};
    term_t alphabet_size = 2;
    adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}


TEST(fast_compute_partition, repreated_pair) {
    text_t text{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    term_t alphabet_size = 2;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, repreated_pair_same_occ) {
    text_t text{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
    term_t alphabet_size = 2;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 212181623541741623541321) {
    text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;
    exp_partition[3] = true;
    exp_partition[7] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 131261051171161051139) {
    text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[4] = true;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 18161517161514) {
    text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;
    exp_partition[4] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 21201619) {
    text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;
    exp_partition[2] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 2322) {
    text_t text{1, 0};
    term_t alphabet_size = 2;
    adj_list_t adj_list(alphabet_size);
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;

    ASSERT_EQ(exp_partition, partition);
}


TEST(fast_partition, repreated_pair) {
    text_t text{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    term_t alphabet_size = 2;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, repreated_pair_same_occ) {
    text_t text{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
    term_t alphabet_size = 2;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 212181623541741623541321) {
    text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;
    exp_partition[1] = false;
    exp_partition[2] = true;
    exp_partition[3] = false;
    exp_partition[4] = true;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 131261051171161051139) {
    text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = false;
    exp_partition[1] = false;
    exp_partition[2] = false;
    exp_partition[3] = false;
    exp_partition[4] = true;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = true;
    exp_partition[8] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 18161517161514) {
    text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;
    exp_partition[1] = false;
    exp_partition[2] = true;
    exp_partition[3] = true;
    exp_partition[4] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 21201619) {
    text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = false;
    exp_partition[1] = true;
    exp_partition[2] = true;
    exp_partition[3] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 2322) {
    text_t text{1, 0};
    term_t alphabet_size = 2;
    partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;
    exp_partition[1] = false;

    ASSERT_EQ(exp_partition, partition);
}


TEST(fast_pcomp, repeated_pair) {
    text_t text{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    std::vector<var_t> mapping = {1, 2};
    term_t alphabet_size = 2;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 3;
    recomp.pcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    std::vector<var_t> exp_mapping = {1, 2, 3};
    term_t exp_alphabet_size = 3;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<var_t> exp_mapping_alpha = {3};
    term_t exp_alphabet_size_alpha = 1;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.blocks = {false};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_pcomp, repeated_pair_same_occ) {
    text_t text{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
    std::vector<var_t> mapping = {1, 2};
    term_t alphabet_size = 2;
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    rlslp.terminals = 3;
    recomp.pcomp(text, rlslp, alphabet_size, mapping);

    text_t exp_text{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    std::vector<var_t> exp_mapping = {1, 2, 3};
    term_t exp_alphabet_size = 3;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<var_t> exp_mapping_alpha = {2, 3};
    term_t exp_alphabet_size_alpha = 2;
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.blocks = {false};

    ASSERT_EQ(exp_text_alpha, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size_alpha, alphabet_size);
    ASSERT_EQ(exp_mapping_alpha, mapping);
}

TEST(fast_pcomp, 212181623541741623541321) {
    text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
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

    text_t exp_text{8, 8, 11, 5, 9, 4, 10, 6, 10, 5, 9, 4, 10, 2, 8};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    term_t exp_alphabet_size = 12;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha = {4, 4, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
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
    text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
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

    text_t exp_text{13, 11, 10, 12, 11, 10, 9};
    std::vector<var_t> exp_mapping = {3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    term_t exp_alphabet_size = 14;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha{4, 2, 1, 3, 2, 1, 0};
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
    text_t text{4, 2, 1, 3, 2, 1, 0};
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

    text_t exp_text{7, 6, 2, 5};
    std::vector<var_t> exp_mapping = {14, 15, 16, 17, 18, 19, 20, 21};
    term_t exp_alphabet_size = 8;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

    text_t exp_text_alpha{3, 2, 0, 1};
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
    text_t text{3, 2, 0, 1};
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
    text_t text{1, 0};
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

    text_t exp_text{2};
    std::vector<var_t> exp_mapping = {22, 23, 24};
    term_t exp_alphabet_size = 3;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);

    recomp.compute_alphabet(text, alphabet_size, mapping);

//    text_t exp_text{24};
    text_t exp_text_alpha{0};
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


TEST(fast_recomp, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(fast_recomp, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;

    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
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

TEST(fast_recomp, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(fast_recomp, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
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

TEST(fast_recomp, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
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

TEST(fast_recomp, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
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

TEST(fast_recomp, repeated_pair) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 1;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = {false, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(fast_recomp, repeated_pair_same_occ) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {0};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 2;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.non_terminals.emplace_back(2, 4, 23);
    exp_rlslp.blocks = {false, true, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}