#include <gtest/gtest.h>

#define private public
#define protected public

#include "recompression/parallel_recompression.hpp"
#include "recompression/util.hpp"

using namespace recomp;

typedef parallel::parallel_recompression<var_t>::text_t text_t;
typedef parallel::parallel_recompression<var_t>::adj_t adj_t;
typedef parallel::parallel_recompression<var_t>::adj_list_t adj_list_t;
typedef parallel::parallel_recompression<var_t>::partition_t partition_t;
typedef parallel::parallel_recompression<var_t>::alphabet_t alphabet_t;
typedef parallel::parallel_recompression<var_t>::bv_t bv_t;

TEST(parallel_bcomp, no_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    bv_t exp_bv;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_bcomp, 21214441332311413334133231141321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_bcomp, 222222222222222222222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
//    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 1;
    bv_t exp_bv = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_bcomp, 22222222211111112222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5, 3, 4});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(3);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 3;
    bv_t exp_bv = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_bcomp, 2222222221111111222200) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{6, 4, 5, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{0, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{2, 9, 9};
//    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(parallel_adj_list, left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
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

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13, 14, 15, 16, 17, 18, 10, 19, 20, 9});
//    exp_adj_list.emplace_back(0);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(6);
//    exp_adj_list.emplace_back(7);
//    exp_adj_list.emplace_back(8);
//    exp_adj_list.emplace_back(11);
//    exp_adj_list.emplace_back(12);
//    exp_adj_list.emplace_back(13);
//    exp_adj_list.emplace_back(14);
//    exp_adj_list.emplace_back(15);
//    exp_adj_list.emplace_back(16);
//    exp_adj_list.emplace_back(17);
//    exp_adj_list.emplace_back(18);
//    exp_adj_list.emplace_back(10);
//    exp_adj_list.emplace_back(19);
//    exp_adj_list.emplace_back(20);
//    exp_adj_list.emplace_back(9);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0, 1, 2, 22, 20, 7, 16, 21, 10, 13, 19, 8, 17, 9, 18, 5, 14, 6, 15, 11, 12, 3, 4});
//    exp_adj_list.emplace_back(0);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(22);
//    exp_adj_list.emplace_back(20);
//    exp_adj_list.emplace_back(7);
//    exp_adj_list.emplace_back(16);
//    exp_adj_list.emplace_back(21);
//    exp_adj_list.emplace_back(10);
//    exp_adj_list.emplace_back(13);
//    exp_adj_list.emplace_back(19);
//    exp_adj_list.emplace_back(8);
//    exp_adj_list.emplace_back(17);
//    exp_adj_list.emplace_back(9);
//    exp_adj_list.emplace_back(18);
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(14);
//    exp_adj_list.emplace_back(6);
//    exp_adj_list.emplace_back(15);
//    exp_adj_list.emplace_back(11);
//    exp_adj_list.emplace_back(12);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(4);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{12, 3, 9, 2, 8, 11, 4, 10, 7, 5, 6, 1, 0});

//    exp_adj_list.emplace_back(12);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(9);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(8);
//    exp_adj_list.emplace_back(11);
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(10);
//    exp_adj_list.emplace_back(7);
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(6);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{5, 1, 4, 2, 3, 0});
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{2, 1, 0});
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0});
//    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0, 1, 2, 3, 4, 5, 6, 7});
//    exp_adj_list.emplace_back(0);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(6);
//    exp_adj_list.emplace_back(7);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(parallel_partition, less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    alphabet_t alphabet{1, 2, 3};
    partition_t partition(3);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 1);

    partition_t exp_partition(3);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[1] = false;
//    exp_partition[2] = true;
//    exp_partition[3] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    exp_partition[2] = true;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    alphabet_t alphabet{1, 2, 3};
    partition_t partition(3);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 1);

    partition_t exp_partition(3);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[1] = false;
