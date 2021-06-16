cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)
project(shoka LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

include(GoogleTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(test)
