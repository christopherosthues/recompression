
if (RECOMPRESSION_ENABLE_TESTS)
    build_test("parallel_recompression")
    build_test("fast_recompression")
    build_test("full_parallel_recompression")
    build_test("recompression")
#    build_test("partition")
    build_test("radix_sort")
    build_test("derive_text")
    build_test("extract")
endif (RECOMPRESSION_ENABLE_TESTS)

