cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

option(TESTS "Generate build target for unit testing." ON)

include_directories(include)

if (TESTS)
    add_subdirectory(test)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(lib)
