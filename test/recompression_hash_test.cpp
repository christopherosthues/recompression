#include <gtest/gtest.h>

#define private public

#include "recompression/hash_recompression.hpp"
#include "recompression/util.hpp"

using namespace recomp;

typedef hash_recompression<var_t>::text_t text_t;
typedef hash_recompression<var_t>::adj_list_t adj_list_t;
typedef hash_recompression<var_t>::partition_t partition_t;
typedef hash_recompression<var_t>::alphabet_t alphabet_t;
typedef hash_recompression<var_t>::bv_t bv_t;

TEST(bcomp_hash, no_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
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

TEST(bcomp_hash, 21214441332311413334133231141321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 5, 1, 6, 2, 3, 7, 4, 1, 8, 4, 1, 6, 2, 3, 7, 4, 1, 3, 2, 1});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(bcomp_hash, 222222222222222222222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
    exp_rlslp.blocks = 1;
    bv_t exp_bv = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(bcomp_hash, 22222222211111112222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 4, 5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(3, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.blocks = 3;
    bv_t exp_bv = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(bcomp_hash, 2222222221111111222200) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = alphabet_size;
    bv_t bv;
    recomp.bcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 4, 5, 6});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;
    exp_rlslp.resize(4, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{0, 2, 2};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(adj_list_hash, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list;
    partition_t partition;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list;
    partition_t partition;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list;
    partition_t partition;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list;
    partition_t partition;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(21, 20, 1);
    exp_adj_list[0].emplace_back(20, 16, 1);
    exp_adj_list[1].emplace_back(19, 16, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(adj_list_hash, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list;
    partition_t partition;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);

    adj_list_t exp_adj_list;
    exp_adj_list[0].emplace_back(23, 22, 1);

    ips4o::sort(exp_adj_list[0].begin(), exp_adj_list[0].end());
    ips4o::sort(exp_adj_list[1].begin(), exp_adj_list[1].end());

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(partition_hash, repreated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(partition_hash, repreated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[1] = false;
    exp_partition[2] = true;
    bool exp_part_l = false;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}

TEST(partition_hash, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

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
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list;
    partition_t partition;
    bool part_l = false;
    hash_recompression<var_t> recomp;
    recomp.compute_adj_list(text, adj_list, partition);
    ips4o::sort(adj_list[0].begin(), adj_list[0].end());
    ips4o::sort(adj_list[1].begin(), adj_list[1].end());
    recomp.compute_partition(text, adj_list, partition, part_l);

    partition_t exp_partition;
    exp_partition[22] = false;
    exp_partition[23] = true;
    bool exp_part_l = true;

    ASSERT_EQ(exp_partition, partition);
    ASSERT_EQ(exp_part_l, part_l);
}


TEST(pcomp_hash, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(pcomp_hash, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 3;
    bv_t bv;
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    exp_rlslp.resize(1, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.blocks = 0;
    bv_t exp_bv = {false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(pcomp_hash, 212151623741841623741321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 5, 1, 6, 2, 3, 7, 4, 1, 8, 4, 1, 6, 2, 3, 7, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 5;
    rlslp.resize(4, 1);
    rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    rlslp.blocks = 4;
    bv_t bv = {true, true, true, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9, 9, 10, 6, 11, 7, 12, 8, 12, 6, 11, 7, 12, 3, 9});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(8, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.blocks = 4;
    bv_t exp_bv = {true, true, true, true, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(pcomp_hash, 131061171281261171239) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 10, 6, 11, 7, 12, 8, 12, 6, 11, 7, 12, 3, 9});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 5;
    rlslp.resize(9, 1);
    rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{14, 15, 16, 17, 15, 16, 18});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(14, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(pcomp_hash, 14151617151618) {
    text_t text = util::create_ui_vector(std::vector<var_t>{14, 15, 16, 17, 15, 16, 18});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 5;
    rlslp.resize(14, 1);
    rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{19, 16, 20, 21});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(17, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{14, 15, 12};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{17, 15, 9};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{16, 18, 7};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(pcomp_hash, 19162021) {
    text_t text = util::create_ui_vector(std::vector<var_t>{19, 16, 20, 21});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 5;
    rlslp.resize(17, 1);
    rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    rlslp.non_terminals[14] = non_terminal<var_t>{14, 15, 12};
    rlslp.non_terminals[15] = non_terminal<var_t>{17, 15, 9};
    rlslp.non_terminals[16] = non_terminal<var_t>{16, 18, 7};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{22, 23});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(19, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{14, 15, 12};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{17, 15, 9};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{16, 18, 7};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{19, 16, 16};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{20, 21, 16};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(pcomp_hash, 2223) {
    text_t text = util::create_ui_vector(std::vector<var_t>{22, 23});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    rlslp.terminals = 5;
    rlslp.resize(19, 1);
    rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    rlslp.non_terminals[14] = non_terminal<var_t>{14, 15, 12};
    rlslp.non_terminals[15] = non_terminal<var_t>{17, 15, 9};
    rlslp.non_terminals[16] = non_terminal<var_t>{16, 18, 7};
    rlslp.non_terminals[17] = non_terminal<var_t>{19, 16, 16};
    rlslp.non_terminals[18] = non_terminal<var_t>{20, 21, 16};
    rlslp.blocks = 5;
    bv_t bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false, false,
            false, false, false};
    recomp.pcomp(text, rlslp, bv);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{24});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    exp_rlslp.resize(20, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{5, 1, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{9, 2, 4};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{13, 10, 8};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{6, 11, 4};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{7, 12, 4};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{8, 12, 5};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{3, 9, 3};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{14, 15, 12};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{17, 15, 9};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{16, 18, 7};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{19, 16, 16};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{20, 21, 16};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{22, 23, 32};
    exp_rlslp.blocks = 5;
    bv_t exp_bv = {true, true, true, true, false, false, false, false, true, false, false, false, false, false, false,
                false, false, false, false, false};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}


TEST(recomp_hash, empty) {
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    
}

TEST(recomp_hash, terminal) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
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

TEST(recomp_hash, short_block2) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 2, 2};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, short_block3) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112, 112});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{112, 3, 3};
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, recompression) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{24});

    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 19;
    exp_rlslp.resize(20, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{20, 1, 4};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 3, 2};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{4, 1, 2};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{24, 6, 8};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{21, 7, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{22, 8, 4};
    exp_rlslp.non_terminals[7] = non_terminal<var_t>{23, 8, 5};
    exp_rlslp.non_terminals[8] = non_terminal<var_t>{3, 5, 3};
    exp_rlslp.non_terminals[9] = non_terminal<var_t>{9, 10, 12};
    exp_rlslp.non_terminals[10] = non_terminal<var_t>{12, 10, 9};
    exp_rlslp.non_terminals[11] = non_terminal<var_t>{11, 13, 7};
    exp_rlslp.non_terminals[12] = non_terminal<var_t>{14, 11, 16};
    exp_rlslp.non_terminals[13] = non_terminal<var_t>{15, 16, 16};
    exp_rlslp.non_terminals[14] = non_terminal<var_t>{17, 18, 32};
    exp_rlslp.non_terminals[15] = non_terminal<var_t>{4, 3, 3};
    exp_rlslp.non_terminals[16] = non_terminal<var_t>{3, 2, 2};
    exp_rlslp.non_terminals[17] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[18] = non_terminal<var_t>{3, 3, 3};
    exp_rlslp.non_terminals[19] = non_terminal<var_t>{5, 2, 4};
    exp_rlslp.blocks = 15;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    
}

TEST(recomp_hash, one_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(1, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 21, 21};
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    
}

TEST(recomp_hash, two_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(3, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{4, 5, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    
}

TEST(recomp_hash, three_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{7});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(5, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{5, 6, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 7, 20};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    
}

TEST(recomp_hash, four_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{9});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 5;
    exp_rlslp.resize(7, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{6, 7, 16};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{8, 9, 6};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 4, 22};
    exp_rlslp.non_terminals[3] = non_terminal<var_t>{2, 9, 9};
    exp_rlslp.non_terminals[4] = non_terminal<var_t>{1, 7, 7};
    exp_rlslp.non_terminals[5] = non_terminal<var_t>{2, 4, 4};
    exp_rlslp.non_terminals[6] = non_terminal<var_t>{0, 2, 2};
    exp_rlslp.blocks = 3;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    
}

TEST(recomp_hash, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{4});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(2, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{2, 1, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{3, 11, 22};
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(recomp_hash, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t> rlslp;
    hash_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{5});
    recomp::rlslp<var_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 4;
    exp_rlslp.resize(3, 1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t>{1, 2, 2};
    exp_rlslp.non_terminals[1] = non_terminal<var_t>{2, 5, 23};
    exp_rlslp.non_terminals[2] = non_terminal<var_t>{3, 11, 22};
    exp_rlslp.blocks = 2;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}
