#include <gtest/gtest.h>

#include <stdio.h>
#include <vector>

#include <io/bitistream.hpp>
#include <io/bitostream.hpp>

using namespace recomp;

TEST(bitstream, empty) {
    std::string file_name = "test.txt";
    int j = 5;
    std::ofstream ofs(file_name.c_str());
    ofs << j;
    ofs.close();

    int i;
    std::ifstream ifs(file_name.c_str());
    ifs >> i;
    ASSERT_EQ(j, i);

    remove(file_name.c_str());
}

TEST(bitstream, bitvector) {
    std::string file_name = "bv.txt";
    std::vector<bool> bv = {true, true, false};

    std::cout << "Write bv" << std::endl;
    BitOStream bos{file_name};
    bos.write_bitvector_compressed(bv);
    bos.close();
    std::cout << "bv written" << std::endl;

    std::cout << "Read bv" << std::endl;
    BitIStream bis(file_name);
    std::vector<bool> bv_in = bis.read_bitvector_compressed();
    bis.close();
    std::cout << "bv read" << std::endl;

    ASSERT_EQ(bv, bv_in);

    remove(file_name.c_str());
}
