#include <gtest/gtest.h>

#include "defs.hpp"
#include "lce_query.hpp"
#include "rlslp.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;

TEST(lcequery, lcequery_empty) {
    text_t text;
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query<var_t, term_t>(rlslp, 0, 10);
    ASSERT_EQ(0, lceq);
}

TEST(lcequery, lcequery_zero) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 1, 0);
    ASSERT_EQ(0, lceq);
}

TEST(lcequery, lcequery_max_len_i) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 32, 5);
    ASSERT_EQ(0, lceq);
}

TEST(lcequery, lcequery_max_len_j) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 5, 32);
    ASSERT_EQ(0, lceq);
}

TEST(lcequery, lcequery) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 6, 14);
//    ASSERT_EQ(1, lceq);
    ASSERT_EQ(4, lceq);
}

TEST(lcequery, lcequery_long) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 6, 19);
//    ASSERT_EQ(1, lceq);
    ASSERT_EQ(11, lceq);
}

TEST(lcequery, lcequery_asso) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq_ij = lce_query::lce_query(rlslp, 6, 14);
    auto lceq_ji = lce_query::lce_query(rlslp, 14, 6);
    ASSERT_EQ(4, lceq_ji);
//    ASSERT_EQ(1, lceq_ji);
    ASSERT_EQ(lceq_ij, lceq_ji);
}

TEST(lcequery, lcequery_complete_text) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    auto t_size = text.size();
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 0, 0);
    ASSERT_EQ(t_size, lceq);
}

TEST(lcequery, lcequery_same_position) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    auto t_size = text.size();
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 10, 10);
    ASSERT_EQ(t_size - 10, lceq);
}

TEST(lcequery, lcequery_compressed_begin) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 2, 0);
    ASSERT_EQ(2, lceq);
}

TEST(lcequery, lcequery_compressed_within) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 3, 1);
    ASSERT_EQ(1, lceq);
}

TEST(lcequery, lcequery_compressed_diff) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 2, 1);
    ASSERT_EQ(0, lceq);
}

TEST(lcequery, lcequery_compressed_in_tree) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 8, 21);
    ASSERT_EQ(9, lceq);

    lceq = lce_query::lce_query(rlslp, 10, 23);
    ASSERT_EQ(7, lceq);
}

TEST(lcequery, lcequery_different_block_same_symbol) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1, 2, 1,
                   2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    auto lceq = lce_query::lce_query(rlslp, 1, 31);
    ASSERT_EQ(3, lceq);

    lceq = lce_query::lce_query(rlslp, 1, 35);
    ASSERT_EQ(1, lceq);

    lceq = lce_query::lce_query(rlslp, 33, 35);
    ASSERT_EQ(1, lceq);

    lceq = lce_query::lce_query(rlslp, 31, 33);
    ASSERT_EQ(3, lceq);
}

size_t lce_query_naive(size_t i, size_t j, text_t &text) {
    size_t lceq = 0;
    auto iter_i = text.begin();
    std::advance(iter_i, i);
    auto iter_j = text.begin();
    std::advance(iter_j, j);
    while (iter_i != text.end() && iter_j != text.end() && (*iter_i) == (*iter_j)) {
        lceq++;
        ++iter_i;
        ++iter_j;
    }
    return lceq;
}

TEST(lcequery, lcequery_complete) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    text_t text_naive = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp);

    for (size_t i = 0; i < text.size(); ++i) {
        for (size_t j = 0; j < text.size(); ++j) {
//            std::cout << "recompression_list" << std::endl;
            auto lceq = lce_query::lce_query(rlslp, i, j);
//            std::cout << "recompression_list finished" << std::endl;
//            std::cout << "naive" << std::endl;
            size_t naive = lce_query_naive(i, j, text_naive);
//            std::cout << "naive finished" << std::endl;
            ASSERT_EQ(naive, lceq);
        }
    }
}
