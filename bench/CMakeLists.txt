if (RECOMPRESSION_ENABLE_BENCHMARKS)
#    build_bench("recompression")
    build_bench("recompression_all")
    build_bench("scale_recompression")
    build_bench("strong_scale_recompression")
    build_bench("recompression_mem")
    build_bench("store_rlslp")

#    build_bench("radix_sort")

    build_bench("lce_query")
    build_bench("long_lce_query")
    build_bench("random_access")

#    build_bench("statistics")
    build_bench("compression_statistics")
#    build_bench("compression_mem")
    build_bench("remove_zeroes")
endif (RECOMPRESSION_ENABLE_BENCHMARKS)

