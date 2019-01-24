
if (RECOMPRESSION_ENABLE_TESTS)
    build_test("bcomp")
    build_test("pcomp")
    build_test("adj_list")
    build_test("partition")
    build_test("recomp")
    build_test("fast_recomp")
    build_test("radix_sort")
    build_test("derive_text")
    build_test("extract")
    build_test("replace_letters")
endif (RECOMPRESSION_ENABLE_TESTS)

