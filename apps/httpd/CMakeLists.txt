function(generate_swagger input output)
    set(cmd "${CMAKE_SOURCE_DIR}/json/json2code.py -f ${input} -o ${output}")
    message(STATUS ${cmd})
    execute_process(COMMAND sh -c ${cmd})
endfunction(generate_swagger)

generate_swagger(${CMAKE_CURRENT_SOURCE_DIR}/demo.json ${CMAKE_CURRENT_BINARY_DIR}/demo.json.hh)
