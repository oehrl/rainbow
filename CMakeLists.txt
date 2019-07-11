cmake_minimum_required(VERSION 3.10)
project(rainbow)

# cmake_dependent_option() does not seem to work properly
option(RAINBOW_BACKEND_CPU "Enable the CPU rendering backend" ON)
option(RAINBOW_BACKEND_VULKAN "Enable the Vulkan rendering backend" ON)
if (APPLE)
  option(RAINBOW_BACKEND_METAL "Enable the Metal rendering backend" ON)
else ()
  option(RAINBOW_BACKEND_OPENGL "Enable the OpenGL rendering backend (required OpenGL 4.3)" ON)
endif ()

find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

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
  src/thread_pool.cpp
  src/integral_range.cpp
  src/parallel.cpp
  src/vector.cpp
  src/data_structures.cpp
  src/constants.cpp
  src/random.cpp
  src/sampling.cpp
  src/photon_map.cpp
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
  PRIVATE assimp glm SDL2 Threads::Threads
)

if (APPLE)
  # The packages provided by homebrew do not define a target. So, we have
  # to set the include directories and link directories manually.
  target_include_directories(rainbow
    PRIVATE ${SDL2_INCLUDE_DIRS}/.. ${ASSIMP_INCLUDE_DIRS}
  )

  target_link_directories(rainbow
    PRIVATE ${SDL2_LIBDIR} ${ASSIMP_LIBRARY_DIRS}
  )
endif (APPLE)

if (RAINBOW_BACKEND_CPU)
  target_sources(rainbow
    PRIVATE
      src/backends/cpu/cpu_backend.cpp
  )
  target_compile_definitions(rainbow
    PRIVATE RAINBOW_BACKEND_CPU=1
  )
endif (RAINBOW_BACKEND_CPU)

if (RAINBOW_BACKEND_VULKAN)
  target_sources(rainbow
    PRIVATE
      src/backends/vulkan/vulkan_backend.cpp
      src/backends/vulkan/error.cpp
  )
  target_compile_definitions(rainbow
    PRIVATE RAINBOW_BACKEND_VULKAN=1
  )
  
  find_package(Vulkan REQUIRED)
  target_link_libraries(rainbow PRIVATE Vulkan)
endif (RAINBOW_BACKEND_VULKAN)

if (RAINBOW_BACKEND_OPENGL)
  target_sources(rainbow
    PRIVATE
      src/backends/opengl/opengl.cpp
      src/backends/opengl/opengl_backend.cpp
      src/backends/opengl/program.cpp
      src/backends/opengl/texture.cpp
      src/backends/opengl/buffer.cpp
  )
  target_compile_definitions(rainbow
    PRIVATE RAINBOW_BACKEND_OPENGL=1
  )
  
  find_package(OpenGL REQUIRED)
  target_link_libraries(rainbow
    PRIVATE OpenGL::GL
  )

  include(cmake/shaders.cmake)
  add_shaders(
    src/backends/opengl/view_rays.comp
  )
  add_dependencies(rainbow shaders)
endif (RAINBOW_BACKEND_OPENGL)

if (RAINBOW_BACKEND_METAL)
  target_sources(rainbow
    PRIVATE
      include/rainbow/backends/metal/metal_backend.hpp
      src/backends/metal/metal_backend.hh
      src/backends/metal/metal_backend.mm
      src/backends/metal/library.metal
  )
  target_compile_definitions(rainbow
    PRIVATE RAINBOW_BACKEND_METAL=1
  )
  target_link_libraries(rainbow
    PRIVATE "-framework Foundation" "-framework Metal"
  )
  
  include(cmake/shaders.cmake)
  add_shaders(
    src/backends/metal/library.metal
  )
  add_dependencies(rainbow shaders)
endif (RAINBOW_BACKEND_METAL)