//    exp_partition[2] = true;
//    exp_partition[3] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    exp_partition[2] = true;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, repreated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    alphabet_t alphabet{1, 2};
    partition_t partition(2);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 1);

    partition_t exp_partition(2);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[1] = false;
//    exp_partition[2] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, repreated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    alphabet_t alphabet{1, 2};
    partition_t partition(2);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 1);

    partition_t exp_partition(2);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[1] = false;
//    exp_partition[2] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    partition_t partition(8);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 1);

    partition_t exp_partition(8);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[1] = false;
//    exp_partition[2] = true;
//    exp_partition[3] = false;
//    exp_partition[4] = true;
//    exp_partition[5] = false;
//    exp_partition[6] = false;
//    exp_partition[7] = false;
//    exp_partition[8] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    exp_partition[2] = false;
    exp_partition[3] = true;
    exp_partition[4] = false;
    exp_partition[5] = false;
    exp_partition[6] = false;
    exp_partition[7] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    partition_t partition(11);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 3);

    partition_t exp_partition(11);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[3] = false;
//    exp_partition[5] = false;
//    exp_partition[6] = false;
//    exp_partition[7] = false;
//    exp_partition[9] = true;
//    exp_partition[10] = true;
//    exp_partition[11] = true;
//    exp_partition[12] = true;
//    exp_partition[13] = false;
    exp_partition[0] = false;
    exp_partition[2] = false;
    exp_partition[3] = false;
    exp_partition[4] = false;
    exp_partition[6] = true;
    exp_partition[7] = true;
    exp_partition[8] = true;
    exp_partition[9] = true;
    exp_partition[10] = false;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    alphabet_t alphabet{14, 15, 16, 17, 18};
    partition_t partition(5);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 14);

    partition_t exp_partition(5);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[14] = false;
//    exp_partition[15] = true;
//    exp_partition[16] = false;
//    exp_partition[17] = false;
//    exp_partition[18] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    exp_partition[2] = false;
    exp_partition[3] = false;
    exp_partition[4] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    alphabet_t alphabet{16, 19, 20, 21};
    partition_t partition(6);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 16);

    partition_t exp_partition(6);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[16] = false;
//    exp_partition[19] = true;
//    exp_partition[20] = true;
//    exp_partition[21] = false;
    exp_partition[0] = false;
    exp_partition[3] = true;
    exp_partition[4] = true;
    exp_partition[5] = false;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_partition, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    alphabet_t alphabet{22, 23};
    partition_t partition(2);
    bool part_l = false;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_partition(text, partition, part_l, 22);

    partition_t exp_partition(2);
    for (size_t i = 0; i < exp_partition.size(); ++i) {
        exp_partition[i] = false;
    }
//    exp_partition[22] = false;
//    exp_partition[23] = true;
    exp_partition[0] = false;
    exp_partition[1] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}


