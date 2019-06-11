cmake_minimum_required(VERSION 3.10)

set(GLM_TEST_ENABLE OFF)
add_subdirectory(extern/glm)
add_subdirectory(extern/assimp)

include(cmake/shaders.cmake)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_shaders(
  shaders/test.comp
  shaders/fullscreen_quad.frag
  shaders/fullscreen_quad.vert
)

add_executable(rainbow
  src/main.cpp
  src/scene.cpp
  src/intersection.cpp
  src/camera.cpp
  src/application.cpp
  src/timing.cpp
  src/octree.cpp

  src/backends/opengl/opengl.cpp
  src/backends/opengl/program.cpp
  src/backends/opengl/texture.cpp
  src/backends/opengl/buffer.cpp
)

add_dependencies(rainbow shaders)

target_include_directories(rainbow
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(rainbow
  PRIVATE assimp glm SDL2 OpenGL
)

set_target_properties(rainbow
  PROPERTIES
    CXX_STANDARD 17
)
