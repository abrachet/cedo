function(add_cedo_system_test test_file cedo_file symbol)
    string(REGEX REPLACE "\.c(|pp)$" ".s" cedo_out ${cedo_file})
    set(cedo_out ${CMAKE_CURRENT_BINARY_DIR}/${cedo_out})

    string(REGEX REPLACE "\.c(|pp)$" ".o" cedo_input ${cedo_file})
    set(cedo_input ${CMAKE_CURRENT_BINARY_DIR}/${cedo_input})

    set(compiler ${CMAKE_CXX_COMPILER})
    if (${cedo_file} MATCHES "\.c$")
        set(compiler ${CMAKE_C_COMPILER})
    endif()
    add_custom_command(
        OUTPUT ${cedo_input}
        COMMAND ${compiler} ${CMAKE_CURRENT_SOURCE_DIR}/${cedo_file} -g -shared -fPIC -o ${cedo_input}
    )

    add_custom_command(
        OUTPUT ${cedo_out}
        DEPENDS cedo ${cedo_input}
        COMMAND ${CMAKE_BINARY_DIR}/bin/cedo -S -s ${symbol} -o ${cedo_out} ${cedo_input}
    )

    string(REGEX REPLACE "\.c(|pp)$" "" exec_name ${test_file})
    add_executable(${exec_name}
        ${cedo_out}
        ${test_file}
    )
    add_test(NAME system.${exec_name} COMMAND ${exec_name})
endfunction()

add_subdirectory(array)
add_subdirectory(compound)
add_subdirectory(typedef)

add_cedo_system_test(basic_test.c basic_test.cedo.c a)
add_cedo_system_test(main_executed_test.c main_executed_test.cedo.c a)
