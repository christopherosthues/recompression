if (RECOMPRESSION_ENABLE_BENCHMARKS)
#    build_bench("recompression")
    build_bench("recompression_all")
    build_bench("scale_recompression")

#    build_bench("radix_sort")
    build_bench("lce_query")
endif (RECOMPRESSION_ENABLE_BENCHMARKS)

