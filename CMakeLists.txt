cmake_minimum_required(VERSION 3.10)

set(GLM_TEST_ENABLE OFF)
add_subdirectory(extern/glm)
add_subdirectory(extern/assimp)

find_package(SDL2 REQUIRED)

add_executable(rainbow
  src/main.cpp
  src/scene.cpp
  src/intersection.cpp
  src/camera.cpp
  src/application.cpp
  src/timing.cpp
)

target_include_directories(rainbow
  PRIVATE "include/"
)

target_link_libraries(rainbow
  PRIVATE assimp glm SDL2
)

set_target_properties(rainbow
  PROPERTIES
    CXX_STANDARD 17
)
