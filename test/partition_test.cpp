#include <gtest/gtest.h>

#define private public
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;
typedef recompression<var_t, term_t>::multiset_t multiset_t;
typedef recompression<var_t, term_t>::partition_t partition_t;
typedef recompression<var_t, term_t>::alphabet_t alphabet_t;

TEST(partition, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    multiset_t multiset(text.size() - 1);
    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    partition_t partition;
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);
    __gnu_parallel::sort(multiset.begin(), multiset.end(), __gnu_parallel::multiway_mergesort_tag());
    recomp.compute_partition(multiset, alphabet, partition);

    partition_t exp_partition;
    exp_partition[1] = true;
    exp_partition[2] = false;
    exp_partition[3] = true;
    exp_partition[4] = false;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = true;
    exp_partition[8] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 131261051171161051139) {
    text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    multiset_t multiset(text.size() - 1);
    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    partition_t partition;
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);
    __gnu_parallel::sort(multiset.begin(), multiset.end(), __gnu_parallel::multiway_mergesort_tag());
    recomp.compute_partition(multiset, alphabet, partition);

    partition_t exp_partition;
    exp_partition[3] = false;
    exp_partition[5] = false;
    exp_partition[6] = false;
    exp_partition[7] = false;
    exp_partition[9] = true;
    exp_partition[10] = true;
    exp_partition[11] = true;
    exp_partition[12] = true;
    exp_partition[13] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 18161517161514) {
    text_t text{18, 16, 15, 17, 16, 15, 14};
    multiset_t multiset(text.size() - 1);
    alphabet_t alphabet{14, 15, 16, 17, 18};
    partition_t partition;
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);
    __gnu_parallel::sort(multiset.begin(), multiset.end(), __gnu_parallel::multiway_mergesort_tag());
    recomp.compute_partition(multiset, alphabet, partition);

    partition_t exp_partition;
    exp_partition[14] = true;
    exp_partition[15] = false;
    exp_partition[16] = true;
    exp_partition[17] = true;
    exp_partition[18] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 21201619) {
    text_t text{21, 20, 16, 19};
    multiset_t multiset(text.size() - 1);
    alphabet_t alphabet{16, 19, 20, 21};
    partition_t partition;
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);
    __gnu_parallel::sort(multiset.begin(), multiset.end(), __gnu_parallel::multiway_mergesort_tag());
    recomp.compute_partition(multiset, alphabet, partition);

    partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 2322) {
    text_t text{23, 22};
    multiset_t multiset(text.size() - 1);
    alphabet_t alphabet{22, 23};
    partition_t partition;
    recompression<var_t, term_t> recomp;
    recomp.compute_multiset(text, multiset);
    __gnu_parallel::sort(multiset.begin(), multiset.end(), __gnu_parallel::multiway_mergesort_tag());
    recomp.compute_partition(multiset, alphabet, partition);

    partition_t exp_partition;
    exp_partition[22] = true;
    exp_partition[23] = false;

    ASSERT_EQ(exp_partition, partition);
}
