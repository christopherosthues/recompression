#include <gtest/gtest.h>

#define private public

#include "fast_recompression.hpp"
#include "recompression.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;
typedef recompression<var_t, term_t>::multiset_t multiset_t;

typedef recompression_fast<var_t, term_t>::text_t fast_text_t;
typedef recompression_fast<var_t, term_t>::multiset_t fast_multiset_t;

typedef parallel::recompression<var_t, term_t>::text_t par_text_t;
typedef parallel::recompression<var_t, term_t>::multiset_t par_multiset_t;

TEST(parallel_multiset, 212181623541741623541321) {
    par_text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    par_multiset_t multiset(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    par_multiset_t exp_multiset;
    exp_multiset.emplace_back(2, 1, 0);
    exp_multiset.emplace_back(2, 1, 1);
    exp_multiset.emplace_back(2, 1, 0);
    exp_multiset.emplace_back(8, 1, 1);
    exp_multiset.emplace_back(8, 1, 0);
    exp_multiset.emplace_back(6, 1, 1);
    exp_multiset.emplace_back(6, 2, 0);
    exp_multiset.emplace_back(3, 2, 1);
    exp_multiset.emplace_back(5, 3, 1);
    exp_multiset.emplace_back(5, 4, 0);
    exp_multiset.emplace_back(4, 1, 0);
    exp_multiset.emplace_back(7, 1, 1);
    exp_multiset.emplace_back(7, 4, 0);
    exp_multiset.emplace_back(4, 1, 0);
    exp_multiset.emplace_back(6, 1, 1);
    exp_multiset.emplace_back(6, 2, 0);
    exp_multiset.emplace_back(3, 2, 1);
    exp_multiset.emplace_back(5, 3, 1);
    exp_multiset.emplace_back(5, 4, 0);
    exp_multiset.emplace_back(4, 1, 0);
    exp_multiset.emplace_back(3, 1, 1);
    exp_multiset.emplace_back(3, 2, 0);
    exp_multiset.emplace_back(2, 1, 0);
    
    ASSERT_EQ(exp_multiset, multiset);
}

TEST(parallel_multiset, 131261051171161051139) {
    par_text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    par_multiset_t multiset(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    par_multiset_t exp_multiset;
    exp_multiset.emplace_back(13, 12, 0);
    exp_multiset.emplace_back(12, 6, 0);
    exp_multiset.emplace_back(10, 6, 1);
    exp_multiset.emplace_back(10, 5, 0);
    exp_multiset.emplace_back(11, 5, 1);
    exp_multiset.emplace_back(11, 7, 0);
    exp_multiset.emplace_back(11, 7, 1);
    exp_multiset.emplace_back(11, 6, 0);
    exp_multiset.emplace_back(10, 6, 1);
    exp_multiset.emplace_back(10, 5, 0);
    exp_multiset.emplace_back(11, 5, 1);
    exp_multiset.emplace_back(11, 3, 0);
    exp_multiset.emplace_back(9, 3, 1);
    
    ASSERT_EQ(exp_multiset, multiset);
}

TEST(parallel_multiset, 18161517161514) {
    par_text_t text{18, 16, 15, 17, 16, 15, 14};
    par_multiset_t multiset(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    par_multiset_t exp_multiset;
    exp_multiset.emplace_back(18, 16, 0);
    exp_multiset.emplace_back(16, 15, 0);
    exp_multiset.emplace_back(17, 15, 1);
    exp_multiset.emplace_back(17, 16, 0);
    exp_multiset.emplace_back(16, 15, 0);
    exp_multiset.emplace_back(15, 14, 0);
    
    ASSERT_EQ(exp_multiset, multiset);
}

TEST(parallel_multiset, 21201619) {
    par_text_t text{21, 20, 16, 19};
    par_multiset_t multiset(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    par_multiset_t exp_multiset;
    exp_multiset.emplace_back(21, 20, 0);
    exp_multiset.emplace_back(20, 16, 0);
    exp_multiset.emplace_back(19, 16, 1);
    
    ASSERT_EQ(exp_multiset, multiset);
}

TEST(parallel_multiset, 2322) {
    par_text_t text{23, 22};
    par_multiset_t multiset(text.size() - 1);
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    par_multiset_t exp_multiset;
    exp_multiset.emplace_back(23, 22, 0);
    
    ASSERT_EQ(exp_multiset, multiset);
}


TEST(multiset, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    multiset_t multiset(text.size() - 1);
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    multiset_t exp_multiset;
    exp_multiset.emplace_back(2, 1, 0);
    exp_multiset.emplace_back(2, 1, 1);
    exp_multiset.emplace_back(2, 1, 0);
    exp_multiset.emplace_back(8, 1, 1);
    exp_multiset.emplace_back(8, 1, 0);
    exp_multiset.emplace_back(6, 1, 1);
    exp_multiset.emplace_back(6, 2, 0);
    exp_multiset.emplace_back(3, 2, 1);
    exp_multiset.emplace_back(5, 3, 1);
    exp_multiset.emplace_back(5, 4, 0);
    exp_multiset.emplace_back(4, 1, 0);
    exp_multiset.emplace_back(7, 1, 1);
    exp_multiset.emplace_back(7, 4, 0);
    exp_multiset.emplace_back(4, 1, 0);
    exp_multiset.emplace_back(6, 1, 1);
    exp_multiset.emplace_back(6, 2, 0);
    exp_multiset.emplace_back(3, 2, 1);
    exp_multiset.emplace_back(5, 3, 1);
    exp_multiset.emplace_back(5, 4, 0);
    exp_multiset.emplace_back(4, 1, 0);
    exp_multiset.emplace_back(3, 1, 1);
    exp_multiset.emplace_back(3, 2, 0);
    exp_multiset.emplace_back(2, 1, 0);

    ASSERT_EQ(exp_multiset, multiset);
}

TEST(multiset, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    multiset_t multiset(text.size() - 1);
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    multiset_t exp_multiset;
    exp_multiset.emplace_back(13, 12, 0);
    exp_multiset.emplace_back(12, 6, 0);
    exp_multiset.emplace_back(10, 6, 1);
    exp_multiset.emplace_back(10, 5, 0);
    exp_multiset.emplace_back(11, 5, 1);
    exp_multiset.emplace_back(11, 7, 0);
    exp_multiset.emplace_back(11, 7, 1);
    exp_multiset.emplace_back(11, 6, 0);
    exp_multiset.emplace_back(10, 6, 1);
    exp_multiset.emplace_back(10, 5, 0);
    exp_multiset.emplace_back(11, 5, 1);
    exp_multiset.emplace_back(11, 3, 0);
    exp_multiset.emplace_back(9, 3, 1);

    ASSERT_EQ(exp_multiset, multiset);
}

TEST(multiset, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    multiset_t multiset(text.size() - 1);
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    multiset_t exp_multiset;
    exp_multiset.emplace_back(18, 16, 0);
    exp_multiset.emplace_back(16, 15, 0);
    exp_multiset.emplace_back(17, 15, 1);
    exp_multiset.emplace_back(17, 16, 0);
    exp_multiset.emplace_back(16, 15, 0);
    exp_multiset.emplace_back(15, 14, 0);

    ASSERT_EQ(exp_multiset, multiset);
}

TEST(multiset, 21201619) {
    text_t text{21, 20, 16, 19};
    multiset_t multiset(text.size() - 1);
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    multiset_t exp_multiset;
    exp_multiset.emplace_back(21, 20, 0);
    exp_multiset.emplace_back(20, 16, 0);
    exp_multiset.emplace_back(19, 16, 1);

    ASSERT_EQ(exp_multiset, multiset);
}

TEST(multiset, 2322) {
    text_t text{23, 22};
    multiset_t multiset(text.size() - 1);
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    multiset_t exp_multiset;
    exp_multiset.emplace_back(23, 22, 0);

    ASSERT_EQ(exp_multiset, multiset);
}


TEST(fast_multiset, 212181623541741623541321) {
    fast_text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    fast_multiset_t multiset(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    fast_multiset_t exp_multiset(alphabet_size);
    exp_multiset[1][0] = std::make_pair(3, 1);
    exp_multiset[2][0] = std::make_pair(0, 1);
    exp_multiset[2][1] = std::make_pair(1, 2);
    exp_multiset[3][0] = std::make_pair(3, 0);
    exp_multiset[4][2] = std::make_pair(0, 2);
    exp_multiset[4][3] = std::make_pair(2, 0);
    exp_multiset[5][0] = std::make_pair(0, 2);
    exp_multiset[5][1] = std::make_pair(2, 0);
    exp_multiset[6][0] = std::make_pair(0, 1);
    exp_multiset[6][3] = std::make_pair(1, 0);
    exp_multiset[7][0] = std::make_pair(1, 1);

    ASSERT_EQ(exp_multiset, multiset);
    ASSERT_EQ(exp_multiset.size(), multiset.size());
}

TEST(fast_multiset, 131261051171161051139) {
    fast_text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    fast_multiset_t multiset(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    fast_multiset_t exp_multiset(alphabet_size);
    exp_multiset[4][0] = std::make_pair(0, 1);
    exp_multiset[5][1] = std::make_pair(2, 0);
    exp_multiset[5][2] = std::make_pair(0, 2);
    exp_multiset[6][0] = std::make_pair(1, 0);
    exp_multiset[6][1] = std::make_pair(0, 2);
    exp_multiset[6][2] = std::make_pair(1, 0);
    exp_multiset[6][3] = std::make_pair(1, 1);
    exp_multiset[7][2] = std::make_pair(1, 0);
    exp_multiset[8][7] = std::make_pair(1, 0);

    ASSERT_EQ(exp_multiset, multiset);
    ASSERT_EQ(exp_multiset.size(), multiset.size());
}

TEST(fast_multiset, 18161517161514) {
    fast_text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    fast_multiset_t multiset(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    fast_multiset_t exp_multiset(alphabet_size);
    exp_multiset[1][0] = std::make_pair(1, 0);
    exp_multiset[2][1] = std::make_pair(2, 0);
    exp_multiset[3][1] = std::make_pair(0, 1);
    exp_multiset[3][2] = std::make_pair(1, 0);
    exp_multiset[4][2] = std::make_pair(1, 0);

    ASSERT_EQ(exp_multiset, multiset);
    ASSERT_EQ(exp_multiset.size(), multiset.size());
}

TEST(fast_multiset, 21201619) {
    fast_text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    fast_multiset_t multiset(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    fast_multiset_t exp_multiset(alphabet_size);
    exp_multiset[1][0] = std::make_pair(0, 1);
    exp_multiset[2][0] = std::make_pair(1, 0);
    exp_multiset[3][2] = std::make_pair(1, 0);

    ASSERT_EQ(exp_multiset, multiset);
    ASSERT_EQ(exp_multiset.size(), multiset.size());
}

TEST(fast_multiset, 2322) {
    fast_text_t text{1, 0};
    term_t alphabet_size = 2;
    fast_multiset_t multiset(alphabet_size);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);

    fast_multiset_t exp_multiset(alphabet_size);
    exp_multiset[1][0] = std::make_pair(1, 0);

    ASSERT_EQ(exp_multiset, multiset);
    ASSERT_EQ(exp_multiset.size(), multiset.size());
}
