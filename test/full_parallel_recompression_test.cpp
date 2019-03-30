#include <gtest/gtest.h>

#define private public

#include "recompression/full_parallel_recompression.hpp"

using namespace recomp;

typedef parallel::full_parallel_recompression<var_t, term_t>::text_t text_t;
typedef parallel::full_parallel_recompression<var_t, term_t>::adj_list_t adj_list_t;
typedef parallel::full_parallel_recompression<var_t, term_t>::partition_t partition_t;
typedef parallel::full_parallel_recompression<var_t, term_t>::alphabet_t alphabet_t;
typedef parallel::full_parallel_recompression<var_t, term_t>::bv_t bv_t;

TEST(full_parallel_bcomp, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    bv_t exp_bv;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(full_parallel_bcomp, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = {2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.blocks = 4;
    bv_t exp_bv{true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(full_parallel_bcomp, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 1;
    bv_t exp_bv{true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(full_parallel_bcomp, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = {5, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 3;
    bv_t exp_bv{true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(full_parallel_bcomp, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = {6, 4, 5, 3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 4;
    bv_t exp_bv{true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(full_parallel_mapping, left_end) {
    text_t text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(1);
    exp_mapping.emplace_back(2);
    exp_mapping.emplace_back(3);

    text_t exp_text = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(full_parallel_mapping, 212181623541741623541321) {
    text_t text = {2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(1);
    exp_mapping.emplace_back(2);
    exp_mapping.emplace_back(3);
    exp_mapping.emplace_back(4);
    exp_mapping.emplace_back(5);
    exp_mapping.emplace_back(6);
    exp_mapping.emplace_back(7);
    exp_mapping.emplace_back(8);

    text_t exp_text = {1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(full_parallel_mapping, 131261051171161051139) {
    text_t text = {13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(3);
    exp_mapping.emplace_back(5);
    exp_mapping.emplace_back(6);
    exp_mapping.emplace_back(7);
    exp_mapping.emplace_back(9);
    exp_mapping.emplace_back(10);
    exp_mapping.emplace_back(11);
    exp_mapping.emplace_back(12);
    exp_mapping.emplace_back(13);

    text_t exp_text = {8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(full_parallel_mapping, 18161517161514) {
    text_t text = {18, 16, 15, 17, 16, 15, 14};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(14);
    exp_mapping.emplace_back(15);
    exp_mapping.emplace_back(16);
    exp_mapping.emplace_back(17);
    exp_mapping.emplace_back(18);

    text_t exp_text = {4, 2, 1, 3, 2, 1, 0};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(full_parallel_mapping, 21201619) {
    text_t text = {21, 20, 16, 19};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(16);
    exp_mapping.emplace_back(19);
    exp_mapping.emplace_back(20);
    exp_mapping.emplace_back(21);

    text_t exp_text = {3, 2, 0, 1};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(full_parallel_mapping, 2322) {
    text_t text = {23, 22};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(22);
    exp_mapping.emplace_back(23);

    text_t exp_text = {1, 0};

    ASSERT_EQ(exp_mapping, mapping);
    ASSERT_EQ(exp_text, text);

}

TEST(full_parallel_mapping, less_productions) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);

    std::vector<var_t> exp_mapping;
    exp_mapping.emplace_back(1);
    exp_mapping.emplace_back(2);
    exp_mapping.emplace_back(3);

    text_t exp_text = {1, 0, 1, 0, 1, 0, 1, 0, 2};

    ASSERT_EQ(exp_mapping, mapping);
    ASSERT_EQ(exp_text, text);
}


TEST(full_parallel_adj_list, left_end) {
    text_t text{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);
//    (2, 1, 1) 0
//    (2, 1, 0) 1
//    (2, 1, 1) 2
//    (2, 1, 0) 3
//    (2, 1, 1) 4
//    (2, 1, 0) 5
//    (2, 1, 1) 6
//    (2, 1, 0) 7
//    (2, 1, 1) 8
//    (2, 1, 1) 11
//    (2, 1, 0) 12
//    (2, 1, 1) 13
//    (2, 1, 0) 14
//    (2, 1, 1) 15
//    (2, 1, 0) 16
//    (2, 1, 1) 17
//    (2, 1, 0) 18
//    (3, 1, 0) 10
//    (3, 1, 1) 19
//    (3, 1, 0) 20
//    (3, 2, 1) 9

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(7);
    exp_adj_list.emplace_back(12);
    exp_adj_list.emplace_back(14);
    exp_adj_list.emplace_back(16);
    exp_adj_list.emplace_back(18);
    exp_adj_list.emplace_back(0);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(8);
    exp_adj_list.emplace_back(11);
    exp_adj_list.emplace_back(13);
    exp_adj_list.emplace_back(15);
    exp_adj_list.emplace_back(17);
    exp_adj_list.emplace_back(10);
    exp_adj_list.emplace_back(20);
    exp_adj_list.emplace_back(19);
    exp_adj_list.emplace_back(9);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_adj_list, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(22);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(20);
    exp_adj_list.emplace_back(21);
    exp_adj_list.emplace_back(7);
    exp_adj_list.emplace_back(16);
    exp_adj_list.emplace_back(10);
    exp_adj_list.emplace_back(13);
    exp_adj_list.emplace_back(19);
    exp_adj_list.emplace_back(8);
    exp_adj_list.emplace_back(17);
    exp_adj_list.emplace_back(9);
    exp_adj_list.emplace_back(18);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(14);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(15);
    exp_adj_list.emplace_back(11);
    exp_adj_list.emplace_back(12);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(2, 1, 0); 0
//    exp_adj_list.emplace_back(2, 1, 1); 1
//    exp_adj_list.emplace_back(2, 1, 0); 2
//    exp_adj_list.emplace_back(2, 1, 0); 22
//    exp_adj_list.emplace_back(3, 1, 1); 20
//    exp_adj_list.emplace_back(3, 2, 1); 7
//    exp_adj_list.emplace_back(3, 2, 1); 16
//    exp_adj_list.emplace_back(3, 2, 0); 21
//    exp_adj_list.emplace_back(4, 1, 0); 10
//    exp_adj_list.emplace_back(4, 1, 0); 13
//    exp_adj_list.emplace_back(4, 1, 0); 19
//    exp_adj_list.emplace_back(5, 3, 1); 8
//    exp_adj_list.emplace_back(5, 3, 1); 17
//    exp_adj_list.emplace_back(5, 4, 0); 9
//    exp_adj_list.emplace_back(5, 4, 0); 18
//    exp_adj_list.emplace_back(6, 1, 1); 5
//    exp_adj_list.emplace_back(6, 1, 1); 14
//    exp_adj_list.emplace_back(6, 2, 0); 6
//    exp_adj_list.emplace_back(6, 2, 0); 15
//    exp_adj_list.emplace_back(7, 1, 1); 11
//    exp_adj_list.emplace_back(7, 4, 0); 12
//    exp_adj_list.emplace_back(8, 1, 1); 3
//    exp_adj_list.emplace_back(8, 1, 0); 4

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_adj_list, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(9, 3, 1);   12
//    exp_adj_list.emplace_back(10, 5, 0);  3
//    exp_adj_list.emplace_back(10, 5, 0);  9
//    exp_adj_list.emplace_back(10, 6, 1);  2
//    exp_adj_list.emplace_back(10, 6, 1);  8
//    exp_adj_list.emplace_back(11, 3, 0);  11
//    exp_adj_list.emplace_back(11, 5, 1);  4
//    exp_adj_list.emplace_back(11, 5, 1);  10
//    exp_adj_list.emplace_back(11, 6, 0);  7
//    exp_adj_list.emplace_back(11, 7, 0);  5
//    exp_adj_list.emplace_back(11, 7, 1);  6
//    exp_adj_list.emplace_back(12, 6, 0);  1
//    exp_adj_list.emplace_back(13, 12, 0); 0

    exp_adj_list.emplace_back(12);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(9);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(8);
    exp_adj_list.emplace_back(11);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(10);
    exp_adj_list.emplace_back(7);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_adj_list, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(15, 14, 0); 5
//    exp_adj_list.emplace_back(16, 15, 0); 1
//    exp_adj_list.emplace_back(16, 15, 0); 4
//    exp_adj_list.emplace_back(17, 15, 1); 2
//    exp_adj_list.emplace_back(17, 16, 0); 3
//    exp_adj_list.emplace_back(18, 16, 0); 0

    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_adj_list, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(19, 16, 1); 2
//    exp_adj_list.emplace_back(20, 16, 0); 1
//    exp_adj_list.emplace_back(21, 20, 0); 0
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_adj_list, 2322) {
    text_t text{23, 22};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(23, 22, 0); 0
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_adj_list, less_productions) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 3};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(7);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(full_parallel_reverse_adj_list, left_end) {
    text_t text{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);
    // (1, 2, 0) 0
    // (1, 2, 0) 2
    // (1, 2, 0) 4
    // (1, 2, 0) 6
    // (1, 2, 0) 8
    // (1, 2, 0) 11
    // (1, 2, 0) 13
    // (1, 2, 0) 15
    // (1, 2, 0) 17
    // (1, 2, 1) 1
    // (1, 2, 1) 3
    // (1, 2, 1) 5
    // (1, 2, 1) 7
    // (1, 2, 1) 12
    // (1, 2, 1) 14
    // (1, 2, 1) 16
    // (1, 2, 1) 18
    // (1, 3, 0) 19
    // (1, 3, 1) 10
    // (1, 3, 1) 20
    // (2, 3, 0) 9

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(8);
    exp_adj_list.emplace_back(11);
    exp_adj_list.emplace_back(13);
    exp_adj_list.emplace_back(15);
    exp_adj_list.emplace_back(17);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(7);
    exp_adj_list.emplace_back(12);
    exp_adj_list.emplace_back(14);
    exp_adj_list.emplace_back(16);
    exp_adj_list.emplace_back(18);
    exp_adj_list.emplace_back(19);
    exp_adj_list.emplace_back(10);
    exp_adj_list.emplace_back(20);
    exp_adj_list.emplace_back(9);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_reverse_adj_list, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);
//    exp_adj_list.emplace_back(1, 2, 0); 1
//    exp_adj_list.emplace_back(1, 2, 1); 0
//    exp_adj_list.emplace_back(1, 2, 1); 2
//    exp_adj_list.emplace_back(1, 2, 1); 22
//    exp_adj_list.emplace_back(1, 3, 0); 20
//    exp_adj_list.emplace_back(1, 4, 1); 10
//    exp_adj_list.emplace_back(1, 4, 1); 13
//    exp_adj_list.emplace_back(1, 4, 1); 19
//    exp_adj_list.emplace_back(1, 6, 0); 5
//    exp_adj_list.emplace_back(1, 6, 0); 14
//    exp_adj_list.emplace_back(1, 7, 0); 11
//    exp_adj_list.emplace_back(1, 8, 0); 3
//    exp_adj_list.emplace_back(1, 8, 1); 4
//    exp_adj_list.emplace_back(2, 3, 0); 7
//    exp_adj_list.emplace_back(2, 3, 0); 16
//    exp_adj_list.emplace_back(2, 3, 1); 21
//    exp_adj_list.emplace_back(2, 6, 1); 6
//    exp_adj_list.emplace_back(2, 6, 1); 15
//    exp_adj_list.emplace_back(3, 5, 0); 8
//    exp_adj_list.emplace_back(3, 5, 0); 17
//    exp_adj_list.emplace_back(4, 5, 1); 9
//    exp_adj_list.emplace_back(4, 5, 1); 18
//    exp_adj_list.emplace_back(4, 7, 1); 12

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(0);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(22);
    exp_adj_list.emplace_back(20);
    exp_adj_list.emplace_back(10);
    exp_adj_list.emplace_back(13);
    exp_adj_list.emplace_back(19);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(14);
    exp_adj_list.emplace_back(11);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(7);
    exp_adj_list.emplace_back(16);
    exp_adj_list.emplace_back(21);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(15);
    exp_adj_list.emplace_back(8);
    exp_adj_list.emplace_back(17);
    exp_adj_list.emplace_back(9);
    exp_adj_list.emplace_back(18);
    exp_adj_list.emplace_back(12);


    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_reverse_adj_list, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(3, 9, 0); 12
//    exp_adj_list.emplace_back(3, 11, 1); 11
//    exp_adj_list.emplace_back(5, 10, 1); 3
//    exp_adj_list.emplace_back(5, 10, 1); 9
//    exp_adj_list.emplace_back(5, 11, 0); 4
//    exp_adj_list.emplace_back(5, 11, 0); 10
//    exp_adj_list.emplace_back(6, 10, 0); 2
//    exp_adj_list.emplace_back(6, 10, 0); 8
//    exp_adj_list.emplace_back(6, 11, 1); 7
//    exp_adj_list.emplace_back(6, 12, 1); 1
//    exp_adj_list.emplace_back(7, 11, 0); 6
//    exp_adj_list.emplace_back(7, 11, 1); 5
//    exp_adj_list.emplace_back(12, 13, 1); 0

    exp_adj_list.emplace_back(12);
    exp_adj_list.emplace_back(11);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(9);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(10);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(8);
    exp_adj_list.emplace_back(7);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_reverse_adj_list, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(15, 14, 0); 5
//    exp_adj_list.emplace_back(16, 15, 0); 1
//    exp_adj_list.emplace_back(16, 15, 0); 4
//    exp_adj_list.emplace_back(17, 15, 1); 2
//    exp_adj_list.emplace_back(17, 16, 0); 3
//    exp_adj_list.emplace_back(18, 16, 0); 0

    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_reverse_adj_list, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(19, 16, 1); 2
//    exp_adj_list.emplace_back(20, 16, 0); 1
//    exp_adj_list.emplace_back(21, 20, 0); 0
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_reverse_adj_list, 2322) {
    text_t text{23, 22};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);

    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(23, 22, 0); 0
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(full_parallel_reverse_adj_list, less_productions) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 3};
    adj_list_t adj_list(text.size() - 1);
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list_reverse(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1);
    exp_adj_list.emplace_back(3);
    exp_adj_list.emplace_back(5);
    exp_adj_list.emplace_back(0);
    exp_adj_list.emplace_back(2);
    exp_adj_list.emplace_back(4);
    exp_adj_list.emplace_back(6);
    exp_adj_list.emplace_back(7);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(full_parallel_recomp, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(full_parallel_recomp, terminal) {
    text_t text = {112};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {112};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 112;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(full_parallel_recomp, short_block2) {
    text_t text = {112, 112};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {113};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.non_terminals.emplace_back(112, 2, 2);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(full_parallel_recomp, short_block3) {
    text_t text = {112, 112, 112};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {113};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.non_terminals.emplace_back(112, 3, 3);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(full_parallel_recomp, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(full_parallel_recomp, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {5};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.non_terminals.emplace_back(5, 4, 16);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(full_parallel_recomp, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, repeated_pair) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, repeated_pair_same_occ) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, left_end) {
    text_t text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, less_productions) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(full_parallel_recomp, pair) {
    text_t text = {112, 111};
    rlslp<var_t, term_t> rlslp;
    parallel::full_parallel_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {113};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.non_terminals.emplace_back(112, 111, 2);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}
