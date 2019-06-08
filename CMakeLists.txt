cmake_minimum_required(VERSION 3.10)

set(GLM_TEST_ENABLE OFF CACHE BOOL "Don't build glm test suite")
add_subdirectory(extern/glm)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Don't build assimp tools")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Don't build assimp test suite")
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

set_target_properties(rainbow
  PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(rainbow
  PRIVATE "include/"
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
