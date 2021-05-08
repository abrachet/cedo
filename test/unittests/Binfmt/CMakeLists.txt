add_executable(binfmt_test
    DebugTest.cpp
    ELFFindSectionTest.cpp
    ELFResolveRelocTest.cpp
    FindFileTriple.cpp
)

target_link_libraries(binfmt_test
    gtest
    gtest_main
    Binfmt
)

# There are headers in lib/Binfmt that aren't exposed in include/
include_directories(${PROJECT_SOURCE_DIR})

add_test(NAME binfmt_test COMMAND binfmt_test)

add_subdirectory(Inputs)
