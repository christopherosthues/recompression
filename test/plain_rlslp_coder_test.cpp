#include <gtest/gtest.h>

#include <stdio.h>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/parallel_recompression.hpp"
#include "recompression/coders/plain_rlslp_coder.hpp"
#include "recompression/io/bitistream.hpp"
#include "recompression/io/bitostream.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;

const std::string extension = ".plain";

TEST(plain_rlslp_coder, empty) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, CHAR_ALPHABET, 4);

    std::cout << "recomp finished" << std::endl;

    std::string file_name = "empty";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, terminal) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {112};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, CHAR_ALPHABET, 4);

    std::string file_name = "terminal";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, short_block) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {112, 112};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 113, 4);

    std::string file_name = "short_block";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, short_block3) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {112, 112, 112};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 113, 4);

    std::string file_name = "short_block3";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, recompression) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 5, 4);

    std::string file_name = "recompression";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, one_block) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "one_block";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, two_blocks) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "two_blocks";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, three_blocks) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "three_blocks";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, four_blocks) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "four_blocks";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, repeated_pair) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "repeated_pair";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, repeated_pair_same_occ) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "repeated_pair_same";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_rlslp_coder, left_end) {
    recomp::rlslp<var_t, term_t> rlslp;
    text_t text = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1};
    parallel::parallel_recompression<var_t, term_t> recomp;
    recomp.recomp(text, rlslp, 4, 4);

    std::string file_name = "left_end";
    coder::PlainRLSLPCoder::Encoder enc{file_name};
    enc.encode<var_t, term_t>(rlslp);

    coder::PlainRLSLPCoder::Decoder dec{file_name};
    recomp::rlslp<var_t, term_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainRLSLPCoder::k_extension;
    remove(file_name.c_str());
}
