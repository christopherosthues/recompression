#include <gtest/gtest.h>

#define private public

#include "recompression_hash.hpp"

using namespace recomp;

typedef recompression_hash<var_t, term_t>::text_t text_t;
//typedef recompression_hash<var_t, term_t>::adj_list_t adj_list_t;
typedef recompression_hash<var_t, term_t>::adj_list_t adj_list_t;
typedef recompression_hash<var_t, term_t>::partition_t partition_t;
typedef recompression_hash<var_t, term_t>::alphabet_t alphabet_t;

TEST(bcomp_hash, no_block) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
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

TEST(bcomp_hash, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {2, 1, 2, 1, 5, 1, 6, 2, 3, 7, 4, 1, 8, 4, 1, 6, 2, 3, 7, 4, 1, 3, 2, 1};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(bcomp_hash, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
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

TEST(bcomp_hash, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {3, 4, 5};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.blocks = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(bcomp_hash, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    recomp.bcomp(text, rlslp);

    text_t exp_text = {3, 4, 5, 6};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.blocks = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}


TEST(adj_list_hash, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list;
    partition_t partition;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(2, 1, 3);
    exp_adj_list[1].emplace_back(2, 1, 1);
    exp_adj_list[1].emplace_back(8, 1, 1);
    exp_adj_list[0].emplace_back(8, 1, 1);
    exp_adj_list[1].emplace_back(6, 1, 2);
    exp_adj_list[0].emplace_back(6, 2, 2);
    exp_adj_list[1].emplace_back(3, 2, 2);
    exp_adj_list[1].emplace_back(5, 3, 2);
    exp_adj_list[0].emplace_back(5, 4, 2);
    exp_adj_list[0].emplace_back(4, 1, 3);
    exp_adj_list[1].emplace_back(7, 1, 1);
    exp_adj_list[0].emplace_back(7, 4, 1);
    exp_adj_list[1].emplace_back(3, 1, 1);
    exp_adj_list[0].emplace_back(3, 2, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list_hash, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t adj_list;
    partition_t partition;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(13, 12, 1);
    exp_adj_list[0].emplace_back(12, 6, 1);
    exp_adj_list[1].emplace_back(10, 6, 2);
    exp_adj_list[0].emplace_back(10, 5, 2);
    exp_adj_list[1].emplace_back(11, 5, 2);
    exp_adj_list[0].emplace_back(11, 7, 1);
    exp_adj_list[1].emplace_back(11, 7, 1);
    exp_adj_list[0].emplace_back(11, 6, 1);
    exp_adj_list[0].emplace_back(11, 3, 1);
    exp_adj_list[1].emplace_back(9, 3, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list_hash, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t adj_list;
    partition_t partition;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(18, 16, 1);
    exp_adj_list[0].emplace_back(16, 15, 2);
    exp_adj_list[1].emplace_back(17, 15, 1);
    exp_adj_list[0].emplace_back(17, 16, 1);
    exp_adj_list[0].emplace_back(15, 14, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list_hash, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t adj_list;
    partition_t partition;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(21, 20, 1);
    exp_adj_list[0].emplace_back(20, 16, 1);
    exp_adj_list[1].emplace_back(19, 16, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list_hash, 2322) {
    text_t text{23, 22};
    adj_list_t adj_list;
    partition_t partition;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(23, 22, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(partition_hash, repreated_pair) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(partition_hash, repreated_pair_same_occ) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(partition_hash, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

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

TEST(partition_hash, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

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

TEST(partition_hash, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

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

TEST(partition_hash, 21201619) {
    text_t text{21, 20, 16, 19};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(partition_hash, 2322) {
    text_t text{23, 22};
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    recompression_hash<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[22] = false;
    exp_partition[23] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}


TEST(pcomp_hash, repeated_pair) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
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

TEST(pcomp_hash, repeated_pair_same_occ) {
    text_t text{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
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

TEST(pcomp_hash, 212151623741841623741321) {
    text_t text{2, 1, 2, 1, 5, 1, 6, 2, 3, 7, 4, 1, 8, 4, 1, 6, 2, 3, 7, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.blocks = {true, true, true, true};
    recomp.pcomp(text, rlslp);

    text_t exp_text{9, 9, 10, 6, 11, 7, 12, 8, 12, 6, 11, 7, 12, 3, 9};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(5, 1, 4);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);

    exp_rlslp.blocks = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp_hash, 131061171281261171239) {
    text_t text{13, 10, 6, 11, 7, 12, 8, 12, 6, 11, 7, 12, 3, 9};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(5, 1, 4);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp);

    text_t exp_text{14, 15, 16, 17, 15, 16, 18};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(5, 1, 4);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
    exp_rlslp.non_terminals.emplace_back(13, 10, 8);
    exp_rlslp.non_terminals.emplace_back(6, 11, 4);
    exp_rlslp.non_terminals.emplace_back(7, 12, 4);
    exp_rlslp.non_terminals.emplace_back(8, 12, 5);
    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp_hash, 14151617151618) {
    text_t text{14, 15, 16, 17, 15, 16, 18};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(5, 1, 4);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(13, 10, 8);
    rlslp.non_terminals.emplace_back(6, 11, 4);
    rlslp.non_terminals.emplace_back(7, 12, 4);
    rlslp.non_terminals.emplace_back(8, 12, 5);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp);

    text_t exp_text{19, 16, 20, 21};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(5, 1, 4);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
    exp_rlslp.non_terminals.emplace_back(13, 10, 8);
    exp_rlslp.non_terminals.emplace_back(6, 11, 4);
    exp_rlslp.non_terminals.emplace_back(7, 12, 4);
    exp_rlslp.non_terminals.emplace_back(8, 12, 5);
    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
    exp_rlslp.non_terminals.emplace_back(14, 15, 12);
    exp_rlslp.non_terminals.emplace_back(17, 15, 9);
    exp_rlslp.non_terminals.emplace_back(16, 18, 7);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp_hash, 19162021) {
    text_t text{19, 16, 20, 21};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(5, 1, 4);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(13, 10, 8);
    rlslp.non_terminals.emplace_back(6, 11, 4);
    rlslp.non_terminals.emplace_back(7, 12, 4);
    rlslp.non_terminals.emplace_back(8, 12, 5);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(14, 15, 12);
    rlslp.non_terminals.emplace_back(17, 15, 9);
    rlslp.non_terminals.emplace_back(16, 18, 7);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false};
    recomp.pcomp(text, rlslp);

    text_t exp_text{22, 23};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(5, 1, 4);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
    exp_rlslp.non_terminals.emplace_back(13, 10, 8);
    exp_rlslp.non_terminals.emplace_back(6, 11, 4);
    exp_rlslp.non_terminals.emplace_back(7, 12, 4);
    exp_rlslp.non_terminals.emplace_back(8, 12, 5);
    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
    exp_rlslp.non_terminals.emplace_back(14, 15, 12);
    exp_rlslp.non_terminals.emplace_back(17, 15, 9);
    exp_rlslp.non_terminals.emplace_back(16, 18, 7);
    exp_rlslp.non_terminals.emplace_back(19, 16, 16);
    exp_rlslp.non_terminals.emplace_back(20, 21, 16);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(pcomp_hash, 2223) {
    text_t text{22, 23};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    rlslp.terminals = 5;
    rlslp.non_terminals.emplace_back(4, 3, 3);
    rlslp.non_terminals.emplace_back(3, 2, 2);
    rlslp.non_terminals.emplace_back(1, 2, 2);
    rlslp.non_terminals.emplace_back(3, 3, 3);
    rlslp.non_terminals.emplace_back(2, 1, 2);
    rlslp.non_terminals.emplace_back(5, 1, 4);
    rlslp.non_terminals.emplace_back(2, 3, 2);
    rlslp.non_terminals.emplace_back(4, 1, 2);
    rlslp.non_terminals.emplace_back(9, 2, 4);
    rlslp.non_terminals.emplace_back(13, 10, 8);
    rlslp.non_terminals.emplace_back(6, 11, 4);
    rlslp.non_terminals.emplace_back(7, 12, 4);
    rlslp.non_terminals.emplace_back(8, 12, 5);
    rlslp.non_terminals.emplace_back(3, 9, 3);
    rlslp.non_terminals.emplace_back(14, 15, 12);
    rlslp.non_terminals.emplace_back(17, 15, 9);
    rlslp.non_terminals.emplace_back(16, 18, 7);
    rlslp.non_terminals.emplace_back(19, 16, 16);
    rlslp.non_terminals.emplace_back(20, 21, 16);
    rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                    false, false, false, false};
    recomp.pcomp(text, rlslp);

    text_t exp_text{24};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(5, 1, 4);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
    exp_rlslp.non_terminals.emplace_back(13, 10, 8);
    exp_rlslp.non_terminals.emplace_back(6, 11, 4);
    exp_rlslp.non_terminals.emplace_back(7, 12, 4);
    exp_rlslp.non_terminals.emplace_back(8, 12, 5);
    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
    exp_rlslp.non_terminals.emplace_back(14, 15, 12);
    exp_rlslp.non_terminals.emplace_back(17, 15, 9);
    exp_rlslp.non_terminals.emplace_back(16, 18, 7);
    exp_rlslp.non_terminals.emplace_back(19, 16, 16);
    exp_rlslp.non_terminals.emplace_back(20, 21, 16);
    exp_rlslp.non_terminals.emplace_back(22, 23, 32);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}


TEST(recomp_hash, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {24};

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 19;
    exp_rlslp.non_terminals.emplace_back(4, 3, 3);
    exp_rlslp.non_terminals.emplace_back(3, 2, 2);
    exp_rlslp.non_terminals.emplace_back(1, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 3, 3);
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(5, 1, 4);
    exp_rlslp.non_terminals.emplace_back(2, 3, 2);
    exp_rlslp.non_terminals.emplace_back(4, 1, 2);
    exp_rlslp.non_terminals.emplace_back(9, 2, 4);
    exp_rlslp.non_terminals.emplace_back(13, 10, 8);
    exp_rlslp.non_terminals.emplace_back(6, 11, 4);
    exp_rlslp.non_terminals.emplace_back(7, 12, 4);
    exp_rlslp.non_terminals.emplace_back(8, 12, 5);
    exp_rlslp.non_terminals.emplace_back(3, 9, 3);
    exp_rlslp.non_terminals.emplace_back(14, 15, 12);
    exp_rlslp.non_terminals.emplace_back(17, 15, 9);
    exp_rlslp.non_terminals.emplace_back(16, 18, 7);
    exp_rlslp.non_terminals.emplace_back(19, 16, 16);
    exp_rlslp.non_terminals.emplace_back(20, 21, 16);
    exp_rlslp.non_terminals.emplace_back(22, 23, 32);
    exp_rlslp.blocks = {true, true, true, true, false, false, false, false, true, false, false, false, false, false,
                        false, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {3};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {5};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 2;
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(3, 4, 16);
    exp_rlslp.blocks = {true, true, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {7};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(3, 4, 16);
    exp_rlslp.non_terminals.emplace_back(6, 5, 20);
    exp_rlslp.blocks = {true, true, true, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {9};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 6;
    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
    exp_rlslp.non_terminals.emplace_back(0, 2, 2);
    exp_rlslp.non_terminals.emplace_back(3, 4, 16);
    exp_rlslp.non_terminals.emplace_back(5, 6, 6);
    exp_rlslp.non_terminals.emplace_back(7, 8, 22);
    exp_rlslp.blocks = {true, true, true, true, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, repeated_pair) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 1;
    exp_rlslp.non_terminals.emplace_back(2, 1, 2);
    exp_rlslp.non_terminals.emplace_back(3, 11, 22);
    exp_rlslp.blocks = {false, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, repeated_pair_same_occ) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_hash<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    text_t exp_text = {5};
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
