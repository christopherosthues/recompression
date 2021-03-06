set(SOURCES
        src/recompression/parallel_recompression.cpp
        src/recompression/experimental/parallel_lock_recompression.cpp
        src/recompression/rlslp.cpp
        src/recompression/parallel_lp_recompression.cpp
        src/recompression/recompression.cpp
        src/recompression/fast_recompression.cpp
        src/recompression/util.cpp
        src/recompression/experimental/parallel_order_less_recompression.cpp
        src/recompression/experimental/parallel_gr2_recompression.cpp
        src/recompression/experimental/parallel_parhip_recompression.cpp
        src/recompression/parallel_rnd_recompression.cpp
        src/recompression/parallel_rnddir_recompression.cpp
        src/recompression/lce_query.cpp
        src/recompression/radix_sort.cpp
        src/recompression/experimental/parallel_order_great_recompression.cpp
        src/recompression/parallel_ls_recompression.cpp
        src/recompression/experimental/parallel_ls3_recompression.cpp
        src/recompression/experimental/parallel_ls5_recompression.cpp
        src/recompression/experimental/parallel_ls_gain_recompression.cpp
        src/recompression/parallel_gr_recompression.cpp
        src/recompression/experimental/parallel_gr_alternate_recompression.cpp
        src/recompression/hash_recompression.cpp
        src/recompression/defs.cpp
        src/recompression/coders/rlslp_rule_sorter.cpp
        src/recompression/coders/coder.cpp
        src/recompression/coders/plain_rlslp_coder.cpp
        src/recompression/coders/plain_fixed_rlslp_coder.cpp
        src/recompression/coders/sorted_rlslp_dr_coder.cpp
        src/recompression/coders/sorted_rlslp_coder.cpp
        src/recompression/io/bitostream.cpp
        src/recompression/io/bitistream.cpp
        src/recompression.cpp
        src/recompression/graph.cpp
)
set(HEADERS
        include/recompression/parallel_lp_recompression.hpp
        include/recompression/experimental/parallel_lock_recompression.hpp
        include/recompression/defs.hpp
        include/recompression/lce_query.hpp
        include/recompression/rlslp.hpp
        include/recompression/recompression.hpp
        include/recompression/parallel_recompression.hpp
        include/recompression/radix_sort.hpp
        include/recompression/experimental/parallel_order_less_recompression.hpp
        include/recompression/experimental/parallel_gr2_recompression.hpp
        include/recompression/experimental/parallel_parhip_recompression.hpp
        include/recompression/parallel_rnd_recompression.hpp
        include/recompression/parallel_rnddir_recompression.hpp
        include/recompression/parallel_ls_recompression.hpp
        include/recompression/experimental/parallel_ls3_recompression.hpp
        include/recompression/experimental/parallel_ls5_recompression.hpp
        include/recompression/experimental/parallel_ls_gain_recompression.hpp
        include/recompression/parallel_gr_recompression.hpp
        include/recompression/experimental/parallel_gr_alternate_recompression.hpp
        include/recompression/fast_recompression.hpp
        include/recompression/hash_recompression.hpp
        include/recompression/util.hpp
        include/recompression/experimental/parallel_order_great_recompression.hpp
        include/recompression/coders/plain_rlslp_coder.hpp
        include/recompression/coders/plain_fixed_rlslp_coder.hpp
        include/recompression/coders/sorted_rlslp_dr_coder.hpp
        include/recompression/coders/sorted_rlslp_coder.hpp
        include/recompression/coders/rlslp_rule_sorter.hpp
        include/recompression/coders/coder.hpp
        include/recompression/io/bitistream.hpp
        include/recompression/io/bitostream.hpp
        include/recompression.hpp
        include/recompression/graph.hpp
)
