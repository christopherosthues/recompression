#include <gtest/gtest.h>

#define private public

#include "parallel_rnd_recompression.hpp"

using namespace recomp;

typedef parallel::parallel_rnd_recompression<var_t, term_t>::text_t text_t;
typedef parallel::parallel_rnd_recompression<var_t, term_t>::adj_list_t adj_list_t;
typedef parallel::parallel_rnd_recompression<var_t, term_t>::partition_t partition_t;
typedef parallel::parallel_rnd_recompression<var_t, term_t>::alphabet_t alphabet_t;
typedef parallel::parallel_rnd_recompression<var_t, term_t>::bv_t bv_t;

TEST(parallel_rnd_bcomp, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_bcomp, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_bcomp, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_bcomp, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_bcomp, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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


TEST(parallel_rnd_mapping, left_end) {
    text_t text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_mapping, 212181623541741623541321) {
    text_t text = {2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_mapping, 131261051171161051139) {
    text_t text = {13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_mapping, 18161517161514) {
    text_t text = {18, 16, 15, 17, 16, 15, 14};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_mapping, 21201619) {
    text_t text = {21, 20, 16, 19};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_mapping, 2322) {
    text_t text = {23, 22};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_mapping, less_productions) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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


TEST(parallel_rnd_adj_list, left_end) {
    text_t text{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);
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

TEST(parallel_rnd_adj_list, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);
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

TEST(parallel_rnd_adj_list, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);
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

TEST(parallel_rnd_adj_list, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);
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

TEST(parallel_rnd_adj_list, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);
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

TEST(parallel_rnd_adj_list, 2322) {
    text_t text{23, 22};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    std::vector<var_t> mapping;
    recomp.compute_mapping(text, mapping);
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_rnd_adj_list, less_productions) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 3};
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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


//TEST(parallel_rnd_partition, less_productions) {
//    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 3};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{1, 2, 3};
//
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_adj_list(text, adj_list);
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[1] = false;
////    exp_partition[2] = true;
////    exp_partition[3] = true;
//    bool exp_part_l = true;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, left_end) {
//    text_t text{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{1, 2, 3};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[1] = false;
////    exp_partition[2] = true;
////    exp_partition[3] = true;
//    bool exp_part_l = false;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, repreated_pair) {
//    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{1, 2};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[1] = false;
////    exp_partition[2] = true;
//    bool exp_part_l = true;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, repreated_pair_same_occ) {
//    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{1, 2};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[1] = false;
////    exp_partition[2] = true;
//    bool exp_part_l = false;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, 212181623541741623541321) {
//    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[1] = false;
////    exp_partition[2] = true;
////    exp_partition[3] = false;
////    exp_partition[4] = true;
////    exp_partition[5] = false;
////    exp_partition[6] = false;
////    exp_partition[7] = false;
////    exp_partition[8] = true;
//    bool exp_part_l = true;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, 131261051171161051139) {
//    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[3] = false;
////    exp_partition[5] = false;
////    exp_partition[6] = false;
////    exp_partition[7] = false;
////    exp_partition[9] = true;
////    exp_partition[10] = true;
////    exp_partition[11] = true;
////    exp_partition[12] = true;
////    exp_partition[13] = false;
//    bool exp_part_l = false;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, 18161517161514) {
//    text_t text{18, 16, 15, 17, 16, 15, 14};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{14, 15, 16, 17, 18};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[14] = false;
////    exp_partition[15] = true;
////    exp_partition[16] = false;
////    exp_partition[17] = false;
////    exp_partition[18] = true;
//    bool exp_part_l = true;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, 21201619) {
//    text_t text{21, 20, 16, 19};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{16, 19, 20, 21};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[16] = false;
////    exp_partition[19] = true;
////    exp_partition[20] = true;
////    exp_partition[21] = false;
//    bool exp_part_l = false;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}
//
//TEST(parallel_rnd_partition, 2322) {
//    text_t text{23, 22};
//    adj_list_t adj_list(text.size() - 1);
//    alphabet_t alphabet{22, 23};
//    bool part_l = false;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    recomp.compute_adj_list(text, adj_list);
//    std::vector<var_t> mapping;
//    recomp.compute_mapping(text, mapping);
//    partition_t partition(mapping.size());
//    recomp.compute_partition(text, adj_list, partition, part_l);
//
//    partition_t exp_partition;
////    exp_partition[22] = false;
////    exp_partition[23] = true;
//    bool exp_part_l = true;
//
//    ASSERT_EQ(exp_partition, partition);
//    ASSERT_EQ(exp_part_l, part_l);
//}


//TEST(parallel_rnd_pcomp, less_productions) {
//    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 3};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 4;
//    bv_t bv;
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{4, 4, 4, 4, 3};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 4;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.blocks = 0;
//    bv_t exp_bv{false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, repeated_pair) {
//    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 3;
//    bv_t bv;
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 3;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.blocks = 0;
//    bv_t exp_bv{false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, repeated_pair_same_occ) {
//    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 3;
//    bv_t bv;
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 3;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.blocks = 0;
//    bv_t exp_bv{false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, left_end) {
//    text_t text{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 4;
//    bv_t bv;
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 5, 1};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 4;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(1, 3, 2);
//    exp_rlslp.blocks = 0;
//    bv_t exp_bv{false, false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, 212181623541741623541321) {
//    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 5;
//    rlslp.non_terminals.emplace_back(1, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 3, 3);
//    rlslp.non_terminals.emplace_back(4, 3, 3);
//    rlslp.blocks = 4;
//    bv_t bv{true, true, true, true};
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{9, 9, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 5;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
//    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
//    exp_rlslp.blocks = 4;
//    bv_t exp_bv{true, true, true, true, false, false, false, false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, 131261051171161051139) {
//    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 5;
//    rlslp.non_terminals.emplace_back(1, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 3, 3);
//    rlslp.non_terminals.emplace_back(4, 3, 3);
//    rlslp.non_terminals.emplace_back(2, 1, 2);
//    rlslp.non_terminals.emplace_back(2, 3, 2);
//    rlslp.non_terminals.emplace_back(4, 1, 2);
//    rlslp.non_terminals.emplace_back(8, 1, 4);
//    rlslp.non_terminals.emplace_back(9, 2, 4);
//    rlslp.blocks = 5;
//    bv_t bv{true, true, true, true, false, false, false, false, true};
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{18, 16, 15, 17, 16, 15, 14};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 5;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
//    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
//    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
//    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
//    exp_rlslp.non_terminals.emplace_back(5, 11, 4);
//    exp_rlslp.non_terminals.emplace_back(6, 10, 4);
//    exp_rlslp.non_terminals.emplace_back(7, 11, 5);
//    exp_rlslp.non_terminals.emplace_back(13, 12, 8);
//    exp_rlslp.blocks = 5;
//    bv_t exp_bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, 18161517161514) {
//    text_t text{18, 16, 15, 17, 16, 15, 14};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 5;
//    rlslp.non_terminals.emplace_back(1, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 3, 3);
//    rlslp.non_terminals.emplace_back(4, 3, 3);
//    rlslp.non_terminals.emplace_back(2, 1, 2);
//    rlslp.non_terminals.emplace_back(2, 3, 2);
//    rlslp.non_terminals.emplace_back(4, 1, 2);
//    rlslp.non_terminals.emplace_back(8, 1, 4);
//    rlslp.non_terminals.emplace_back(9, 2, 4);
//    rlslp.non_terminals.emplace_back(3, 9, 3);
//    rlslp.non_terminals.emplace_back(5, 11, 4);
//    rlslp.non_terminals.emplace_back(6, 10, 4);
//    rlslp.non_terminals.emplace_back(7, 11, 5);
//    rlslp.non_terminals.emplace_back(13, 12, 8);
//    rlslp.blocks = 5;
//    bv_t bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false};
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{21, 20, 16, 19};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 5;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
//    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
//    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
//    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
//    exp_rlslp.non_terminals.emplace_back(5, 11, 4);
//    exp_rlslp.non_terminals.emplace_back(6, 10, 4);
//    exp_rlslp.non_terminals.emplace_back(7, 11, 5);
//    exp_rlslp.non_terminals.emplace_back(13, 12, 8);
//    exp_rlslp.non_terminals.emplace_back(15, 14, 7);
//    exp_rlslp.non_terminals.emplace_back(15, 17, 9);
//    exp_rlslp.non_terminals.emplace_back(18, 16, 12);
//    exp_rlslp.blocks = 5;
//    bv_t exp_bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
//                false, false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, 21201619) {
//    text_t text{21, 20, 16, 19};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 5;
//    rlslp.non_terminals.emplace_back(1, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 3, 3);
//    rlslp.non_terminals.emplace_back(4, 3, 3);
//    rlslp.non_terminals.emplace_back(2, 1, 2);
//    rlslp.non_terminals.emplace_back(2, 3, 2);
//    rlslp.non_terminals.emplace_back(4, 1, 2);
//    rlslp.non_terminals.emplace_back(8, 1, 4);
//    rlslp.non_terminals.emplace_back(9, 2, 4);
//    rlslp.non_terminals.emplace_back(3, 9, 3);
//    rlslp.non_terminals.emplace_back(5, 11, 4);
//    rlslp.non_terminals.emplace_back(6, 10, 4);
//    rlslp.non_terminals.emplace_back(7, 11, 5);
//    rlslp.non_terminals.emplace_back(13, 12, 8);
//    rlslp.non_terminals.emplace_back(15, 14, 7);
//    rlslp.non_terminals.emplace_back(15, 17, 9);
//    rlslp.non_terminals.emplace_back(18, 16, 12);
//    rlslp.blocks = 5;
//    bv_t bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
//            false};
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{23, 22};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 5;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
//    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
//    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
//    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
//    exp_rlslp.non_terminals.emplace_back(5, 11, 4);
//    exp_rlslp.non_terminals.emplace_back(6, 10, 4);
//    exp_rlslp.non_terminals.emplace_back(7, 11, 5);
//    exp_rlslp.non_terminals.emplace_back(13, 12, 8);
//    exp_rlslp.non_terminals.emplace_back(15, 14, 7);
//    exp_rlslp.non_terminals.emplace_back(15, 17, 9);
//    exp_rlslp.non_terminals.emplace_back(18, 16, 12);
//    exp_rlslp.non_terminals.emplace_back(16, 19, 11);
//    exp_rlslp.non_terminals.emplace_back(21, 20, 21);
//    exp_rlslp.blocks = 5;
//    bv_t exp_bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
//                false, false, false, false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}
//
//TEST(parallel_rnd_pcomp, 2322) {
//    text_t text{23, 22};
//    rlslp<var_t, term_t> rlslp;
//    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    rlslp.terminals = 5;
//    rlslp.non_terminals.emplace_back(1, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 3, 3);
//    rlslp.non_terminals.emplace_back(4, 3, 3);
//    rlslp.non_terminals.emplace_back(2, 1, 2);
//    rlslp.non_terminals.emplace_back(2, 3, 2);
//    rlslp.non_terminals.emplace_back(4, 1, 2);
//    rlslp.non_terminals.emplace_back(8, 1, 4);
//    rlslp.non_terminals.emplace_back(9, 2, 4);
//    rlslp.non_terminals.emplace_back(3, 9, 3);
//    rlslp.non_terminals.emplace_back(5, 11, 4);
//    rlslp.non_terminals.emplace_back(6, 10, 4);
//    rlslp.non_terminals.emplace_back(7, 11, 5);
//    rlslp.non_terminals.emplace_back(13, 12, 8);
//    rlslp.non_terminals.emplace_back(15, 14, 7);
//    rlslp.non_terminals.emplace_back(15, 17, 9);
//    rlslp.non_terminals.emplace_back(18, 16, 12);
//    rlslp.non_terminals.emplace_back(16, 19, 11);
//    rlslp.non_terminals.emplace_back(21, 20, 21);
//    rlslp.blocks = 5;
//    bv_t bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
//            false, false, false};
//    recomp.pcomp(text, rlslp, bv);
//
//    text_t exp_text{24};
//    recomp::rlslp<var_t, term_t> exp_rlslp;
//    exp_rlslp.terminals = 5;
//    exp_rlslp.root = 0;
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
//    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
//    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
//    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
//    exp_rlslp.non_terminals.emplace_back(5, 11, 4);
//    exp_rlslp.non_terminals.emplace_back(6, 10, 4);
//    exp_rlslp.non_terminals.emplace_back(7, 11, 5);
//    exp_rlslp.non_terminals.emplace_back(13, 12, 8);
//    exp_rlslp.non_terminals.emplace_back(15, 14, 7);
//    exp_rlslp.non_terminals.emplace_back(15, 17, 9);
//    exp_rlslp.non_terminals.emplace_back(18, 16, 12);
//    exp_rlslp.non_terminals.emplace_back(16, 19, 11);
//    exp_rlslp.non_terminals.emplace_back(21, 20, 21);
//    exp_rlslp.non_terminals.emplace_back(23, 22, 32);
//    exp_rlslp.blocks = 5;
//    bv_t exp_bv{true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
//                false, false, false, false, false};
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_rlslp, rlslp);
//    ASSERT_EQ(exp_bv, bv);
//}


TEST(parallel_rnd_recomp, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_recomp, terminal) {
    text_t text = {112};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_recomp, short_block2) {
    text_t text = {112, 112};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_recomp, short_block3) {
    text_t text = {112, 112, 112};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_recomp, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_recomp, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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

TEST(parallel_rnd_recomp, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, repeated_pair) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, repeated_pair_same_occ) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, left_end) {
    text_t text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, less_productions) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_rnd_recomp, pair) {
    text_t text = {112, 111};
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_rnd_recompression<var_t, term_t> recomp;
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
