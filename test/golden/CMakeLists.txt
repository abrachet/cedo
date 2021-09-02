function(add_cedo_golden_test)
    cmake_parse_arguments(
        "GOLDEN"
        ""
        "CEDO_SRC;EXPECTED_OUTPUT"
        "SYMS"
        ${ARGN}
    )

    string(REGEX REPLACE "\.c(|pp)$" ".out.s" cedo_out ${GOLDEN_CEDO_SRC})
    set(cedo_out ${CMAKE_CURRENT_BINARY_DIR}/${cedo_out})

    string(REGEX REPLACE "\.c(|pp)$" ".o" cedo_input ${GOLDEN_CEDO_SRC})
    set(cedo_input ${CMAKE_CURRENT_BINARY_DIR}/${cedo_input})

    set(compiler ${CMAKE_CXX_COMPILER})
    if (${GOLDEN_CEDO_SRC} MATCHES "\.c$")
        set(compiler ${CMAKE_C_COMPILER})
    endif()
    execute_process(
        COMMAND ${compiler} ${CMAKE_CURRENT_SOURCE_DIR}/${GOLDEN_CEDO_SRC} -g -shared -fPIC -o ${cedo_input}
    )

    set(sym_list "")
    foreach(sym ${GOLDEN_SYMS})
        set(sym_list "${sym_list} -s ${sym}")
    endforeach()

    execute_process(
        COMMAND sh -c  "${CMAKE_BINARY_DIR}/bin/cedo -S ${sym_list} --no-version -o ${cedo_out} ${cedo_input}"
    )

    string(REGEX REPLACE "\.c(|pp)$" "" test_name ${GOLDEN_CEDO_SRC})
    add_test(NAME golden.${test_name} COMMAND ${CMAKE_COMMAND} -E compare_files ${cedo_out} ${CMAKE_CURRENT_SOURCE_DIR}/${GOLDEN_EXPECTED_OUTPUT})
endfunction()

add_subdirectory(compound)
add_subdirectory(pointer)

add_cedo_golden_test(CEDO_SRC basic.c EXPECTED_OUTPUT basic.s SYMS a b)
add_cedo_golden_test(CEDO_SRC nullptr.c EXPECTED_OUTPUT nullptr.s SYMS a)
