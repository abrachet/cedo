add_executable(backend_test
    AsmStreamerTest.cpp
    EmitAsmTest.cpp
)

target_link_libraries(backend_test
    gtest
    gtest_main
    Backend
)

add_test(NAME backend_test COMMAND backend_test)
