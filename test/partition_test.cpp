#include <gtest/gtest.h>

#define private public

#include "fast_recompression.hpp"
#include "partition.hpp"
#include "recompression.hpp"
#include "parallel_recompression.hpp"

using namespace recomp;

typedef recompression<var_t, term_t>::text_t text_t;
typedef recompression<var_t, term_t>::adj_list_t adj_list_t;
typedef recompression<var_t, term_t>::partition_t partition_t;
typedef recompression<var_t, term_t>::alphabet_t alphabet_t;

typedef recompression_fast<var_t, term_t>::text_t fast_text_t;
typedef recompression_fast<var_t, term_t>::adj_list_t fast_multiset_t;
typedef recompression_fast<var_t, term_t>::partition_t fast_partition_t;

typedef parallel::recompression<var_t, term_t>::text_t par_text_t;
typedef parallel::recompression<var_t, term_t>::adj_list_t par_multiset_t;
typedef parallel::recompression<var_t, term_t>::partition_t par_partition_t;
typedef parallel::recompression<var_t, term_t>::alphabet_t par_alphabet_t;

TEST(parallel_partition, 212181623541741623541321) {
    par_text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition<par_multiset_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
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

TEST(parallel_partition, 131261051171161051139) {
    par_text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition<par_multiset_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
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

TEST(parallel_partition, 18161517161514) {
    par_text_t text{18, 16, 15, 17, 16, 15, 14};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{14, 15, 16, 17, 18};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition<par_multiset_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
    exp_partition[14] = true;
    exp_partition[15] = false;
    exp_partition[16] = true;
    exp_partition[17] = true;
    exp_partition[18] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(parallel_partition, 21201619) {
    par_text_t text{21, 20, 16, 19};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{16, 19, 20, 21};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition<par_multiset_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(parallel_partition, 2322) {
    par_text_t text{23, 22};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{22, 23};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition<par_multiset_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
    exp_partition[22] = true;
    exp_partition[23] = false;

    ASSERT_EQ(exp_partition, partition);
}


TEST(full_parallel_partition, 212181623541741623541321) {
    par_text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition_full_parallel<var_t, par_multiset_t, par_alphabet_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
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

TEST(full_parallel_partition, 131261051171161051139) {
    par_text_t text{13, 12, 6, 10, 5, 11, 7, 11, 6, 10, 5, 11, 3, 9};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition_full_parallel<var_t, par_multiset_t, par_alphabet_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
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

TEST(full_parallel_partition, 18161517161514) {
    par_text_t text{18, 16, 15, 17, 16, 15, 14};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{14, 15, 16, 17, 18};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition_full_parallel<var_t, par_multiset_t, par_alphabet_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
    exp_partition[14] = true;
    exp_partition[15] = false;
    exp_partition[16] = true;
    exp_partition[17] = true;
    exp_partition[18] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(full_parallel_partition, 21201619) {
    par_text_t text{21, 20, 16, 19};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{16, 19, 20, 21};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition_full_parallel<var_t, par_multiset_t, par_alphabet_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(full_parallel_partition, 2322) {
    par_text_t text{23, 22};
    par_multiset_t multiset(text.size() - 1);
    par_alphabet_t alphabet{22, 23};
    par_partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    parallel::recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    parallel::compute_partition_full_parallel<var_t, par_multiset_t, par_alphabet_t, par_partition_t>(multiset, /*alphabet,*/ partition);

    par_partition_t exp_partition;
    exp_partition[22] = true;
    exp_partition[23] = false;

    ASSERT_EQ(exp_partition, partition);
}


TEST(partition, 212181623541741623541321) {
    text_t text{2, 1, 2, 1, 8, 1, 6, 2, 3, 5, 4, 1, 7, 4, 1, 6, 2, 3, 5, 4, 1, 3, 2, 1};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{1, 2, 3, 4, 5, 6, 7, 8};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, /*alphabet,*/ partition);

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
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{3, 5, 6, 7, 9, 10, 11, 12, 13};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, /*alphabet,*/ partition);

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
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{14, 15, 16, 17, 18};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, /*alphabet,*/ partition);

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
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{16, 19, 20, 21};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, /*alphabet,*/ partition);

    partition_t exp_partition;
    exp_partition[16] = false;
    exp_partition[19] = true;
    exp_partition[20] = true;
    exp_partition[21] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(partition, 2322) {
    text_t text{23, 22};
    adj_list_t multiset(text.size() - 1);
    alphabet_t alphabet{22, 23};
    partition_t partition;
    for (const auto& a : alphabet) {
        partition[a] = false;
    }
    recompression<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    ips4o::parallel::sort(multiset.begin(), multiset.end());
    recomp.compute_partition(multiset, /*alphabet,*/ partition);

    partition_t exp_partition;
    exp_partition[22] = true;
    exp_partition[23] = false;

    ASSERT_EQ(exp_partition, partition);
}


TEST(fast_compute_partition, 212181623541741623541321) {
    fast_text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    fast_multiset_t multiset(alphabet_size);
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    recomp.compute_partition(multiset, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;
    exp_partition[3] = true;
    exp_partition[7] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 131261051171161051139) {
    fast_text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    fast_multiset_t multiset(alphabet_size);
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    recomp.compute_partition(multiset, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[4] = true;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 18161517161514) {
    fast_text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    fast_multiset_t multiset(alphabet_size);
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    recomp.compute_partition(multiset, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;
    exp_partition[4] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 21201619) {
    fast_text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    fast_multiset_t multiset(alphabet_size);
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    recomp.compute_partition(multiset, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;
    exp_partition[2] = true;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_compute_partition, 2322) {
    fast_text_t text{1, 0};
    term_t alphabet_size = 2;
    fast_multiset_t multiset(alphabet_size);
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.compute_adj_list(text, multiset);
    recomp.compute_partition(multiset, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[1] = true;

    ASSERT_EQ(exp_partition, partition);
}


TEST(fast_partition, 212181623541741623541321) {
    fast_text_t text{1, 0, 1, 0, 7, 0, 5, 1, 2, 4, 3, 0, 6, 3, 0, 5, 1, 2, 4, 3, 0, 2, 1, 0};
    term_t alphabet_size = 8;
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;
    exp_partition[1] = false;
    exp_partition[2] = true;
    exp_partition[3] = false;
    exp_partition[4] = true;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 131261051171161051139) {
    fast_text_t text{8, 7, 2, 5, 1, 6, 3, 6, 2, 5, 1, 6, 0, 4};
    term_t alphabet_size = 9;
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = false;
    exp_partition[1] = false;
    exp_partition[2] = false;
    exp_partition[3] = false;
    exp_partition[4] = true;
    exp_partition[5] = true;
    exp_partition[6] = true;
    exp_partition[7] = true;
    exp_partition[8] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 18161517161514) {
    fast_text_t text{4, 2, 1, 3, 2, 1, 0};
    term_t alphabet_size = 5;
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;
    exp_partition[1] = false;
    exp_partition[2] = true;
    exp_partition[3] = true;
    exp_partition[4] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 21201619) {
    fast_text_t text{3, 2, 0, 1};
    term_t alphabet_size = 4;
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = false;
    exp_partition[1] = true;
    exp_partition[2] = true;
    exp_partition[3] = false;

    ASSERT_EQ(exp_partition, partition);
}

TEST(fast_partition, 2322) {
    fast_text_t text{1, 0};
    term_t alphabet_size = 2;
    fast_partition_t partition(alphabet_size, false);
    recompression_fast<var_t, term_t> recomp;
    recomp.partition(text, alphabet_size, partition);

    fast_partition_t exp_partition(alphabet_size, false);
    exp_partition[0] = true;
    exp_partition[1] = false;

    ASSERT_EQ(exp_partition, partition);
}
