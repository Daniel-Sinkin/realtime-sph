cmake_minimum_required(VERSION 3.20)

if(APPLE AND NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED CMAKE_CXX_COMPILER)
  foreach(DS_TEMPLATE_LLVM_PREFIX IN ITEMS /opt/homebrew/opt/llvm /usr/local/opt/llvm)
    if(EXISTS "${DS_TEMPLATE_LLVM_PREFIX}/bin/clang" AND EXISTS "${DS_TEMPLATE_LLVM_PREFIX}/bin/clang++")
      set(CMAKE_C_COMPILER "${DS_TEMPLATE_LLVM_PREFIX}/bin/clang" CACHE FILEPATH "C compiler" FORCE)
      set(CMAKE_CXX_COMPILER "${DS_TEMPLATE_LLVM_PREFIX}/bin/clang++" CACHE FILEPATH "C++ compiler" FORCE)
      message(STATUS "Using LLVM toolchain from ${DS_TEMPLATE_LLVM_PREFIX}")
      break()
    endif()
  endforeach()
endif()

project(realtime_sph LANGUAGES C CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(GLSLC_EXECUTABLE NAMES glslc REQUIRED)

add_subdirectory(external/ds-vk)

set(REALTIME_SPH_SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(REALTIME_SPH_SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
set(REALTIME_SPH_SHADER_SOURCES
  shaders/realtime_sph_apply_delta.comp
  shaders/realtime_sph_build_grid.comp
  shaders/realtime_sph_build_neighbors.comp
  shaders/realtime_sph_delta.comp
  shaders/realtime_sph_lambda.comp
  shaders/realtime_sph_particles.frag
  shaders/realtime_sph_particles.vert
  shaders/realtime_sph_predict.comp
  shaders/realtime_sph_reset.comp
  shaders/realtime_sph_velocity.comp
)

set(REALTIME_SPH_SHADER_OUTPUTS)
foreach(REALTIME_SPH_SHADER IN LISTS REALTIME_SPH_SHADER_SOURCES)
  get_filename_component(REALTIME_SPH_SHADER_NAME "${REALTIME_SPH_SHADER}" NAME)
  set(REALTIME_SPH_SHADER_OUTPUT "${REALTIME_SPH_SHADER_OUTPUT_DIR}/${REALTIME_SPH_SHADER_NAME}.spv")
  add_custom_command(
    OUTPUT "${REALTIME_SPH_SHADER_OUTPUT}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${REALTIME_SPH_SHADER_OUTPUT_DIR}"
    COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${REALTIME_SPH_SHADER}" -o "${REALTIME_SPH_SHADER_OUTPUT}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${REALTIME_SPH_SHADER}"
    COMMENT "Compiling ${REALTIME_SPH_SHADER_NAME}"
    VERBATIM
  )
  list(APPEND REALTIME_SPH_SHADER_OUTPUTS "${REALTIME_SPH_SHADER_OUTPUT}")
endforeach()
add_custom_target(realtime_sph_shaders DEPENDS ${REALTIME_SPH_SHADER_OUTPUTS})

add_executable(realtime_sph
  app/main.cpp
  src/realtime_sph/realtime_sph_app.cpp
)
add_dependencies(realtime_sph realtime_sph_shaders)

target_include_directories(realtime_sph PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(realtime_sph PRIVATE
  DS_VK_SHADER_DIR="${REALTIME_SPH_SHADER_OUTPUT_DIR}"
)

target_link_libraries(realtime_sph PRIVATE
  ds_vk
  ds_vk_viz
  ds_vk_picker
  ds_vk_manipulator
  ds_vk_warnings
)
