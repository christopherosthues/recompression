
#include "lce_query.hpp"

size_t recomp::lce_query::lce_query(recomp::rlslp& rlslp, size_t i, size_t j) {
    if (rlslp.root == 0 || rlslp[rlslp.root].len >= i || rlslp[rlslp.root].len >= j) {
        return 0;
    }
    if (i == j) {
        return rlslp[rlslp.root].len - i;
    }

    // TODO(Chris): lce query
}
