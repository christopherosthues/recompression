#include <gtest/gtest.h>

#define private public

#include "recompression/parallel_order_great_recompression.hpp"

using namespace recomp;

typedef parallel::recompression_order_gr<var_t, term_t>::text_t text_t;
typedef parallel::recompression_order_gr<var_t, term_t>::adj_list_t adj_list_t;
typedef parallel::recompression_order_gr<var_t, term_t>::partition_t partition_t;
typedef parallel::recompression_order_gr<var_t, term_t>::alphabet_t alphabet_t;
typedef parallel::recompression_order_gr<var_t, term_t>::bv_t bv_t;

TEST(parallel_order_gr_bcomp, no_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    bv_t exp_bv;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_bcomp, 21214441332311413334133231141321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_bcomp, 222222222222222222222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{2, 21, 21};
    exp_rlslp.blocks = 1;
    bv_t exp_bv = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_bcomp, 22222222211111112222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5, 3, 4});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(3);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 7, 7};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{2, 4, 4};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{2, 9, 9};
    exp_rlslp.blocks = 3;
    bv_t exp_bv = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_bcomp, 2222222221111111222200) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{6, 4, 5, 3});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{0, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{2, 4, 4};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{2, 9, 9};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(parallel_order_gr_adj_list, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

//    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(0, 2, 1);
//    exp_adj_list.emplace_back(1, 2, 1);
//    exp_adj_list.emplace_back(0, 2, 1);
//    exp_adj_list.emplace_back(1, 8, 1);
//    exp_adj_list.emplace_back(0, 8, 1);
//    exp_adj_list.emplace_back(1, 6, 1);
//    exp_adj_list.emplace_back(0, 6, 2);
//    exp_adj_list.emplace_back(1, 3, 2);
//    exp_adj_list.emplace_back(1, 5, 3);
//    exp_adj_list.emplace_back(0, 5, 4);
//    exp_adj_list.emplace_back(0, 4, 1);
//    exp_adj_list.emplace_back(1, 7, 1);
//    exp_adj_list.emplace_back(0, 7, 4);
//    exp_adj_list.emplace_back(0, 4, 1);
//    exp_adj_list.emplace_back(1, 6, 1);
//    exp_adj_list.emplace_back(0, 6, 2);
//    exp_adj_list.emplace_back(1, 3, 2);
//    exp_adj_list.emplace_back(1, 5, 3);
//    exp_adj_list.emplace_back(0, 5, 4);
//    exp_adj_list.emplace_back(0, 4, 1);
//    exp_adj_list.emplace_back(1, 3, 1);
//    exp_adj_list.emplace_back(0, 3, 2);
//    exp_adj_list.emplace_back(0, 2, 1);
    size_t exp_begin = 13;

//    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_begin, begin);

//    ips4o::parallel::sort(adj_list.begin(), adj_list.end());

    adj_list_t exp_sort_adj_list;
    exp_sort_adj_list.emplace_back(0, 2, 1);
    exp_sort_adj_list.emplace_back(0, 2, 1);
    exp_sort_adj_list.emplace_back(0, 2, 1);
    exp_sort_adj_list.emplace_back(0, 3, 2);
    exp_sort_adj_list.emplace_back(0, 4, 1);
    exp_sort_adj_list.emplace_back(0, 4, 1);
    exp_sort_adj_list.emplace_back(0, 4, 1);
    exp_sort_adj_list.emplace_back(0, 5, 4);
    exp_sort_adj_list.emplace_back(0, 5, 4);
    exp_sort_adj_list.emplace_back(0, 6, 2);
    exp_sort_adj_list.emplace_back(0, 6, 2);
    exp_sort_adj_list.emplace_back(0, 7, 4);
    exp_sort_adj_list.emplace_back(0, 8, 1);

    exp_sort_adj_list.emplace_back(1, 2, 1);
    exp_sort_adj_list.emplace_back(1, 3, 1);
    exp_sort_adj_list.emplace_back(1, 3, 2);
    exp_sort_adj_list.emplace_back(1, 3, 2);
    exp_sort_adj_list.emplace_back(1, 5, 3);
    exp_sort_adj_list.emplace_back(1, 5, 3);
    exp_sort_adj_list.emplace_back(1, 6, 1);
    exp_sort_adj_list.emplace_back(1, 6, 1);
    exp_sort_adj_list.emplace_back(1, 7, 1);
    exp_sort_adj_list.emplace_back(1, 8, 1);

    ASSERT_EQ(exp_sort_adj_list, adj_list);
}

TEST(parallel_order_gr_adj_list, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

//    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(0, 13, 12);
//    exp_adj_list.emplace_back(0, 12, 6);
//    exp_adj_list.emplace_back(1, 10, 6);
//    exp_adj_list.emplace_back(0, 10, 5);
//    exp_adj_list.emplace_back(1, 11, 5);
//    exp_adj_list.emplace_back(0, 11, 7);
//    exp_adj_list.emplace_back(1, 11, 7);
//    exp_adj_list.emplace_back(0, 11, 6);
//    exp_adj_list.emplace_back(1, 10, 6);
//    exp_adj_list.emplace_back(0, 10, 5);
//    exp_adj_list.emplace_back(1, 11, 5);
//    exp_adj_list.emplace_back(0, 11, 3);
//    exp_adj_list.emplace_back(1, 9, 3);
    size_t exp_begin = 7;

//    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_begin, begin);

//    ips4o::parallel::sort(adj_list.begin(), adj_list.end());

    adj_list_t exp_sort_adj_list;
    exp_sort_adj_list.emplace_back(0, 10, 5);
    exp_sort_adj_list.emplace_back(0, 10, 5);
    exp_sort_adj_list.emplace_back(0, 11, 3);
    exp_sort_adj_list.emplace_back(0, 11, 6);
    exp_sort_adj_list.emplace_back(0, 11, 7);
    exp_sort_adj_list.emplace_back(0, 12, 6);
    exp_sort_adj_list.emplace_back(0, 13, 12);

    exp_sort_adj_list.emplace_back(1, 9, 3);
    exp_sort_adj_list.emplace_back(1, 10, 6);
    exp_sort_adj_list.emplace_back(1, 10, 6);
    exp_sort_adj_list.emplace_back(1, 11, 5);
    exp_sort_adj_list.emplace_back(1, 11, 5);
    exp_sort_adj_list.emplace_back(1, 11, 7);

    ASSERT_EQ(exp_sort_adj_list, adj_list);
}

TEST(parallel_order_gr_adj_list, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

//    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(0, 18, 16);
//    exp_adj_list.emplace_back(0, 16, 15);
//    exp_adj_list.emplace_back(1, 17, 15);
//    exp_adj_list.emplace_back(0, 17, 16);
//    exp_adj_list.emplace_back(0, 16, 15);
//    exp_adj_list.emplace_back(0, 15, 14);
    size_t exp_begin = 5;

//    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_begin, begin);

//    ips4o::parallel::sort(adj_list.begin(), adj_list.end());

    adj_list_t exp_sort_adj_list;
    exp_sort_adj_list.emplace_back(0, 15, 14);
    exp_sort_adj_list.emplace_back(0, 16, 15);
    exp_sort_adj_list.emplace_back(0, 16, 15);
    exp_sort_adj_list.emplace_back(0, 17, 16);
    exp_sort_adj_list.emplace_back(0, 18, 16);

    exp_sort_adj_list.emplace_back(1, 17, 15);

    ASSERT_EQ(exp_sort_adj_list, adj_list);
}

TEST(parallel_order_gr_adj_list, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

//    adj_list_t exp_adj_list;
//    exp_adj_list.emplace_back(0, 21, 20);
//    exp_adj_list.emplace_back(0, 20, 16);
//    exp_adj_list.emplace_back(1, 19, 16);
    size_t exp_begin = 2;

//    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_begin, begin);

//    ips4o::parallel::sort(adj_list.begin(), adj_list.end());

    adj_list_t exp_sort_adj_list;
    exp_sort_adj_list.emplace_back(0, 20, 16);
    exp_sort_adj_list.emplace_back(0, 21, 20);
    exp_sort_adj_list.emplace_back(1, 19, 16);

    ASSERT_EQ(exp_sort_adj_list, adj_list);
}

TEST(parallel_order_gr_adj_list, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(0, 23, 22);
    size_t exp_begin = 1;

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_begin, begin);
}


