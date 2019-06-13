cmake_minimum_required(VERSION 3.10)

# cmake_dependent_option() does not seem to work properly
option(RAINBOW_BACKEND_CPU "Enable the CPU rendering backend" ON)
if (APPLE)
  option(RAINBOW_BACKEND_OPENGL "Enable the OpenGL rendering backend (required OpenGL 4.3)" OFF)
else ()
  option(RAINBOW_BACKEND_OPENGL "Enable the OpenGL rendering backend (required OpenGL 4.3)" ON)
endif ()

set(GLM_TEST_ENABLE OFF)
add_subdirectory(extern/glm)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(extern/assimp)

find_package(SDL2 REQUIRED)

add_executable(rainbow
  src/main.cpp
  src/scene.cpp
  src/intersection.cpp
  src/camera.cpp
  src/application.cpp
  src/timing.cpp
  src/octree.cpp
  src/rendering_backend.cpp
  src/viewport.cpp
)

set_target_properties(rainbow
  PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(rainbow
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(rainbow
  PRIVATE assimp glm SDL2
)

if (APPLE)
  # The SDL2 package provided by homebrew does not define a target. So, we have
  # to set the include directories and link directories manually.
  target_include_directories(rainbow
    PRIVATE ${SDL2_INCLUDE_DIRS}/..
  )

  target_link_directories(rainbow
    PRIVATE ${SDL2_LIBDIR}
  )
endif (APPLE)

if (RAINBOW_BACKEND_CPU)
  target_sources(rainbow
    PRIVATE
      src/backends/cpu/cpu_backend.cpp
  )
endif (RAINBOW_BACKEND_CPU)


if (RAINBOW_BACKEND_OPENGL)
  target_sources(rainbow
    PRIVATE
      src/backends/opengl/opengl.cpp
      src/backends/opengl/opengl_backend.cpp
      src/backends/opengl/program.cpp
      src/backends/opengl/texture.cpp
      src/backends/opengl/buffer.cpp
  )
  
  find_package(OpenGL REQUIRED)
  target_link_libraries(rainbow
    PRIVATE OpenGL::GL
  )

  include(cmake/shaders.cmake)
  add_shaders(
    shaders/test.comp
  )
  add_dependencies(rainbow shaders)
endif (RAINBOW_BACKEND_OPENGL)
