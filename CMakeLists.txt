cmake_minimum_required(VERSION 3.14)

project(cedo LANGUAGES CXX C ASM)

set(CMAKE_CXX_STANDARD 17)

option(TESTS "Generate build target for unit testing." ON)

# add_compile_options("-fno-exceptions" "-Wall")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

include_directories(include)

if (TESTS)
    add_subdirectory(test)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS_DEBUG  "-g")

add_subdirectory(src)
add_subdirectory(lib)

