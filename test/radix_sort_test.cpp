#include <gtest/gtest.h>

#include <iostream>
#include <utility>
#include <vector>

#include <ips4o.hpp>

#include "recompression/defs.hpp"
#include "recompression/radix_sort.hpp"
#include "recompression/util.hpp"

#include "recompression/parallel_recompression.hpp"

using namespace recomp;

typedef var_t variable_t;
typedef std::tuple<variable_t, variable_t, bool> adj_t;
typedef std::vector<adj_t> adj_list_t;

typedef parallel::parallel_recompression<recomp::var_t, recomp::term_t>::text_t text_t;
typedef size_t pair_t;
typedef std::pair<variable_t, size_t> block_t;


//TEST(digits, output_digits) {
//    std::uint64_t val = (24 << 4) | (56 << 16);
//    DLOG(INFO) << val;
//
//    DLOG(INFO) << digits<std::uint64_t, 8>(val, 0);
//    DLOG(INFO) << digits<std::uint64_t, 8>(val, 1);
//    DLOG(INFO) << digits<std::uint64_t, 8>(val, 2);
//    DLOG(INFO) << digits<std::uint64_t, 8>(val, 3);
//}

TEST(digits, digits) {
    std::vector<variable_t> vec{23, 23423, 234333, 5758, 6994, 599, 9023, 2, 774, 563};

    const auto n_blocks = sizeof(variable_t) * CHAR_BIT / 4;
//    DLOG(INFO) << "Number of blocks: " << n_blocks;

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
//    DLOG(INFO) << "Number of blocks: " << n_blocks;
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

    lsd_radix_sort<>(vec);

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

    ASSERT_EQ(exp_vec, vec);
}

TEST(lsd_radix_sort, all_equal) {
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

    lsd_radix_sort<>(vec);

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

    lsd_radix_sort<>(vec);

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

    lsd_radix_sort<>(vec);

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

    parallel::partitioned_radix_sort<>(vec);

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

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs_pair, all_equal) {
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

    parallel::partitioned_radix_sort<>(vec);

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

    parallel::partitioned_radix_sort<>(vec);

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

    parallel::partitioned_radix_sort<>(vec);

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
    adj_list_t vec;
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

    parallel::partitioned_radix_sort<>(vec);

    adj_list_t exp_vec;
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
    adj_list_t vec;
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

    parallel::partitioned_radix_sort<>(vec);

    adj_list_t exp_vec;
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
    adj_list_t vec;
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

    parallel::partitioned_radix_sort<>(vec);

    adj_list_t exp_vec;
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
    adj_list_t vec;
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

    parallel::partitioned_radix_sort<>(vec);

    adj_list_t exp_vec;
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

TEST(pprs, par_lsd) {
    std::vector<variable_t> vec;
    vec.emplace_back(23);
    vec.emplace_back(23423);
    vec.emplace_back(234333);
    vec.emplace_back(5758);
    vec.emplace_back(599);
    vec.emplace_back(9023);
    vec.emplace_back(2);
    vec.emplace_back(774);
    vec.emplace_back(23423);
    vec.emplace_back(563);
    vec.emplace_back(2);
    vec.emplace_back(9023);
    vec.emplace_back(563);
    vec.emplace_back(6994);
    vec.emplace_back(2);

    parallel::partitioned_radix_sort<variable_t, 8>(vec);

    std::vector<variable_t> exp_vec;
    exp_vec.emplace_back(2);
    exp_vec.emplace_back(2);
    exp_vec.emplace_back(2);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(563);
    exp_vec.emplace_back(563);
    exp_vec.emplace_back(599);
    exp_vec.emplace_back(774);
    exp_vec.emplace_back(5758);
    exp_vec.emplace_back(6994);
    exp_vec.emplace_back(9023);
    exp_vec.emplace_back(9023);
    exp_vec.emplace_back(23423);
    exp_vec.emplace_back(23423);
    exp_vec.emplace_back(234333);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs, all_equal) {
    std::vector<variable_t> vec;
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);

    parallel::partitioned_radix_sort<variable_t, 8>(vec);

    std::vector<variable_t> exp_vec;
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs, one_sec_diff) {
    std::vector<variable_t> vec;
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);

    parallel::partitioned_radix_sort<variable_t, 8>(vec);

    std::vector<variable_t> exp_vec;
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);

    ASSERT_EQ(exp_vec, vec);
}

TEST(pprs, one_diff) {
    std::vector<variable_t> vec;
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(256);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);
    vec.emplace_back(23);

    parallel::partitioned_radix_sort<variable_t, 8>(vec);

    std::vector<variable_t> exp_vec;
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(23);
    exp_vec.emplace_back(256);

    ASSERT_EQ(exp_vec, vec);
}



