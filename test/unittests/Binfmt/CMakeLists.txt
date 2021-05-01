add_executable(binfmt_test
    DebugTest.cpp
    FindFileTriple.cpp
)

target_link_libraries(binfmt_test
    gtest
    gtest_main
    Binfmt
)

add_test(NAME binfmt_test COMMAND binfmt_test)

add_subdirectory(Inputs)
