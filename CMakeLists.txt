#
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

cmake_minimum_required(VERSION 3.14)
project(the LANGUAGES CXX)

option(BUILD_TESTS "Build test programs" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall)
add_compile_options(-Wconversion)
add_compile_options(-Werror)
add_compile_options(-Wextra)
add_compile_options(-Wextra-semi)
add_compile_options(-Wold-style-cast)
add_compile_options(-Wparentheses)
add_compile_options(-Wshadow)
add_compile_options(-Wundef)
add_compile_options(-Wunreachable-code)
add_compile_options(-pedantic-errors)

set(
  sources

  src/Error.cpp src/Error.hpp
  src/Reader.cpp src/Reader.hpp
  src/version.hpp
)

add_executable(${PROJECT_NAME} ${sources} src/main.cpp)
