cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BadApple VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

add_library(glad
  vendor/glad/src/glad.c
)

target_include_directories(glad
  PUBLIC
  vendor/glad/include
)

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio
  INTERFACE
  vendor/miniaudio
)

add_executable(bad_apple
  src/main.cpp
  src/shader.cpp
  src/video_reader.cpp
)

target_include_directories(bad_apple
  PRIVATE
  include
)

target_link_libraries(bad_apple
  PRIVATE
  glfw
  glad
  miniaudio
)
