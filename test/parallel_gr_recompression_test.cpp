#include <gtest/gtest.h>

#define private public
#define protected public

#include "recompression/parallel_gr_recompression.hpp"
#include "recompression/util.hpp"

using namespace recomp;

typedef parallel::parallel_gr_recompression<var_t, term_t>::text_t text_t;
typedef parallel::parallel_gr_recompression<var_t, term_t>::adj_t adj_t;
typedef parallel::parallel_gr_recompression<var_t, term_t>::adj_list_t adj_list_t;
typedef parallel::parallel_gr_recompression<var_t, term_t>::partition_t partition_t;
typedef parallel::parallel_gr_recompression<var_t, term_t>::alphabet_t alphabet_t;
typedef parallel::parallel_gr_recompression<var_t, term_t>::bv_t bv_t;

TEST(parallel_gr_bcomp, no_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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

TEST(parallel_gr_bcomp, 21214441332311413334133231141321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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

TEST(parallel_gr_bcomp, 222222222222222222222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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
//    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 1;
    bv_t exp_bv = {true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_gr_bcomp, 22222222211111112222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 4, 4);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 3;
    bv_t exp_bv = {true, true, true};

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_bv, bv);
}

TEST(parallel_gr_bcomp, 2222222221111111222200) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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


//TEST(parallel_gr_mapping, left_end) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 4;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{1, 2, 3});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0});
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_mapping, mapping);
//}
//
//TEST(parallel_gr_mapping, 212181623541741623541321) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 9;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{1, 2, 3, 4, 5, 6, 7, 8});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0});
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_mapping, mapping);
//}
//
//TEST(parallel_gr_mapping, 131261051171161051139) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 14;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{3, 5, 6, 7, 9, 10, 11, 12, 13});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4});
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_mapping, mapping);
//}
//
//TEST(parallel_gr_mapping, 18161517161514) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 19;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{14, 15, 16, 17, 18});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{4, 2, 1, 3, 2, 1, 0});
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_mapping, mapping);
//}
//
//TEST(parallel_gr_mapping, 21201619) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 22;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{16, 19, 20, 21});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3, 2, 0, 1});
//
//    ASSERT_EQ(exp_text, text);
//    ASSERT_EQ(exp_mapping, mapping);
//}
//
//TEST(parallel_gr_mapping, 2322) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 24;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{22, 23});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{1, 0});
//
//    ASSERT_EQ(exp_mapping, mapping);
//    ASSERT_EQ(exp_text, text);
//
//}
//
//TEST(parallel_gr_mapping, less_productions) {
//    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
//    rlslp<var_t, term_t> rlslp;
//    rlslp.terminals = 4;
//    parallel::parallel_gr_recompression<var_t, term_t> recomp;
//    recomp.cores = 4;
//    ui_vector<var_t> mapping;
//    recomp.compute_mapping(text, rlslp, mapping);
//
//    ui_vector<var_t> exp_mapping = util::create_ui_vector(std::vector<var_t>{1, 2, 3});
//
//    text_t exp_text = util::create_ui_vector(std::vector<var_t>{1, 0, 1, 0, 1, 0, 1, 0, 2});
//
//    ASSERT_EQ(exp_mapping, mapping);
//    ASSERT_EQ(exp_text, text);
//}


TEST(parallel_gr_adj_list, left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{1, 3, 5, 7, 12, 14, 16, 18, 0, 2, 4, 6, 8, 11, 13, 15, 17, 10, 20, 19, 9});
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(7);
//    exp_adj_list.emplace_back(12);
//    exp_adj_list.emplace_back(14);
//    exp_adj_list.emplace_back(16);
//    exp_adj_list.emplace_back(18);
//    exp_adj_list.emplace_back(0);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(6);
//    exp_adj_list.emplace_back(8);
//    exp_adj_list.emplace_back(11);
//    exp_adj_list.emplace_back(13);
//    exp_adj_list.emplace_back(15);
//    exp_adj_list.emplace_back(17);
//    exp_adj_list.emplace_back(10);
//    exp_adj_list.emplace_back(20);
//    exp_adj_list.emplace_back(19);
//    exp_adj_list.emplace_back(9);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_gr_adj_list, 212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0, 2, 22, 1, 20, 21, 7, 16, 10, 13, 19, 8, 17, 9, 18, 5, 14, 6, 15, 11, 12, 4, 3});
//    exp_adj_list.emplace_back(0);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(22);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(20);
//    exp_adj_list.emplace_back(21);
//    exp_adj_list.emplace_back(7);
//    exp_adj_list.emplace_back(16);
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
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(3);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_gr_adj_list, 131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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

TEST(parallel_gr_adj_list, 18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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

TEST(parallel_gr_adj_list, 21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{2, 1, 0});
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_gr_adj_list, 2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0});
//    exp_adj_list.emplace_back(0);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_gr_adj_list, less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    adj_list_t adj_list(text.size() - 1);
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list = util::create_ui_vector(std::vector<adj_t>{0, 2, 4, 6, 1, 3, 5, 7});
//    exp_adj_list.emplace_back(0);
//    exp_adj_list.emplace_back(2);
//    exp_adj_list.emplace_back(4);
//    exp_adj_list.emplace_back(6);
//    exp_adj_list.emplace_back(1);
//    exp_adj_list.emplace_back(3);
//    exp_adj_list.emplace_back(5);
//    exp_adj_list.emplace_back(7);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(parallel_gr_recomp, empty) {
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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

TEST(parallel_gr_recomp, terminal) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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

TEST(parallel_gr_recomp, short_block2) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{112, 2, 2};
//    exp_rlslp.non_terminals.emplace_back(112, 2, 2);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_gr_recomp, short_block3) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112, 112});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});

    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{112, 3, 3};
//    exp_rlslp.non_terminals.emplace_back(112, 3, 3);
    exp_rlslp.is_empty = false;
    exp_rlslp.blocks = 0;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_gr_recomp, recompression) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, one_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{3});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 3;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{2, 21, 21};
//    exp_rlslp.non_terminals.emplace_back(2, 21, 21);
    exp_rlslp.blocks = 0;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_gr_recomp, two_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
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
//    exp_rlslp.non_terminals.emplace_back(5, 4, 16);
//    exp_rlslp.non_terminals.emplace_back(1, 7, 7);
//    exp_rlslp.non_terminals.emplace_back(2, 9, 9);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}

TEST(parallel_gr_recomp, three_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, four_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 4;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string exp_text = {2, 1, 2, 1, 2, 1, 2, 1, 3};
    ASSERT_EQ(exp_text, rlslp.derive_text());
}

TEST(parallel_gr_recomp, pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 111});
    rlslp<var_t, term_t> rlslp;
    parallel::parallel_gr_recompression<var_t, term_t> recomp;
    recomp.cores = 4;
    term_t alphabet_size = 113;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    text_t exp_text = util::create_ui_vector(std::vector<var_t>{113});
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = alphabet_size;
    exp_rlslp.root = 113;
    exp_rlslp.resize(1);
    exp_rlslp.non_terminals[0] = non_terminal<var_t, term_t>{112, 111, 2};
//    exp_rlslp.non_terminals.emplace_back(112, 111, 2);
    exp_rlslp.blocks = 1;
    exp_rlslp.is_empty = false;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
}
