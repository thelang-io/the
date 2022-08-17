cmake_minimum_required(VERSION 3.14)
project(the-core VERSION 0.11.13 LANGUAGES CXX)

option(BUILD_TESTS "Build test programs" OFF)
option(CODEGEN_MEMCHECK "Memory check codegen binaries" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
  sources

  src/AST.cpp src/AST.hpp
  src/ASTChecker.hpp
  src/ASTExpr.cpp src/ASTExpr.hpp
  src/ASTNode.cpp src/ASTNode.hpp
  src/Codegen.cpp src/Codegen.hpp
  src/CodegenCleanUp.cpp src/CodegenCleanUp.hpp
  src/Error.cpp src/Error.hpp
  src/Lexer.cpp src/Lexer.hpp
  src/Parser.cpp src/Parser.hpp
  src/ParserExpr.cpp src/ParserExpr.hpp
  src/ParserStmt.cpp src/ParserStmt.hpp
  src/ParserType.cpp src/ParserType.hpp
  src/Reader.cpp src/Reader.hpp
  src/Token.cpp src/Token.hpp
  src/Type.cpp src/Type.hpp
  src/TypeMap.cpp src/TypeMap.hpp
  src/Var.cpp src/Var.hpp
  src/VarMap.cpp src/VarMap.hpp
  src/VarStack.cpp src/VarStack.hpp
  src/config.hpp
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
endfunction ()

add_executable(${PROJECT_NAME} ${sources} src/main.cpp)
target_add_flags(${PROJECT_NAME})

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
  enable_testing()

  include(CTest)
  include(FetchContent)
  include(GoogleTest)

  FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
  FetchContent_MakeAvailable(googletest)

  set(
    tests

    test/ASTTest.cpp
    test/ASTCheckerTest.cpp
    test/ASTExprTest.cpp
    test/CodegenTest.cpp
    test/CodegenCleanUpTest.cpp
    test/ErrorTest.cpp
    test/LexerTest.cpp
    test/MockAST.cpp test/MockAST.hpp
    test/MockLexer.cpp test/MockLexer.hpp
    test/MockParser.cpp test/MockParser.hpp
    test/MockReader.cpp test/MockReader.hpp
    test/ParserTest.cpp
    test/ReaderTest.cpp
    test/TokenTest.cpp
    test/TypeTest.cpp
    test/TypeMapTest.cpp
    test/VarMapTest.cpp
    test/VarStackTest.cpp
    test/utils.cpp test/utils.hpp
  )

  add_executable(${PROJECT_NAME}-test ${sources} ${tests})
  target_link_libraries(${PROJECT_NAME}-test gmock_main)

  if (CODEGEN_MEMCHECK)
    gtest_discover_tests(${PROJECT_NAME}-test WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" PROPERTIES ENVIRONMENT "CODEGEN_MEMCHECK=ON")
  else ()
    gtest_discover_tests(${PROJECT_NAME}-test WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
  endif ()
endif ()
