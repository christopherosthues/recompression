#include <gtest/gtest.h>

#define private public
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;
typedef recompression<var_t, term_t>::multiset_t multiset_t;

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
