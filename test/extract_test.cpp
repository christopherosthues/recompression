#include <gtest/gtest.h>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/parallel_recompression.hpp"
#include "recompression/util.hpp"

using namespace recomp;

typedef parallel::parallel_recompression<var_t>::text_t text_t;

TEST(extract, empty) {
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 0;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    std::string extr = rlslp.extract(0, 9);
    ASSERT_EQ("", extr);
}

TEST(extract, recompression) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, short_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{'a', 'a', 'a'});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 255;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, one_block) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, two_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, three_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, four_blocks) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 5;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}

TEST(extract, pairs) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    std::string exp_string;
    exp_string.resize(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        exp_string[i] = static_cast<char>(text[i]);
    }

    rlslp<var_t> rlslp;
    parallel::parallel_recompression<var_t> recomp;
    term_t alphabet_size = 3;
    recomp.recomp(text, rlslp, alphabet_size, 4);

    for (size_t i = 0; i < exp_string.size(); ++i) {
        for (size_t len = 0; len < exp_string.size(); ++len) {
            std::string extr = rlslp.extract(i, len);
            std::string expected = exp_string.substr(i, len);
            ASSERT_EQ(expected, extr);
        }
    }
}
