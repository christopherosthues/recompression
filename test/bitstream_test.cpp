#include <gtest/gtest.h>

#include <random>
#include <stdio.h>
#include <vector>

#include <io/bitistream.hpp>
#include <io/bitostream.hpp>

using namespace recomp;

TEST(bitstream, empty) {
    std::string file_name = "bv_empty.txt";
    std::vector<bool> bv = {};

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bit) {
    std::string file_name = "bit.txt";

    BitOStream bos{file_name};
    bos.write_bit(true);
    bos.close();

    BitIStream bis(file_name);
    bool bit = bis.read_bit();
    bis.close();

    ASSERT_EQ(true, bit);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector31) {
    std::string file_name = "bv0.txt";
    std::vector<bool> bv = {true, true, false, true, false, false, false, true, false, false, true, false, true, false,
                            true, true, true, true, false, false, false, true, false, false, true, false, true, false,
                            false, true, true};

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector32) {
    std::string file_name = "bv01.txt";
    std::vector<bool> bv = {true, true, false, true, false, false, false, true, false, false, true, false, true, false,
                            true, true, true, true, false, false, false, true, false, false, true, false, true, false,
                            false, true, true, false};

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector33) {
    std::string file_name = "bv02.txt";
    std::vector<bool> bv = {true, true, false, true, false, false, false, true, false, false, true, false, true, false,
                            true, true, true, true, false, false, false, true, false, false, true, false, true, false,
                            false, true, true, false, true};

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector110) {
    std::string file_name = "bv1.txt";
    std::vector<bool> bv = {true, true, false};

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector110100001) {
    std::string file_name = "bv2.txt";
    std::vector<bool> bv = {true, true, false, true, false, false, false, false, true};

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector61_1) {
    std::string file_name = "bv3.txt";
    std::vector<bool> bv(61, true);

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector100_1) {
    std::string file_name = "bv4.txt";
    std::vector<bool> bv(100, true);

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector100_10) {
    std::string file_name = "bv5.txt";
    std::vector<bool> bv(100, true);
    for (size_t i = 67; i < bv.size(); ++i) {
        bv[i] = false;
    }

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector_too_long_1) {
    std::string file_name = "bv6.txt";
    std::uint32_t len = -1;
    size_t length = len;
    std::vector<bool> bv(length + 1, true);
    bv[bv.size() - 1] = false;

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector_too_long_0) {
    std::string file_name = "bv7.txt";
    std::uint32_t len = -1;
    size_t length = len;
    std::vector<bool> bv(length, false);
    bv[bv.size() - 1] = true;

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector_random) {
    std::string file_name = "bv_random.txt";
    size_t length = 100000;
    std::vector<bool> bv(length);
    auto gen = std::bind(std::uniform_int_distribution<>(0, 1), std::default_random_engine());
    for (size_t i = 0; i < bv.size(); ++i) {
        bv[i] = gen();
    }

    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();

    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}
