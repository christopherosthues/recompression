if (RECOMPRESSION_ENABLE_BENCHMARKS)
    build_bench("recompression")
#    build_bench("parallel_recomp")
#    build_bench("recomp")
#    build_bench("fast_recomp")

    build_bench("radix_sort")
endif (RECOMPRESSION_ENABLE_BENCHMARKS)

