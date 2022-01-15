cmake_minimum_required(VERSION 3.14)

project(
  the-core
  VERSION 1.5.5
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

  AST.cpp AST.hpp
  Codegen.cpp Codegen.hpp
  Error.cpp Error.hpp
  Lexer.cpp Lexer.hpp
  Parser.cpp Parser.hpp
  Reader.cpp Reader.hpp
  TypeMap.cpp TypeMap.hpp
  VarMap.cpp VarMap.hpp
)

add_executable(${PROJECT_NAME} ${sources} main.cpp)
target_add_flags(${PROJECT_NAME})
