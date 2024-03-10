cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(shoka LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)
add_subdirectory(test)