TEST(parallel_bucket_sort, par_lsd) {
    adj_list_t vec;
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23423, 2133);
    vec.emplace_back(0, 234333, 4);
    vec.emplace_back(1, 5758, 66);
    vec.emplace_back(0, 599, 3);
    vec.emplace_back(0, 9023, 4456);
    vec.emplace_back(0, 2, 4);
    vec.emplace_back(1, 774, 4);
    vec.emplace_back(1, 23423, 993);
    vec.emplace_back(0, 563, 4);
    vec.emplace_back(0, 2, 7);
    vec.emplace_back(0, 9023, 778);
    vec.emplace_back(0, 563, 2);
    vec.emplace_back(0, 6994, 4);
    vec.emplace_back(0, 2, 2);

    size_t begin = 12;
    parallel::bucket_sort(vec);

    adj_list_t exp_vec;
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23423, 2133);
    exp_vec.emplace_back(0, 234333, 4);
    exp_vec.emplace_back(0, 599, 3);
    exp_vec.emplace_back(0, 9023, 4456);
    exp_vec.emplace_back(0, 2, 4);
    exp_vec.emplace_back(0, 563, 4);
    exp_vec.emplace_back(0, 2, 7);
    exp_vec.emplace_back(0, 9023, 778);
    exp_vec.emplace_back(0, 563, 2);
    exp_vec.emplace_back(0, 6994, 4);
    exp_vec.emplace_back(0, 2, 2);
    exp_vec.emplace_back(1, 5758, 66);
    exp_vec.emplace_back(1, 774, 4);
    exp_vec.emplace_back(1, 23423, 993);

    ASSERT_EQ(exp_vec, vec);

    auto iter = vec.begin();
    std::advance(iter, begin);
    ips4o::parallel::sort(vec.begin(), iter);
    ips4o::parallel::sort(iter, vec.end());

    adj_list_t exp_vec_ips;
    exp_vec_ips.emplace_back(0, 2, 2);
    exp_vec_ips.emplace_back(0, 2, 4);
    exp_vec_ips.emplace_back(0, 2, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 563, 2);
    exp_vec_ips.emplace_back(0, 563, 4);
    exp_vec_ips.emplace_back(0, 599, 3);
    exp_vec_ips.emplace_back(0, 6994, 4);
    exp_vec_ips.emplace_back(0, 9023, 778);
    exp_vec_ips.emplace_back(0, 9023, 4456);
    exp_vec_ips.emplace_back(0, 23423, 2133);
    exp_vec_ips.emplace_back(0, 234333, 4);
    exp_vec_ips.emplace_back(1, 774, 4);
    exp_vec_ips.emplace_back(1, 5758, 66);
    exp_vec_ips.emplace_back(1, 23423, 993);

    ASSERT_EQ(exp_vec_ips, vec);
}

TEST(parallel_bucket_sort, all_equal) {
    adj_list_t vec;
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);

    size_t begin = 8;
    parallel::bucket_sort(vec);

    adj_list_t exp_vec;
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);

    ASSERT_EQ(exp_vec, vec);

    auto iter = vec.begin();
    std::advance(iter, begin);
    ips4o::parallel::sort(vec.begin(), iter);
    ips4o::parallel::sort(iter, vec.end());

    adj_list_t exp_vec_ips;
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);


    ASSERT_EQ(exp_vec_ips, vec);
}

TEST(parallel_bucket_sort, one_sec_diff) {
    adj_list_t vec;
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 6);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);

    size_t begin = 8;
    parallel::bucket_sort(vec);

    adj_list_t exp_vec;
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 6);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);

    ASSERT_EQ(exp_vec, vec);

    auto iter = vec.begin();
    std::advance(iter, begin);
    ips4o::parallel::sort(vec.begin(), iter);
    ips4o::parallel::sort(iter, vec.end());

    adj_list_t exp_vec_ips;
    exp_vec_ips.emplace_back(0, 23, 6);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);

    ASSERT_EQ(exp_vec_ips, vec);
}

TEST(parallel_bucket_sort, one_diff) {
    adj_list_t vec;
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 256, 4);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(1, 23, 7);
    vec.emplace_back(0, 23, 7);
    vec.emplace_back(0, 23, 7);

    size_t begin = 7;
    parallel::bucket_sort(vec);

    adj_list_t exp_vec;
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(0, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 256, 4);
    exp_vec.emplace_back(1, 23, 7);
    exp_vec.emplace_back(1, 23, 7);

    ASSERT_EQ(exp_vec, vec);

    auto iter = vec.begin();
    std::advance(iter, begin);
    ips4o::parallel::sort(vec.begin(), iter);
    ips4o::parallel::sort(iter, vec.end());

    adj_list_t exp_vec_ips;
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(0, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 23, 7);
    exp_vec_ips.emplace_back(1, 256, 4);

    ASSERT_EQ(exp_vec_ips, vec);
}


TEST(pprs, pairs_less_productions) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 3});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{1, 3, 5, 7});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{1, 3, 5, 7});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_repeated_pair) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_repeated_pair_same_occ) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_left_end) {
    text_t text = util::create_ui_vector(std::vector<var_t>{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 6, 8, 11, 13, 15, 17, 19});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 6, 8, 11, 13, 15, 17, 19});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_212181623541741623541321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 7, 10, 13, 16, 19, 22});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 7, 10, 13, 16, 19, 22});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_131261051171161051139) {
    text_t text = util::create_ui_vector(std::vector<var_t>{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 6, 8, 10, 12});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 4, 6, 8, 10, 12});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_18161517161514) {
    text_t text = util::create_ui_vector(std::vector<var_t>{18, 16, 15, 17, 16, 15, 14});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 5});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0, 2, 5});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_21201619) {
    text_t text = util::create_ui_vector(std::vector<var_t>{21, 20, 16, 19});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0, 2});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0, 2});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}

