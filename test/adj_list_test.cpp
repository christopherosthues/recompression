#include <gtest/gtest.h>

#define private public

#include "fast_recompression.hpp"
#include "recompression.hpp"
#include "parallel_recompression.hpp"
#include "parallel_fast_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;
typedef recompression<var_t, term_t>::adj_list_t adj_list_t;

typedef recompression_fast<var_t, term_t>::text_t fast_text_t;
typedef recompression_fast<var_t, term_t>::adj_list_t fast_adj_list_t;

typedef parallel::recompression<var_t, term_t>::text_t par_text_t;
typedef parallel::recompression<var_t, term_t>::adj_list_t par_adj_list_t;

typedef parallel::recompression_fast<var_t, term_t>::adj_list_t par_fast_adj_list_t;

TEST(parallel_adj_list, 212181623541741623541321) {
    par_text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    par_adj_list_t adj_list(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    par_adj_list_t exp_adj_list;
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

TEST(parallel_adj_list, 131261051171161051139) {
    par_text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    par_adj_list_t adj_list(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    par_adj_list_t exp_adj_list;
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

TEST(parallel_adj_list, 18161517161514) {
    par_text_t text{18, 16, 15, 17, 16, 15, 14};
    par_adj_list_t adj_list(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    par_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(18, 16, 0);
    exp_adj_list.emplace_back(16, 15, 0);
    exp_adj_list.emplace_back(17, 15, 1);
    exp_adj_list.emplace_back(17, 16, 0);
    exp_adj_list.emplace_back(16, 15, 0);
    exp_adj_list.emplace_back(15, 14, 0);
    
    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 21201619) {
    par_text_t text{21, 20, 16, 19};
    par_adj_list_t adj_list(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    par_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(21, 20, 0);
    exp_adj_list.emplace_back(20, 16, 0);
    exp_adj_list.emplace_back(19, 16, 1);
    
    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_adj_list, 2322) {
    par_text_t text{23, 22};
    par_adj_list_t adj_list(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    par_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(23, 22, 0);
    
    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(parallel_fast_adj_list, 212181623541741623541321) {
    par_text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    par_fast_adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, begin);

    par_fast_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1, 2, 1);
    exp_adj_list.emplace_back(0, 2, 1);
    exp_adj_list.emplace_back(1, 2, 1);
    exp_adj_list.emplace_back(0, 8, 1);
    exp_adj_list.emplace_back(1, 8, 1);
    exp_adj_list.emplace_back(0, 6, 1);
    exp_adj_list.emplace_back(1, 6, 2);
    exp_adj_list.emplace_back(0, 3, 2);
    exp_adj_list.emplace_back(0, 5, 3);
    exp_adj_list.emplace_back(1, 5, 4);
    exp_adj_list.emplace_back(1, 4, 1);
    exp_adj_list.emplace_back(0, 7, 1);
    exp_adj_list.emplace_back(1, 7, 4);
    exp_adj_list.emplace_back(1, 4, 1);
    exp_adj_list.emplace_back(0, 6, 1);
    exp_adj_list.emplace_back(1, 6, 2);
    exp_adj_list.emplace_back(0, 3, 2);
    exp_adj_list.emplace_back(0, 5, 3);
    exp_adj_list.emplace_back(1, 5, 4);
    exp_adj_list.emplace_back(1, 4, 1);
    exp_adj_list.emplace_back(0, 3, 1);
    exp_adj_list.emplace_back(1, 3, 2);
    exp_adj_list.emplace_back(1, 2, 1);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_fast_adj_list, 131261051171161051139) {
    par_text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    par_fast_adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, begin);

    par_fast_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1, 13, 12);
    exp_adj_list.emplace_back(1, 12, 6);
    exp_adj_list.emplace_back(0, 10, 6);
    exp_adj_list.emplace_back(1, 10, 5);
    exp_adj_list.emplace_back(0, 11, 5);
    exp_adj_list.emplace_back(1, 11, 7);
    exp_adj_list.emplace_back(0, 11, 7);
    exp_adj_list.emplace_back(1, 11, 6);
    exp_adj_list.emplace_back(0, 10, 6);
    exp_adj_list.emplace_back(1, 10, 5);
    exp_adj_list.emplace_back(0, 11, 5);
    exp_adj_list.emplace_back(1, 11, 3);
    exp_adj_list.emplace_back(0, 9, 3);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_fast_adj_list, 18161517161514) {
    par_text_t text{18, 16, 15, 17, 16, 15, 14};
    par_fast_adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, begin);

    par_fast_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1, 18, 16);
    exp_adj_list.emplace_back(1, 16, 15);
    exp_adj_list.emplace_back(0, 17, 15);
    exp_adj_list.emplace_back(1, 17, 16);
    exp_adj_list.emplace_back(1, 16, 15);
    exp_adj_list.emplace_back(1, 15, 14);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_fast_adj_list, 21201619) {
    par_text_t text{21, 20, 16, 19};
    par_fast_adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, begin);

    par_fast_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1, 21, 20);
    exp_adj_list.emplace_back(1, 20, 16);
    exp_adj_list.emplace_back(0, 19, 16);

    ASSERT_EQ(exp_adj_list, adj_list);
}

TEST(parallel_fast_adj_list, 2322) {
    par_text_t text{23, 22};
    par_fast_adj_list_t adj_list(text.size() - 1);
    size_t begin = 0;
    parallel::recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list, begin);

    par_fast_adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(1, 23, 22);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(adj_list, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t adj_list(text.size() - 1);
    recompression<var_t, term_t> recomp;
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
    recompression<var_t, term_t> recomp;
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
    recompression<var_t, term_t> recomp;
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
    recompression<var_t, term_t> recomp;
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
    recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    adj_list_t exp_adj_list;
    exp_adj_list.emplace_back(23, 22, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
}


TEST(fast_adj_list, 212181623541741623541321) {
    fast_text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    fast_adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    fast_adj_list_t exp_adj_list(alphabet_size);
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
    fast_text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    fast_adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    fast_adj_list_t exp_adj_list(alphabet_size);
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
    fast_text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    fast_adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    fast_adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(1, 0);
    exp_adj_list[2][1] = std::make_pair(2, 0);
    exp_adj_list[3][1] = std::make_pair(0, 1);
    exp_adj_list[3][2] = std::make_pair(1, 0);
    exp_adj_list[4][2] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}

TEST(fast_adj_list, 21201619) {
    fast_text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    fast_adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    fast_adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(0, 1);
    exp_adj_list[2][0] = std::make_pair(1, 0);
    exp_adj_list[3][2] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}

TEST(fast_adj_list, 2322) {
    fast_text_t text{1, 0};
    term_t alphabet_size = 2;
    fast_adj_list_t adj_list(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, adj_list);

    fast_adj_list_t exp_adj_list(alphabet_size);
    exp_adj_list[1][0] = std::make_pair(1, 0);

    ASSERT_EQ(exp_adj_list, adj_list);
    ASSERT_EQ(exp_adj_list.size(), adj_list.size());
}
