
#pragma once

#include "rlslp.hpp"

namespace recomp {
namespace lce_query {

size_t lce_query(rlslp& rlslp, size_t i, size_t j);

size_t lce_query(rlslp& rlslp, size_t i, size_t j, variable_t nt_i, variable_t nt_j);

}  // namespace lce_query
}  // namespace recomp
