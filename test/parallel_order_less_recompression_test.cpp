#include <gtest/gtest.h>

#define private public

#include "recompression/experimental/parallel_order_less_recompression.hpp"

using namespace recomp;

typedef parallel::recompression_order_ls<var_t>::text_t text_t;
typedef parallel::recompression_order_ls<var_t>::adj_list_t adj_list_t;
typedef parallel::recompression_order_ls<var_t>::partition_t partition_t;
typedef parallel::recompression_order_ls<var_t>::alphabet_t alphabet_t;
typedef parallel::recompression_order_ls<var_t>::bv_t bv_t;

TEST(parallel_order_ls_bcomp, no_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
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

TEST(parallel_order_ls_bcomp, 21214441332311413334133231141321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_bcomp, 222222222222222222222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
    exp_rlslp.blocks = 1;
    bv_t exp_bv = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_bcomp, 22222222211111112222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5, 3, 4});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(3, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.blocks = 3;
    bv_t exp_bv = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_bcomp, 2222222221111111222200) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{6, 4, 5, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{0, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(parallel_order_ls_adj_list, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0, 1, 2);
    exp_adj_list.emplace_back(0, 1, 3);
    exp_adj_list.emplace_back(0, 1, 6);
    exp_adj_list.emplace_back(0, 1, 6);
    exp_adj_list.emplace_back(0, 1, 7);
    exp_adj_list.emplace_back(0, 1, 8);
    exp_adj_list.emplace_back(0, 2, 3);
    exp_adj_list.emplace_back(0, 2, 3);
    exp_adj_list.emplace_back(0, 3, 5);
    exp_adj_list.emplace_back(0, 3, 5);
    exp_adj_list.emplace_back(1, 1, 2);
    exp_adj_list.emplace_back(1, 1, 2);
    exp_adj_list.emplace_back(1, 1, 2);
    exp_adj_list.emplace_back(1, 1, 4);
    exp_adj_list.emplace_back(1, 1, 4);
    exp_adj_list.emplace_back(1, 1, 4);
    exp_adj_list.emplace_back(1, 1, 8);
    exp_adj_list.emplace_back(1, 2, 3);
    exp_adj_list.emplace_back(1, 2, 6);
    exp_adj_list.emplace_back(1, 2, 6);
    exp_adj_list.emplace_back(1, 4, 5);
    exp_adj_list.emplace_back(1, 4, 5);
    exp_adj_list.emplace_back(1, 4, 7);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_order_ls_adj_list, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0, 3, 9);
    exp_adj_list.emplace_back(0, 5, 11);
    exp_adj_list.emplace_back(0, 5, 11);
    exp_adj_list.emplace_back(0, 6, 10);
    exp_adj_list.emplace_back(0, 6, 10);
    exp_adj_list.emplace_back(0, 7, 11);
    exp_adj_list.emplace_back(1, 3, 11);
    exp_adj_list.emplace_back(1, 5, 10);
    exp_adj_list.emplace_back(1, 5, 10);
    exp_adj_list.emplace_back(1, 6, 11);
    exp_adj_list.emplace_back(1, 6, 12);
    exp_adj_list.emplace_back(1, 7, 11);
    exp_adj_list.emplace_back(1, 12, 13);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_order_ls_adj_list, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0, 15, 17);
    exp_adj_list.emplace_back(1, 14, 15);
    exp_adj_list.emplace_back(1, 15, 16);
    exp_adj_list.emplace_back(1, 15, 16);
    exp_adj_list.emplace_back(1, 16, 17);
    exp_adj_list.emplace_back(1, 16, 18);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_order_ls_adj_list, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0, 16, 19);
    exp_adj_list.emplace_back(1, 16, 20);
    exp_adj_list.emplace_back(1, 20, 21);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_order_ls_adj_list, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1, 22, 23);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(parallel_order_ls_partition, repreated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_ls_partition, repreated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_ls_partition, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    exp_partition[3] = false;
    exp_partition[4] = true;
    exp_partition[5] = false;
    exp_partition[6] = false;
    exp_partition[7] = false;
    exp_partition[8] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_ls_partition, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[3] = false;
    exp_partition[5] = false;
    exp_partition[6] = false;
    exp_partition[7] = false;
    exp_partition[9] = true;
    exp_partition[10] = true;
    exp_partition[11] = true;
    exp_partition[12] = true;
    exp_partition[13] = false;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_ls_partition, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{14, 15, 16, 17, 18};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[14] = false;
    exp_partition[15] = true;
    exp_partition[16] = false;
    exp_partition[17] = false;
    exp_partition[18] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_ls_partition, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{16, 19, 20, 21};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_ls_partition, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{22, 23};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    recomp.compute_partition(adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[22] = false;
    exp_partition[23] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}


TEST(parallel_order_ls_pcomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_pcomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_pcomp, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(4, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.blocks = 4;
    bv_t bv = {true, true, true, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{10, 10, 12, 6, 9, 5, 11, 7, 11, 6, 9, 5, 11, 3, 10});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(8, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_pcomp, 13126951171169511310) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 9, 5, 11, 7, 11, 6, 9, 5, 11, 3, 10});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(9, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(14, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_pcomp, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(14, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{21, 19, 16, 20});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(17, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{15, 14, 7};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_pcomp, 21191620) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 19, 16, 20});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(17, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    rlslp.non_terminals[14] = non_terminal<var_t>{15, 17, 9};
    rlslp.non_terminals[15] = non_terminal<var_t>{15, 14, 7};
    rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{23, 22});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(19, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{15, 14, 7};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{16, 20, 11};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{21, 19, 21};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_ls_pcomp, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(19, 4);
    rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    rlslp.non_terminals[14] = non_terminal<var_t>{15, 17, 9};
    rlslp.non_terminals[15] = non_terminal<var_t>{15, 14, 7};
    rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    rlslp.non_terminals[17] = non_terminal<var_t>{16, 20, 11};
    rlslp.non_terminals[18] = non_terminal<var_t>{21, 19, 21};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{24});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(20, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{8, 1, 4};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{10, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{3, 10, 3};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{5, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{6, 9, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{7, 11, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{13, 12, 8};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{15, 17, 9};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{15, 14, 7};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{18, 16, 12};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{16, 20, 11};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{21, 19, 21};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{23, 22, 32};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(parallel_order_ls_recomp, empty) {
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
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

TEST(parallel_order_ls_recomp, terminal) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
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

TEST(parallel_order_ls_recomp, short_block2) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 2, 2};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, short_block3) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112, 112});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 3, 3};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, recompression) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{24});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 19;
    exp_rlslp.resize(20, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{23, 1, 4};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{3, 6, 3};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{20, 7, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{21, 5, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{22, 7, 5};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{24, 8, 8};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{10, 12, 9};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{10, 9, 7};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{13, 11, 12};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{11, 15, 11};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{16, 14, 21};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{18, 17, 32};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{6, 2, 4};
    exp_rlslp.blocks = 15;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, one_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, two_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(3, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 4, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, three_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{7});
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

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, four_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 5;
    exp_rlslp.resize(7, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{8, 6, 6};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{9, 7, 16};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{4, 3, 22};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{0, 2, 2};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.blocks = 3;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{4});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(2, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 11, 22};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(3, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 5, 23};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 11, 22};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_ls_recomp, pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 111});
    rlslp<var_t> rlslp;
    parallel::recompression_order_ls<var_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 111, 2};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}
