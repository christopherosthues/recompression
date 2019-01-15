
if (RECOMPRESSION_ENABLE_BENCHMARKS)
    build_bench("parallel_bcomp")
    build_bench("parallel_pcomp")
    build_bench("parallel_recomp")

    build_bench("bcomp")
    build_bench("pcomp")
    build_bench("recomp")
    build_bench("radix_sort")
endif (RECOMPRESSION_ENABLE_BENCHMARKS)

