#include <gtest/gtest.h>

#include <iostream>
#include <utility>
#include <vector>

#include "defs.hpp"
#include "radix_sort.hpp"
#include "util.hpp"

#include "parallel_recompression.hpp"

using namespace recomp;

typedef var_t variable_t;
typedef recompression<var_t, term_t>::multiset_t multiset_t;


TEST(digits, output_digits) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::uint64_t val = (24 << 4) | (56 << 16);
    DLOG(INFO) << val;

    DLOG(INFO) << digits<std::uint64_t, 8>(val, 0);
    DLOG(INFO) << digits<std::uint64_t, 8>(val, 1);
    DLOG(INFO) << digits<std::uint64_t, 8>(val, 2);
    DLOG(INFO) << digits<std::uint64_t, 8>(val, 3);
//    std::cout << parallel::igits<std::uint64_t>(val, 4) << std::endl;
//    std::cout << parallel::igits<std::uint64_t>(val, 5) << std::endl;
//    std::cout << parallel::igits<std::uint64_t>(val, 6) << std::endl;
//    std::cout << parallel::igits<std::uint64_t>(val, 7) << std::endl;
}

TEST(digits, digits) {
    std::vector<variable_t> vec{23, 23423, 234333, 5758, 6994, 599, 9023, 2, 774, 563};

    const auto n_blocks = sizeof(variable_t) * CHAR_BIT / 4;
    DLOG(INFO) << "Number of blocks: " << n_blocks;

    ASSERT_EQ(8, n_blocks);

    std::array<std::vector<std::uint8_t>, n_blocks> exp_digit;
    exp_digit[0] = std::vector<std::uint8_t>{7, 15, 13, 14, 2, 7, 15, 2, 6, 3};
    exp_digit[1] = std::vector<std::uint8_t>{1, 7, 5, 7, 5, 5, 3, 0, 0, 3};
    exp_digit[2] = std::vector<std::uint8_t>{0, 11, 3, 6, 11, 2, 3, 0, 3, 2};
    exp_digit[3] = std::vector<std::uint8_t>{0, 5, 9, 1, 1, 0, 2, 0, 0, 0};
    exp_digit[4] = std::vector<std::uint8_t>{0, 0, 3, 0, 0, 0, 0, 0, 0, 0};
    exp_digit[5] = std::vector<std::uint8_t>(10, 0);
    exp_digit[6] = std::vector<std::uint8_t>(10, 0);
    exp_digit[7] = std::vector<std::uint8_t>(10, 0);

    std::array<std::vector<std::uint8_t>, n_blocks> digit;
    for (std::uint8_t i = 0; i < n_blocks; ++i) {
        for (size_t j = 0; j < vec.size(); ++j) {
            digit[i].emplace_back(digits<variable_t, 4>(vec[j], i));
        }
    }

    ASSERT_EQ(exp_digit, digit);
}

TEST(digits, digits_mask) {
    std::vector<variable_t> vec{23, 23423, 234333, 5758, 6994, 599, 9023, 2, 774, 563};

    const auto n_blocks = sizeof(variable_t) * CHAR_BIT / 4;
    DLOG(INFO) << "Number of blocks: " << n_blocks;
    variable_t mask = (1 << 4) - 1;

    ASSERT_EQ(8, n_blocks);

    std::array<std::vector<std::uint8_t>, n_blocks> exp_digit;
    exp_digit[0] = std::vector<std::uint8_t>{7, 15, 13, 14, 2, 7, 15, 2, 6, 3};
    exp_digit[1] = std::vector<std::uint8_t>{1, 7, 5, 7, 5, 5, 3, 0, 0, 3};
    exp_digit[2] = std::vector<std::uint8_t>{0, 11, 3, 6, 11, 2, 3, 0, 3, 2};
    exp_digit[3] = std::vector<std::uint8_t>{0, 5, 9, 1, 1, 0, 2, 0, 0, 0};
    exp_digit[4] = std::vector<std::uint8_t>{0, 0, 3, 0, 0, 0, 0, 0, 0, 0};
    exp_digit[5] = std::vector<std::uint8_t>(10, 0);
    exp_digit[6] = std::vector<std::uint8_t>(10, 0);
    exp_digit[7] = std::vector<std::uint8_t>(10, 0);

    std::array<std::vector<std::uint8_t>, n_blocks> digit;
    for (std::uint8_t i = 0; i < n_blocks; ++i) {
        for (size_t j = 0; j < vec.size(); ++j) {
            digit[i].emplace_back(digits<variable_t, 4>(vec[j], i, mask));
        }
    }

    ASSERT_EQ(exp_digit, digit);
}

