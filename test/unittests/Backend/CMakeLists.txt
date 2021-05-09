add_executable(backend_test
    AsmStreamerTest.cpp
)

target_link_libraries(backend_test
    gtest
    gtest_main
)

add_test(NAME backend_test COMMAND backend_test)
