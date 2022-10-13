cmake_minimum_required(VERSION 3.14)
project(the VERSION 0.11.0 LANGUAGES CXX)

option(BUILD_COVERAGE "Build coverage" OFF)
option(BUILD_TESTS "Build test programs" OFF)
option(TEST_CODEGEN_COMPILE "Whether codegen tests binary should be compiled" ON)
option(TEST_CODEGEN_MEMCHECK "Whether codegen tests binary should be memory checked" OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Build type - Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Specify the build type for single-configuration generators" FORCE)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (BUILD_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif ()

set(
  sources

  src/codegen/fs.cpp src/codegen/fs.hpp
  src/codegen/process.cpp src/codegen/process.hpp
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

function (the_target_add_flags target)
  if (MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else ()
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
  endif ()
endfunction ()

add_library(${PROJECT_NAME}-lib STATIC ${sources})
the_target_add_flags(${PROJECT_NAME}-lib)

add_executable(${PROJECT_NAME} src/main.cpp)
the_target_add_flags(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-lib)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
  enable_testing()

  include(FetchContent)
  include(GoogleTest)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
  )

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
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

  add_executable(${PROJECT_NAME}-tests ${tests})
  target_link_libraries(${PROJECT_NAME}-tests gmock_main ${PROJECT_NAME}-lib)

  gtest_discover_tests(
    ${PROJECT_NAME}-tests
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    PROPERTIES
    ENVIRONMENT "TEST_CODEGEN_COMPILE=${TEST_CODEGEN_COMPILE}"
    ENVIRONMENT "TEST_CODEGEN_MEMCHECK=${TEST_CODEGEN_MEMCHECK}"
    TIMEOUT 300
  )
endif ()