TEST(lsd_radix_sort, par_lsd) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23423, 2133);
    vec.emplace_back(234333, 4);
    vec.emplace_back(5758, 66);
    vec.emplace_back(599, 3);
    vec.emplace_back(9023, 4456);
    vec.emplace_back(2, 4);
    vec.emplace_back(774, 4);
    vec.emplace_back(23423, 993);
    vec.emplace_back(563, 4);
    vec.emplace_back(2, 7);
    vec.emplace_back(9023, 778);
    vec.emplace_back(563, 2);
    vec.emplace_back(6994, 4);
    vec.emplace_back(2, 2);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    lsd_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(2, 2);
    exp_vec.emplace_back(2, 4);
    exp_vec.emplace_back(2, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(563, 2);
    exp_vec.emplace_back(563, 4);
    exp_vec.emplace_back(599, 3);
    exp_vec.emplace_back(774, 4);
    exp_vec.emplace_back(5758, 66);
    exp_vec.emplace_back(6994, 4);
    exp_vec.emplace_back(9023, 778);
    exp_vec.emplace_back(9023, 4456);
    exp_vec.emplace_back(23423, 993);
    exp_vec.emplace_back(23423, 2133);
    exp_vec.emplace_back(234333, 4);

//    exp_vec.emplace_back(2, 4);
//    exp_vec.emplace_back(23, 4);
//    exp_vec.emplace_back(563, 4);
//    exp_vec.emplace_back(599, 4);
//    exp_vec.emplace_back(774, 4);
//    exp_vec.emplace_back(5758, 4);
//    exp_vec.emplace_back(6994, 4);
//    exp_vec.emplace_back(9023, 4);
//    exp_vec.emplace_back(23423, 4);
//    exp_vec.emplace_back(234333, 4);

    ASSERT_EQ(exp_vec, vec);
}

TEST(lsd_radix_sort, all_equal) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    lsd_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);

    ASSERT_EQ(exp_vec, vec);
}

TEST(lsd_radix_sort, one_sec_diff) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 6);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    lsd_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(23, 6);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);

    ASSERT_EQ(exp_vec, vec);
}

