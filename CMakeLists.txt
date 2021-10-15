cmake_minimum_required(VERSION 3.14)
project(the-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
  sources

  src/Error.cpp src/Error.hpp
  src/Reader.cpp src/Reader.hpp
  src/ReaderLocation.cpp src/ReaderLocation.hpp
)

function (target_add_flags target)
  target_compile_options(${target} PRIVATE -Wall)
  target_compile_options(${target} PRIVATE -Wconversion)
  target_compile_options(${target} PRIVATE -Werror)
  target_compile_options(${target} PRIVATE -Wextra)
  target_compile_options(${target} PRIVATE -Wold-style-cast)
  target_compile_options(${target} PRIVATE -Wparentheses)
  target_compile_options(${target} PRIVATE -Wshadow)
  target_compile_options(${target} PRIVATE -Wundef)
  target_compile_options(${target} PRIVATE -Wunreachable-code)
  target_compile_options(${target} PRIVATE -pedantic-errors)
endfunction()

add_executable(${PROJECT_NAME} ${sources} src/main.cpp)
target_add_flags(${PROJECT_NAME})
