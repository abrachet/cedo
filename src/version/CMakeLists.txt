

execute_process(
    COMMAND test -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cedo
    RESULT_VARIABLE found
)

if(${found} EQUAL 0)
    set(cedo_input ${CMAKE_CURRENT_BINARY_DIR}/ExportVersion.o)
    set(cedo_output ${CMAKE_CURRENT_BINARY_DIR}/version.s)

    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -g -shared -fPIC -o ${cedo_input} ${CMAKE_CURRENT_SOURCE_DIR}/ExportVersion.c
    )

    execute_process(
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cedo -S -s githash -s version -o ${cedo_output} ${cedo_input}
    )

    target_sources(cedo PRIVATE ${cedo_output})
endif()

target_sources(cedo PUBLIC Version.cpp)
