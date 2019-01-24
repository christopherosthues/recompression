
if (RECOMPRESSION_ENABLE_BENCHMARKS)
    #build_bench("parallel_bcomp")
    #build_bench("parallel_pcomp")
    build_bench("parallel_recomp")

    #build_bench("bcomp")
    #build_bench("pcomp")
    build_bench("recomp")

    #build_bench("fast_bcomp")
    #build_bench("fast_pcomp")
    build_bench("fast_recomp")

    build_bench("radix_sort")
endif (RECOMPRESSION_ENABLE_BENCHMARKS)