TEST(parallel_order_gr_partition, repreated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(parallel_order_gr_partition, repreated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_parl_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_parl_l, part_l);
}

TEST(parallel_order_gr_partition, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    exp_partition[3] = false;
    exp_partition[4] = true;
    exp_partition[5] = false;
    exp_partition[6] = false;
    exp_partition[7] = false;
    exp_partition[8] = true;
    bool exp_parl_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_parl_l, part_l);
}

TEST(parallel_order_gr_partition, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

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
    bool exp_parl_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_parl_l, part_l);
}

TEST(parallel_order_gr_partition, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{14, 15, 16, 17, 18};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[14] = false;
    exp_partition[15] = true;
    exp_partition[16] = false;
    exp_partition[17] = false;
    exp_partition[18] = true;
    bool exp_parl_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_parl_l, part_l);
}

TEST(parallel_order_gr_partition, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{16, 19, 20, 21};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;
    bool exp_parl_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_parl_l, part_l);
}

TEST(parallel_order_gr_partition, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{22, 23};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    size_t begin = 0;
    bool part_l = false;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list, begin);
    ips4o::parallel::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(text, adj_list, partition, begin, part_l);

    partition_t exp_partition;
    exp_partition[22] = false;
    exp_partition[23] = true;
    bool exp_parl_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_parl_l, part_l);
}


