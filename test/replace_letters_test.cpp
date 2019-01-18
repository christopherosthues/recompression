#include <gtest/gtest.h>

#include <glog/logging.h>

#define private public

#include "fast_recompression.hpp"
#include "util.hpp"

using namespace recomp;

typedef recompression_fast<var_t, term_t>::text_t text_t;

TEST(replace_letters, 212141323141341323141321) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 4, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = 5;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 3, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 4;

    LOG(INFO) << "rlslp: " << std::to_string(rlslp);
    LOG(INFO) << "exp_rlslp: " << std::to_string(exp_rlslp);

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 212141323181341323141321) {
    text_t text = {2, 1, 2, 1, 4, 1, 3, 2, 3, 1, 8, 1, 3, 4, 1, 3, 2, 3, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 9;
    rlslp.terminals = 9;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 0, 2, 1, 2, 0, 4, 0, 2, 3, 0, 2, 1, 2, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4, 8};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 9;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 5;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 21214441332311413334133231141321) {
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 5;
    rlslp.terminals = 5;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 0, 1, 0, 3, 3, 3, 0, 2, 2, 1, 2, 0, 0, 3, 0, 2, 2, 2, 3, 0, 2, 2, 1, 2, 0, 0, 3, 0, 2, 1, 0};
    std::vector<var_t> exp_mapping = {1, 2, 3, 4};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 5;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 4;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 222222222222222222222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = 3;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<var_t> exp_mapping = {2};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 1;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 22222222211111112222) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = 3;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
    std::vector<var_t> exp_mapping = {1, 2};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 2;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}

TEST(replace_letters, 2222222221111111222200) {
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    rlslp<var_t, term_t> rlslp;
    recompression_fast<var_t, term_t> recomp;
    term_t alphabet_size = 3;
    rlslp.terminals = 3;

    std::vector<var_t> mapping;

    recomp.replace_letters(text, alphabet_size, mapping);

    text_t exp_text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    std::vector<var_t> exp_mapping = {0, 1, 2};
    recomp::rlslp<var_t, term_t> exp_rlslp;
    exp_rlslp.terminals = 3;
    exp_rlslp.root = 0;
    term_t exp_alphabet_size = 3;

    ASSERT_EQ(exp_text, text);
    ASSERT_EQ(exp_rlslp, rlslp);
    ASSERT_EQ(exp_alphabet_size, alphabet_size);
    ASSERT_EQ(exp_mapping, mapping);
}