TEST(lsd_radix_sort, one_diff) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(256, 4);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    lsd_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(256, 4);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_pair, par_lsd) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23423, 2133);
    vec.emplace_back(234333, 4);
    vec.emplace_back(5758, 66);
    vec.emplace_back(599, 3);
    vec.emplace_back(9023, 4456);
    vec.emplace_back(2, 4);
    vec.emplace_back(774, 4);
    vec.emplace_back(23423, 993);
    vec.emplace_back(563, 4);
    vec.emplace_back(2, 7);
    vec.emplace_back(9023, 778);
    vec.emplace_back(563, 2);
    vec.emplace_back(6994, 4);
    vec.emplace_back(2, 2);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(2, 2);
    exp_vec.emplace_back(2, 4);
    exp_vec.emplace_back(2, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(563, 2);
    exp_vec.emplace_back(563, 4);
    exp_vec.emplace_back(599, 3);
    exp_vec.emplace_back(774, 4);
    exp_vec.emplace_back(5758, 66);
    exp_vec.emplace_back(6994, 4);
    exp_vec.emplace_back(9023, 778);
    exp_vec.emplace_back(9023, 4456);
    exp_vec.emplace_back(23423, 993);
    exp_vec.emplace_back(23423, 2133);
    exp_vec.emplace_back(234333, 4);

//    exp_vec.emplace_back(2, 4);
//    exp_vec.emplace_back(23, 4);
//    exp_vec.emplace_back(563, 4);
//    exp_vec.emplace_back(599, 4);
//    exp_vec.emplace_back(774, 4);
//    exp_vec.emplace_back(5758, 4);
//    exp_vec.emplace_back(6994, 4);
//    exp_vec.emplace_back(9023, 4);
//    exp_vec.emplace_back(23423, 4);
//    exp_vec.emplace_back(234333, 4);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_pair, all_equal) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_pair, one_sec_diff) {
//    std::cout << (1 << 0) << "," << (1 << 1) << "," << (1 << 2) << "," << (1 << 3) << std::endl;
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 6);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(23, 6);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_pair, one_diff) {
    std::vector<std::pair<variable_t, variable_t>> vec;
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(256, 4);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);
    vec.emplace_back(23, 7);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::vector_blocks_to_string(vec);

    std::vector<std::pair<variable_t, variable_t>> exp_vec;
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(23, 7);
    exp_vec.emplace_back(256, 4);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_tuple, par_lsd) {
    multiset_t vec;
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23423, 2133, 0);
    vec.emplace_back(234333, 4, 0);
    vec.emplace_back(5758, 66, 1);
    vec.emplace_back(599, 3, 0);
    vec.emplace_back(9023, 4456, 0);
    vec.emplace_back(2, 4, 0);
    vec.emplace_back(774, 4, 1);
    vec.emplace_back(23423, 993, 1);
    vec.emplace_back(563, 4, 0);
    vec.emplace_back(2, 7, 0);
    vec.emplace_back(9023, 778, 0);
    vec.emplace_back(563, 2, 0);
    vec.emplace_back(6994, 4, 0);
    vec.emplace_back(2, 2, 0);

    DLOG(INFO) << util::multiset_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::multiset_to_string(vec);

    multiset_t exp_vec;
    exp_vec.emplace_back(2, 2, 0);
    exp_vec.emplace_back(2, 4, 0);
    exp_vec.emplace_back(2, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(563, 2, 0);
    exp_vec.emplace_back(563, 4, 0);
    exp_vec.emplace_back(599, 3, 0);
    exp_vec.emplace_back(774, 4, 1);
    exp_vec.emplace_back(5758, 66, 1);
    exp_vec.emplace_back(6994, 4, 0);
    exp_vec.emplace_back(9023, 778, 0);
    exp_vec.emplace_back(9023, 4456, 0);
    exp_vec.emplace_back(23423, 993, 1);
    exp_vec.emplace_back(23423, 2133, 0);
    exp_vec.emplace_back(234333, 4, 0);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_tuple, all_equal) {
    multiset_t vec;
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);

    DLOG(INFO) << util::multiset_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::multiset_to_string(vec);

    multiset_t exp_vec;
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_tuple, one_sec_diff) {
    multiset_t vec;
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 6, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);

    DLOG(INFO) << util::multiset_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::multiset_to_string(vec);

    multiset_t exp_vec;
    exp_vec.emplace_back(23, 6, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_tuple, one_diff) {
    multiset_t vec;
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(256, 4, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 1);
    vec.emplace_back(23, 7, 0);
    vec.emplace_back(23, 7, 0);

    DLOG(INFO) << util::multiset_to_string(vec);

    parallel::partitioned_radix_sort<>(vec);

    DLOG(INFO) << util::multiset_to_string(vec);

    multiset_t exp_vec;
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 1);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(23, 7, 0);
    exp_vec.emplace_back(256, 4, 1);

    ASSERT_EQ(exp_vec, vec);
}
