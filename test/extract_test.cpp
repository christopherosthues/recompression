#include <gtest/gtest.h>

#include <sstream>

#include "defs.hpp"
#include "rlslp.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;

TEST(extract, extract) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {4, 1, 3, 3, 2};
    auto extr = rlslp.extract(6, 5);
    ASSERT_EQ(expected, extr);

    expected = {4, 4, 1, 3, 3};
    extr = rlslp.extract(5, 5);
    ASSERT_EQ(expected, extr);

    expected = {4, 4, 4, 1, 3, 3};
    extr = rlslp.extract(4, 6);
    ASSERT_EQ(expected, extr);

    expected = {1, 3, 3, 3, 4, 1, 3, 3, 2};
    extr = rlslp.extract(15, 9);
    ASSERT_EQ(expected, extr);

    expected = {1, 2, 1, 4, 4};
    extr = rlslp.extract(1, 5);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_blocks) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {4, 4, 4};
    auto extr = rlslp.extract(4, 3);
    ASSERT_EQ(expected, extr);

    expected = {4, 4, 4};
    extr = rlslp.extract(4, 3);
    ASSERT_EQ(expected, extr);

    expected = {3, 3};
    extr = rlslp.extract(8, 2);
    ASSERT_EQ(expected, extr);

    expected = {1, 1};
    extr = rlslp.extract(12, 2);
    ASSERT_EQ(expected, extr);

    expected = {3, 3, 3};
    extr = rlslp.extract(16, 3);
    ASSERT_EQ(expected, extr);

    expected = {3, 3};
    extr = rlslp.extract(21, 2);
    ASSERT_EQ(expected, extr);

    expected = {1, 1};
    extr = rlslp.extract(25, 2);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_partial_block_begin) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {4, 4, 4};
    auto extr = rlslp.extract(4, 3);
    ASSERT_EQ(expected, extr);

    expected = {4, 4};
    extr = rlslp.extract(4, 2);
    ASSERT_EQ(expected, extr);

    expected = {3};
    extr = rlslp.extract(8, 1);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_block_middle) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {3};
    auto extr = rlslp.extract(17, 1);
    ASSERT_EQ(expected, extr);

    expected = {4};
    extr = rlslp.extract(5, 1);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_block_end) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {4, 4};
    auto extr = rlslp.extract(5, 2);
    ASSERT_EQ(expected, extr);

    expected = {3, 3};
    extr = rlslp.extract(17, 2);
    ASSERT_EQ(expected, extr);

    expected = {3};
    extr = rlslp.extract(18, 1);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_block_included) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {4, 4, 4};
    auto extr = rlslp.extract(4, 3);
    ASSERT_EQ(expected, extr);

    expected = {1, 4, 4, 4, 1, 3};
    extr = rlslp.extract(3, 6);
    ASSERT_EQ(expected, extr);

    expected = {3, 3, 2};
    extr = rlslp.extract(8, 3);
    ASSERT_EQ(expected, extr);

    expected = {1, 3, 3, 3, 4};
    extr = rlslp.extract(15, 5);
    ASSERT_EQ(expected, extr);

    expected = {1, 3, 3, 3};
    extr = rlslp.extract(15, 4);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_start_pos_out_of_bound) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected;

    auto extr = rlslp.extract(32, 9);
    ASSERT_EQ(expected, extr);

    extr = rlslp.extract(60, 20);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_all) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1,
                       3, 2, 1};
    auto extr = rlslp.extract(0, 32);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_max) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1,
                       3, 2, 1};
    auto extr = rlslp.extract(0, 34);
    ASSERT_EQ(expected, extr);

    expected = {2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    extr = rlslp.extract(10, 34);
    ASSERT_EQ(expected, extr);

    expected = {2, 3, 1, 1, 4, 1, 3, 2, 1};
    extr = rlslp.extract(23, 20);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_single_char) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected;
    text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    for (size_t i = 0; i < text.size(); ++i) {
        expected = text[i];
        auto extr = rlslp.extract(i, 1);
//        std::cout << i << std::endl;
        ASSERT_EQ(expected, extr);
    }
}

TEST(extract, extract_empty) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected;
    auto extr = rlslp.extract(5, 0);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_empty_text) {
    text_t text = {};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    std::string expected;
    auto extr = rlslp.extract(5, 0);
    ASSERT_EQ(expected, extr);

    extr = rlslp.extract(0, 0);
    ASSERT_EQ(expected, extr);

    extr = rlslp.extract(0, 2);
    ASSERT_EQ(expected, extr);
}

TEST(extract, extract_all_substrings) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    recompression<var_t, term_t> recomp;
    rlslp<var_t, term_t> rlslp;
    recomp.recomp(text, rlslp, 5);

    text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    std::stringstream expected;
    for (size_t i = 0; i < text.size(); ++i) {
        for (size_t j = 1; j <= text.size() - i; ++j) {
            expected = std::stringstream{};
            for (size_t k = i; k < i+j; ++k) {
                expected << static_cast<char>(text[k]);
            }
            auto extr = rlslp.extract(i, j);
            ASSERT_EQ(expected.str(), extr);
        }
    }
}
