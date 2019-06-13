function(ADD_SHADERS)

  set(SHADERS_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/rainbow/shaders.hpp)
  file(WRITE ${SHADERS_FILE} "#pragma once\n")

  foreach (SHADER ${ARGV})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    get_filename_component(SHADER_EXTENSION ${SHADER} EXT)
    string(SUBSTRING ${SHADER_EXTENSION} 1 -1 SHADER_EXTENSION)

    file(READ ${SHADER} SHADER_SOURCE)
    string(REPLACE "\n" "\\n" SHADER_SOURCE "${SHADER_SOURCE}")

    set(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/rainbow/shaders/${SHADER_NAME}.${SHADER_EXTENSION}.hpp)
    
    configure_file(
      cmake/shader.hpp.in
      ${OUTPUT}
    )
    set(OUTPUTS ${OUTPUTS} ${OUTPUT})
    file(APPEND ${SHADERS_FILE} "#include \"shaders/${SHADER_NAME}.${SHADER_EXTENSION}.hpp\"\n")
  endforeach(SHADER)

  add_custom_command(
    OUTPUT ${OUTPUTS}
    MAIN_DEPENDENCY cmake/shader.hpp.in
    DEPENDS ${ARGV}
    COMMAND ${CMAKE_COMMAND} -E echo "Generating shader header files"
  )

  add_custom_target(
    shaders
    DEPENDS ${OUTPUTS}
    SOURCES ${ARGV}
  )

endfunction(ADD_SHADERS)