TEST(parallel_order_gr_pcomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(4);
    rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    rlslp.blocks = 4;
    bv_t bv = {true, true, true, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9, 9, 11, 6, 12, 5, 10, 7, 10, 6, 12, 5, 10, 3, 9});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(8);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, 131161251071061251039) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 11, 6, 12, 5, 10, 7, 10, 6, 12, 5, 10, 3, 9});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(9);
    rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{14, 18, 16, 17, 18, 16, 15});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(14);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    exp_rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};

    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, 14181617181615) {
    text_t text = util::create_ui_vector(std::vector<var_t>{14, 18, 16, 17, 18, 16, 15});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(14);
    rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{14, 19, 17, 19, 15});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(15);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    exp_rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    exp_rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, 1419171915) {
    text_t text = util::create_ui_vector(std::vector<var_t>{14, 19, 17, 19, 15});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(15);
    rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{20, 21, 15});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(17);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    exp_rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    exp_rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    exp_rlslp.non_terminals[15] = non_terminal<var_t, term_t>{14, 19, 16};
    exp_rlslp.non_terminals[16] = non_terminal<var_t, term_t>{17, 19, 13};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, 202115) {
    text_t text = util::create_ui_vector(std::vector<var_t>{20, 21, 15});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(17);
    rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    rlslp.non_terminals[15] = non_terminal<var_t, term_t>{14, 19, 16};
    rlslp.non_terminals[16] = non_terminal<var_t, term_t>{17, 19, 13};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{22, 15});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(18);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    exp_rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    exp_rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    exp_rlslp.non_terminals[15] = non_terminal<var_t, term_t>{14, 19, 16};
    exp_rlslp.non_terminals[16] = non_terminal<var_t, term_t>{17, 19, 13};
    exp_rlslp.non_terminals[17] = non_terminal<var_t, term_t>{20, 21, 29};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_order_gr_pcomp, 2215) {
    text_t text = util::create_ui_vector(std::vector<var_t>{22, 15});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    rlslp.terminals = 5;
    rlslp.resize(18);
    rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    rlslp.non_terminals[15] = non_terminal<var_t, term_t>{14, 19, 16};
    rlslp.non_terminals[16] = non_terminal<var_t, term_t>{17, 19, 13};
    rlslp.non_terminals[17] = non_terminal<var_t, term_t>{20, 21, 29};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{23});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    rlslp.resize(19);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{8, 1, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t, term_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t, term_t>{3, 9, 3};
    exp_rlslp.non_terminals[11] = non_terminal<var_t, term_t>{5, 10, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t, term_t>{7, 10, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t, term_t>{6, 12, 4};
    exp_rlslp.non_terminals[14] = non_terminal<var_t, term_t>{18, 16, 8};
    exp_rlslp.non_terminals[15] = non_terminal<var_t, term_t>{14, 19, 16};
    exp_rlslp.non_terminals[16] = non_terminal<var_t, term_t>{17, 19, 13};
    exp_rlslp.non_terminals[17] = non_terminal<var_t, term_t>{20, 21, 29};
    exp_rlslp.non_terminals[18] = non_terminal<var_t, term_t>{22, 15, 32};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(parallel_order_gr_recomp, empty) {
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, terminal) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{112});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 112;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, short_block2) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{112, 2, 2};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, short_block3) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112, 112});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{112, 3, 3};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, recompression) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{23});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 18;
    exp_rlslp.resize(19);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{4, 1, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{22, 1, 4};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{2, 3, 2};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{23, 7, 8};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{3, 5, 3};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{19, 6, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t, term_t>{21, 6, 5};
    exp_rlslp.non_terminals[8] = non_terminal<var_t, term_t>{20, 8, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t, term_t>{13, 11, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t, term_t>{9, 14, 16};
    exp_rlslp.non_terminals[11] = non_terminal<var_t, term_t>{12, 14, 13};
    exp_rlslp.non_terminals[12] = non_terminal<var_t, term_t>{15, 16, 29};
    exp_rlslp.non_terminals[13] = non_terminal<var_t, term_t>{17, 10, 32};
    exp_rlslp.non_terminals[14] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[15] = non_terminal<var_t, term_t>{3, 2, 2};
    exp_rlslp.non_terminals[16] = non_terminal<var_t, term_t>{3, 3, 3};
    exp_rlslp.non_terminals[17] = non_terminal<var_t, term_t>{4, 3, 3};
    exp_rlslp.non_terminals[18] = non_terminal<var_t, term_t>{5, 2, 4};
    exp_rlslp.blocks = 14;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, one_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{2, 21, 21};
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, two_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(3);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{5, 4, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{2, 9, 9};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, three_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{7});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(5);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{5, 6, 11};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{7, 3, 20};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{1, 7, 7};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{2, 4, 4};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{2, 9, 9};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, four_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 5;
    exp_rlslp.resize(7);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{8, 6, 6};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{9, 7, 16};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{4, 3, 22};
    exp_rlslp.non_terminals[3] = non_terminal<var_t, term_t>{0, 2, 2};
    exp_rlslp.non_terminals[4] = non_terminal<var_t, term_t>{1, 7, 7};
    exp_rlslp.non_terminals[5] = non_terminal<var_t, term_t>{2, 4, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t, term_t>{2, 9, 9};
    exp_rlslp.blocks = 3;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{4});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(2);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{3, 11, 22};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(3);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t, term_t>{2, 5, 23};
    exp_rlslp.non_terminals[2] = non_terminal<var_t, term_t>{3, 11, 22};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_order_gr_recomp, pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 111});
    rlslp<var_t, term_t> rlslp;
    parallel::recompression_order_gr<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{112, 111, 2};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}
