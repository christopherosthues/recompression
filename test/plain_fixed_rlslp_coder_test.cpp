#include <gtest/gtest.h>

#include <stdio.h>

#include "recompression/defs.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/parallel_recompression.hpp"
#include "recompression/coders/plain_fixed_rlslp_coder.hpp"
#include "recompression/io/bitistream.hpp"
#include "recompression/io/bitostream.hpp"
#include "recompression/util.hpp"

using namespace recomp;

typedef recompression<var_t>::text_t text_t;

TEST(plain_fixed_rlslp_coder, empty) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, CHAR_ALPHABET, 4);

    std::cout << "recomp finished" << std::endl;

    std::string file_name = "empty";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, terminal) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{112});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, CHAR_ALPHABET, 4);

    std::string file_name = "terminal";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, short_block) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 113, 4);

    std::string file_name = "short_block";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, short_block3) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{112, 112, 112});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 113, 4);

    std::string file_name = "short_block3";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, recompression) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 5, 4);

    std::string file_name = "recompression";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, one_block) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "one_block";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, two_blocks) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "two_blocks";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, three_blocks) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "three_blocks";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, four_blocks) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "four_blocks";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, repeated_pair) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "repeated_pair";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, repeated_pair_same_occ) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 3, 4);

    std::string file_name = "repeated_pair_same";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}

TEST(plain_fixed_rlslp_coder, left_end) {
    recomp::rlslp<var_t> rlslp;
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    parallel::parallel_recompression<var_t> recomp;
    recomp.recomp(text, rlslp, 4, 4);

    std::string file_name = "left_end";
    coder::PlainFixedRLSLPCoder::Encoder enc = {file_name};
    enc.encode<var_t>(rlslp);

    coder::PlainFixedRLSLPCoder::Decoder dec = {file_name};
    recomp::rlslp<var_t> in_rlslp = dec.decode();

    ASSERT_EQ(rlslp, in_rlslp);
    ASSERT_EQ(rlslp.derive_text(), in_rlslp.derive_text());

    file_name += coder::PlainFixedRLSLPCoder::k_extension;
    remove(file_name.c_str());
}