TEST(parallel_pcomp, less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 4;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 4, 4, 4, 5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 4;
    exp_rlslp.root = 0;
    exp_rlslp.resize(2);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(1, 3, 2);
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 4;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 5, 1});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 4;
    exp_rlslp.root = 0;
    exp_rlslp.resize(2);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(1, 3, 2);
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
//    rlslp.non_terminals.emplace_back(1, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 2, 2);
//    rlslp.non_terminals.emplace_back(3, 3, 3);
//    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.blocks = 4;
    bv_t bv = {true, true, true, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9, 9, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(8);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
//    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(8, 1, 4);
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(9);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
//    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(14);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(14);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(17);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{15, 14, 7};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(17);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    rlslp.non_terminals[14] = non_terminal<var_t>{15, 14, 7};
    rlslp.non_terminals[15] = non_terminal<var_t>{15, 17, 9};
    rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{23, 22});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(19);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{15, 14, 7};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{16, 19, 11};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{21, 20, 21};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_pcomp, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;

    rlslp.resize(19);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    rlslp.non_terminals[14] = non_terminal<var_t>{15, 14, 7};
    rlslp.non_terminals[15] = non_terminal<var_t>{15, 17, 9};
    rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    rlslp.non_terminals[17] = non_terminal<var_t>{16, 19, 11};
    rlslp.non_terminals[18] = non_terminal<var_t>{21, 20, 21};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{24});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(20);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{15, 14, 7};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{16, 19, 11};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{21, 20, 21};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{23, 22, 32};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(parallel_recomp, empty) {
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, terminal) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{112});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 112;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, short_block2) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 2, 2};
//    exp_rlslp.non_terminals.emplace_back(112, 2, 2);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, short_block3) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112, 112});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 3, 3};
//    exp_rlslp.non_terminals.emplace_back(112, 3, 3);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, recompression) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{24});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 19;
    exp_rlslp.resize(20);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{23, 1, 4};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{3, 5, 3};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{20, 7, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{21, 6, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{22, 7, 5};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{24, 8, 8};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{10, 9, 7};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{10, 12, 9};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{13, 11, 12};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{11, 14, 11};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{16, 15, 21};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{18, 17, 32};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{5, 2, 4};
    exp_rlslp.blocks = 15;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, one_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
//    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 111});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 111, 2};
//    exp_rlslp.non_terminals.emplace_back(112, 111, 2);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, two_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(3);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 4, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
//    exp_rlslp.non_terminals.emplace_back(5, 4, 16);
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, three_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{7});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(5);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 6, 11};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{7, 3, 20};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 9, 9};
//    exp_rlslp.non_terminals.emplace_back(5, 6, 11);
//    exp_rlslp.non_terminals.emplace_back(7, 3, 20);
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, four_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 5;
    exp_rlslp.resize(7);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{8, 6, 6};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{9, 7, 16};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{4, 3, 22};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{0, 2, 2};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 9, 9};
//    exp_rlslp.non_terminals.emplace_back(8, 6, 6);
//    exp_rlslp.non_terminals.emplace_back(9, 7, 16);
//    exp_rlslp.non_terminals.emplace_back(4, 3, 22);
//    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 3;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{4});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(2);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 11, 22};
//    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
//    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(3);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 5, 23};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 11, 22};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
//    exp_rlslp.non_terminals.emplace_back(2, 5, 23);
//    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{11});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 4;
    exp_rlslp.root = 9;
    exp_rlslp.resize(8);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{5, 1, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{11, 3, 11};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{10, 6, 11};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{7, 8, 22};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 4, 8};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 5, 10};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);  // 4    4
//    exp_rlslp.non_terminals.emplace_back(1, 3, 2);  // 5    5
//    exp_rlslp.non_terminals.emplace_back(5, 1, 3);  // 8    6
//    exp_rlslp.non_terminals.emplace_back(11, 3, 11);  // 9   7
//    exp_rlslp.non_terminals.emplace_back(10, 6, 11);  // 10  8
//    exp_rlslp.non_terminals.emplace_back(7, 8, 22);  // 11 9
//    exp_rlslp.non_terminals.emplace_back(4, 4, 8);  // 6    10
//    exp_rlslp.non_terminals.emplace_back(4, 5, 10); // 7    11

    exp_rlslp.blocks = 6;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_recomp, less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    // 2, 4, 4, 4, 5
    // 2, 6, 5
    // 7, 5
    // 8

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{8});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 4;
    exp_rlslp.root = 7;
    exp_rlslp.resize(5);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 3, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 8, 7};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{6, 5, 9};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{4, 3, 6};
//    exp_rlslp.non_terminals.emplace_back(1, 2, 2);  // 4    4
//    exp_rlslp.non_terminals.emplace_back(1, 3, 2);  // 5    5
//    exp_rlslp.non_terminals.emplace_back(2, 8, 7);  // 7    6
//    exp_rlslp.non_terminals.emplace_back(6, 5, 9);  // 8    7
//    exp_rlslp.non_terminals.emplace_back(4, 3, 6);  // 6    8
    exp_rlslp.blocks = 4;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}