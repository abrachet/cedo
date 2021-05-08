
file(GLOB yaml_inputs "*.yaml")

foreach(file ${yaml_inputs})
    string(REPLACE ".yaml" ".o" output ${file})
    get_filename_component(output ${output} NAME)
    execute_process(COMMAND yaml2obj ${file} -o ${CMAKE_CURRENT_BINARY_DIR}/${output})
endforeach()

# We don't want to compile normally with CMake because we don't want
# project wide flags etc here.
file(GLOB c_inputs "*.c")

foreach(file ${c_inputs})
    string(REPLACE ".c" ".o" output ${file})
    get_filename_component(output ${output} NAME)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -g ${file} -c -o ${CMAKE_CURRENT_BINARY_DIR}/${output})
endforeach()

file(GLOB asm_inputs "*.s")
foreach(file ${asm_inputs})
    string(REPLACE ".s" ".o" output ${file})
    get_filename_component(output ${output} NAME)
    execute_process(COMMAND ${CMAKE_C_COMPILER} ${file} -c -o ${CMAKE_CURRENT_BINARY_DIR}/${output})
endforeach()
