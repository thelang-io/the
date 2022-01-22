cmake_minimum_required(VERSION 3.14)

project(
  the-core
  VERSION 1.6.0
  LANGUAGES CXX
)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
FetchContent_MakeAvailable(googletest)

function(target_add_flags target)
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

set(
  sources

  src/Error.cpp src/Error.hpp
  src/Lexer.cpp src/Lexer.hpp
  src/Reader.cpp src/Reader.hpp
  src/Token.cpp src/Token.hpp
)

add_executable(${PROJECT_NAME} ${sources} src/main.cpp)
target_add_flags(${PROJECT_NAME})

set(
  tests

  test/ErrorTest.cpp
  test/LexerTest.cpp
  test/MockReader.cpp test/MockReader.hpp
  test/ReaderTest.cpp
  test/TokenTest.cpp
  test/utils.hpp
)

add_executable(${PROJECT_NAME}-test ${sources} ${tests})
target_link_libraries(${PROJECT_NAME}-test gmock_main)
