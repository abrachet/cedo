
file(GLOB yaml_inputs "*.yaml")

foreach(file ${yaml_inputs})
    string(REPLACE ".yaml" ".o" output ${file})
    get_filename_component(output ${output} NAME)
    execute_process(COMMAND yaml2obj ${file} -o ${CMAKE_CURRENT_BINARY_DIR}/${output})
endforeach()
