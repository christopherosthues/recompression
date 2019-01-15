#include <gtest/gtest.h>

#include "defs.hpp"
#include "rlslp.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef parallel::recompression<var_t, term_t>::text_t text_t;

TEST(extract, empty) {
    text_t text = {};
    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size);

    std::string extr = rlslp.extract(0, 9);
    ASSERT_EQ("", extr);
}

TEST(extract, recompression) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, short_block) {
    text_t text = {2, 2, 2};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, one_block) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, two_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, three_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, four_blocks) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, pairs) {
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t, term_t> rlslp;
    parallel::recompression<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}
