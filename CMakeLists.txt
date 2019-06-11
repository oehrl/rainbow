cmake_minimum_required(VERSION 3.10)

set(GLM_TEST_ENABLE OFF CACHE BOOL "Don't build glm test suite")
add_subdirectory(extern/glm)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Don't build assimp tools")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Don't build assimp test suite")
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
  PRIVATE assimp glm SDL2 OpenGL
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
