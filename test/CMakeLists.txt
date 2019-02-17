if (RECOMPRESSION_ENABLE_TESTS)
    build_test("parallel_recompression")
#    build_test("full_parallel_recompression")
#    build_test("parallel_order_great_recompression")
#    build_test("parallel_order_less_recompression")

    build_test("fast_recompression")
#    build_test("sequential_recompression")
#    build_test("recompression_hash")
#
#    build_test("radix_sort")
#    build_test("derive_text")
#    build_test("extract")
#    build_test("lce_query")

#    build_test("bitstream")
endif (RECOMPRESSION_ENABLE_TESTS)