TEST(pprs, pairs_2322) {
    text_t text = util::create_ui_vector(std::vector<var_t>{23, 22});
    ui_vector<size_t> pairs = util::create_ui_vector(std::vector<size_t>{0});

    parallel::partitioned_parallel_radix_sort_pairs<text_t, variable_t, size_t, 8>(text, pairs, 4);

    ui_vector<size_t> exp_pairs = util::create_ui_vector(std::vector<size_t>{0});

    auto sort_cond = [&](const size_t& i, const size_t& j) {
        auto char_i = text[i];
        auto char_i1 = text[i + 1];
        auto char_j = text[j];
        auto char_j1 = text[j + 1];
        if (char_i == char_j) {
            return char_i1 < char_j1;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_pairs.begin(), exp_pairs.end(), sort_cond, 4);

    ASSERT_EQ(exp_pairs, pairs);
}


TEST(pprs, blocks_21214441332311413334133231141321) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 1, 2, 1, 4, 4, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 3, 3, 4, 1, 3, 3, 2, 3, 1, 1, 4, 1, 3, 2, 1});
    std::vector<std::pair<variable_t, size_t>> p;
    p.emplace_back(3, 4);
    p.emplace_back(2, 8);
    p.emplace_back(2, 12);
    p.emplace_back(3, 16);
    p.emplace_back(2, 21);
    p.emplace_back(2, 25);
    ui_vector<block_t> blocks = util::create_ui_vector(p);

    parallel::partitioned_parallel_radix_sort_blocks<text_t, variable_t, block_t, 8>(text, blocks, 4);

    ui_vector<block_t> exp_blocks = util::create_ui_vector(p);

    auto sort_cond = [&](const block_t& i, const block_t& j) {
        auto char_i = text[i.second];
        auto char_j = text[j.second];
        if (char_i == char_j) {
            return i.first < j.first;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_blocks.begin(), exp_blocks.end(), sort_cond, 4);

    ASSERT_EQ(exp_blocks, blocks);
}

TEST(pprs, blocks_222222222222222222222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2});
    std::vector<std::pair<variable_t, size_t>> p;
    p.emplace_back(21, 0);
    ui_vector<block_t> blocks = util::create_ui_vector(p);

    parallel::partitioned_parallel_radix_sort_blocks<text_t, variable_t, block_t, 8>(text, blocks, 4);

    ui_vector<block_t> exp_blocks = util::create_ui_vector(p);

    auto sort_cond = [&](const block_t& i, const block_t& j) {
        auto char_i = text[i.second];
        auto char_j = text[j.second];
        if (char_i == char_j) {
            return i.first < j.first;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_blocks.begin(), exp_blocks.end(), sort_cond, 4);

    ASSERT_EQ(exp_blocks, blocks);
}

TEST(pprs, blocks_22222222211111112222) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2});
    std::vector<std::pair<variable_t, size_t>> p;
    p.emplace_back(9, 0);
    p.emplace_back(7, 9);
    p.emplace_back(4, 16);
    ui_vector<block_t> blocks = util::create_ui_vector(p);

    parallel::partitioned_parallel_radix_sort_blocks<text_t, variable_t, block_t, 8>(text, blocks, 4);

    ui_vector<block_t> exp_blocks = util::create_ui_vector(p);

    auto sort_cond = [&](const block_t& i, const block_t& j) {
        auto char_i = text[i.second];
        auto char_j = text[j.second];
        if (char_i == char_j) {
            return i.first < j.first;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_blocks.begin(), exp_blocks.end(), sort_cond, 4);

    ASSERT_EQ(exp_blocks, blocks);
}

TEST(pprs, blocks_2222222221111111222200) {
    text_t text = util::create_ui_vector(std::vector<var_t>{2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0});
    std::vector<std::pair<variable_t, size_t>> p;
    p.emplace_back(9, 0);
    p.emplace_back(7, 9);
    p.emplace_back(4, 16);
    p.emplace_back(2, 20);
    ui_vector<block_t> blocks = util::create_ui_vector(p);

    parallel::partitioned_parallel_radix_sort_blocks<text_t, variable_t, block_t, 8>(text, blocks, 4);

    ui_vector<block_t> exp_blocks = util::create_ui_vector(p);

    auto sort_cond = [&](const block_t& i, const block_t& j) {
        auto char_i = text[i.second];
        auto char_j = text[j.second];
        if (char_i == char_j) {
            return i.first < j.first;
        } else {
            return char_i < char_j;
        }
    };
    ips4o::parallel::sort(exp_blocks.begin(), exp_blocks.end(), sort_cond, 4);

    ASSERT_EQ(exp_blocks, blocks);
}
