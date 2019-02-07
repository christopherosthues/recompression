#include <gtest/gtest.h>

#define private public

#include "sequential_recompression.hpp"

using namespace recomp;

typedef sequential_recompression<var_t, term_t>::text_t text_t;
typedef sequential_recompression<var_t, term_t>::adj_list_t adj_list_t;
typedef sequential_recompression<var_t, term_t>::partition_t partition_t;
typedef sequential_recompression<var_t, term_t>::alphabet_t alphabet_t;

TEST(bcomp, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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


TEST(adj_list, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list(text.size() - 1);
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(2, 1, 0);
    exp_adj_list.emplace_back(2, 1, 1);
    exp_adj_list.emplace_back(2, 1, 0);
    exp_adj_list.emplace_back(8, 1, 1);
    exp_adj_list.emplace_back(8, 1, 0);
    exp_adj_list.emplace_back(6, 1, 1);
    exp_adj_list.emplace_back(6, 2, 0);
    exp_adj_list.emplace_back(3, 2, 1);
    exp_adj_list.emplace_back(5, 3, 1);
    exp_adj_list.emplace_back(5, 4, 0);
    exp_adj_list.emplace_back(4, 1, 0);
    exp_adj_list.emplace_back(7, 1, 1);
    exp_adj_list.emplace_back(7, 4, 0);
    exp_adj_list.emplace_back(4, 1, 0);
    exp_adj_list.emplace_back(6, 1, 1);
    exp_adj_list.emplace_back(6, 2, 0);
    exp_adj_list.emplace_back(3, 2, 1);
    exp_adj_list.emplace_back(5, 3, 1);
    exp_adj_list.emplace_back(5, 4, 0);
    exp_adj_list.emplace_back(4, 1, 0);
    exp_adj_list.emplace_back(3, 1, 1);
    exp_adj_list.emplace_back(3, 2, 0);
    exp_adj_list.emplace_back(2, 1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t adj_list(text.size() - 1);
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(13, 12, 0);
    exp_adj_list.emplace_back(12, 6, 0);
    exp_adj_list.emplace_back(10, 6, 1);
    exp_adj_list.emplace_back(10, 5, 0);
    exp_adj_list.emplace_back(11, 5, 1);
    exp_adj_list.emplace_back(11, 7, 0);
    exp_adj_list.emplace_back(11, 7, 1);
    exp_adj_list.emplace_back(11, 6, 0);
    exp_adj_list.emplace_back(10, 6, 1);
    exp_adj_list.emplace_back(10, 5, 0);
    exp_adj_list.emplace_back(11, 5, 1);
    exp_adj_list.emplace_back(11, 3, 0);
    exp_adj_list.emplace_back(9, 3, 1);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t adj_list(text.size() - 1);
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(18, 16, 0);
    exp_adj_list.emplace_back(16, 15, 0);
    exp_adj_list.emplace_back(17, 15, 1);
    exp_adj_list.emplace_back(17, 16, 0);
    exp_adj_list.emplace_back(16, 15, 0);
    exp_adj_list.emplace_back(15, 14, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t adj_list(text.size() - 1);
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(21, 20, 0);
    exp_adj_list.emplace_back(20, 16, 0);
    exp_adj_list.emplace_back(19, 16, 1);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list, 2322) {
    text_t text{23, 22};
    adj_list_t adj_list(text.size() - 1);
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(23, 22, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(partition, repreated_pair) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    ips4o::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition;
    exp_partition[1] = true;
    exp_partition[2] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, repreated_pair_same_occ) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    adj_list_t adj_list(text.size() - 1);
    alphabet_t alphabet{1, 2};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);
    ips4o::sort(adj_list.begin(), adj_list.end());
    recomp.compute_partition(adj_list, partition);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, partition);

    partition_t exp_partition;
    exp_partition[1] = true;
    exp_partition[2] = false;
    exp_partition[3] = true;
    exp_partition[4] = false;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = true;
    exp_partition[8] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, partition);

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

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{14, 15, 16, 17, 18};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, partition);

    partition_t exp_partition;
    exp_partition[14] = true;
    exp_partition[15] = false;
    exp_partition[16] = true;
    exp_partition[17] = true;
    exp_partition[18] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{16, 19, 20, 21};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, partition);

    partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 2322) {
    text_t text{23, 22};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{22, 23};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    sequential_recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, partition);

    partition_t exp_partition;
    exp_partition[22] = true;
    exp_partition[23] = false;

    ASSERT_EQ(exp_partition, partition);
}


TEST(pcomp, repeated_pair) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    rlslp.terminals = 3;
    recomp.pcomp(text, rlslp);

    text_t exp_text{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.blocks = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp, repeated_pair_same_occ) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    rlslp.terminals = 3;
    recomp.pcomp(text, rlslp);

    text_t exp_text{2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.blocks = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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
    sequential_recompression<var_t, term_t> recomp;
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


TEST(recomp, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, terminal) {
    text_t text = {112};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
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

TEST(recomp, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {24};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 24;
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
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {5};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 5;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(4, 3, 16);
    exp_rlslp.blocks = {true, true, false};
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {7};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 7;
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(3, 4, 11);
    exp_rlslp.non_terminals.emplace_back(5, 6, 20);
    exp_rlslp.blocks = {true, true, true, false, false};
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {9};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 9;
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(5, 3, 6);
    exp_rlslp.non_terminals.emplace_back(6, 4, 16);
    exp_rlslp.non_terminals.emplace_back(8, 7, 22);
    exp_rlslp.blocks = {true, true, true, true, false, false, false};
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, repeated_pair) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = {false, true};
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp, repeated_pair_same_occ) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    sequential_recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = {5};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 5;
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.non_terminals.emplace_back(2, 4, 23);
    exp_rlslp.blocks = {false, true, false};
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}
