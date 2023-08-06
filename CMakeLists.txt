#
# Copyright (c) 2018 Aaron Delasy
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.14)
project(the VERSION 0.13.1)

option(BUILD_COVERAGE "Build coverage" OFF)
option(BUILD_TESTS "Build test programs" OFF)
option(TEST_CODEGEN_COMPILE "Whether codegen tests binary should be compiled" ON)
option(TEST_CODEGEN_MEMCHECK "Whether codegen tests binary should be memory checked" OFF)
set(TEST_CODEGEN_PLATFORM "default" CACHE STRING "Codegen tests binary platform")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Build type - Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Specify the build type for single-configuration generators" FORCE)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (BUILD_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-instr-generate -fcoverage-mapping")
endif ()

set(
  sources

  src/codegen/block.cpp
  src/codegen/expr-access.cpp
  src/codegen/expr-array.cpp
  src/codegen/expr-as.cpp
  src/codegen/expr-assign.cpp
  src/codegen/expr-await.cpp
  src/codegen/expr-binary.cpp
  src/codegen/expr-call.cpp
  src/codegen/expr-closure.cpp
  src/codegen/expr-cond.cpp
  src/codegen/expr-is.cpp
  src/codegen/expr-lit.cpp
  src/codegen/expr-map.cpp
  src/codegen/expr-obj.cpp
  src/codegen/expr-ref.cpp
  src/codegen/expr-unary.cpp
  src/codegen/fn-decl.cpp
  src/codegen/helpers.cpp
  src/codegen/node-break.cpp
  src/codegen/node-continue.cpp
  src/codegen/node-enum-decl.cpp
  src/codegen/node-expr.cpp
  src/codegen/node-fn-decl.cpp
  src/codegen/node-if.cpp
  src/codegen/node-loop.cpp
  src/codegen/node-main.cpp
  src/codegen/node-obj-decl.cpp
  src/codegen/node-return.cpp
  src/codegen/node-throw.cpp
  src/codegen/node-try.cpp
  src/codegen/node-var-decl.cpp
  src/codegen/type.cpp
  src/codegen/type-any.cpp
  src/codegen/type-array.cpp
  src/codegen/type-fn.cpp
  src/codegen/type-map.cpp
  src/codegen/type-obj.cpp
  src/codegen/type-opt.cpp
  src/codegen/type-union.cpp

  src/codegen-ast/CodegenAST.hpp
  src/codegen-ast/CodegenASTExpr.cpp
  src/codegen-ast/CodegenASTStmt.cpp
  src/codegen-ast/CodegenASTType.cpp

  src/AST.cpp src/AST.hpp
  src/ASTChecker.hpp
  src/ASTExpr.cpp
  src/ASTNode.cpp
  src/Codegen.cpp src/Codegen.hpp
  src/CodegenAPIItem.hpp
  src/CodegenCleanUp.cpp src/CodegenCleanUp.hpp
  src/CParser.cpp src/CParser.hpp
  src/Error.cpp src/Error.hpp
  src/Lexer.cpp src/Lexer.hpp
  src/Parser.cpp src/Parser.hpp
  src/ParserComment.cpp src/ParserComment.hpp
  src/ParserExpr.cpp
  src/ParserStmt.cpp
  src/ParserType.cpp
  src/Reader.cpp src/Reader.hpp
  src/Token.cpp src/Token.hpp
  src/Type.cpp src/Type.hpp
  src/TypeMap.cpp src/TypeMap.hpp
  src/Var.cpp src/Var.hpp
  src/VarMap.cpp src/VarMap.hpp
  src/VarStack.cpp src/VarStack.hpp
  src/codegen-api.hpp
  src/codegen-metadata.hpp
  src/config.hpp
  src/utils.hpp
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

  FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.13.0)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  set(
    tests

    test/ASTTest.cpp
    test/ASTCheckerTest.cpp
    test/ASTExprTest.cpp
    test/CodegenTest.cpp
    test/CodegenASTTest.cpp
    test/CodegenCleanUpTest.cpp
    test/CParserTest.cpp
    test/ErrorTest.cpp
    test/LexerTest.cpp
    test/MockAST.cpp test/MockAST.hpp
    test/MockLexer.cpp test/MockLexer.hpp
    test/MockParser.cpp test/MockParser.hpp
    test/MockReader.cpp test/MockReader.hpp
    test/ParserTest.cpp
    test/ParserCommentTest.cpp
    test/ParserDocTest.cpp
    test/ParserExprTest.cpp
    test/ParserTypeTest.cpp
    test/ReaderTest.cpp
    test/TokenTest.cpp
    test/TypeTest.cpp
    test/TypeMatchTest.cpp
    test/TypeMapTest.cpp
    test/UtilsTest.cpp
    test/VarMapTest.cpp
    test/VarStackTest.cpp
    test/utils.cpp test/utils.hpp
  )

  add_executable(${PROJECT_NAME}-tests ${tests})
  target_link_libraries(${PROJECT_NAME}-tests gmock_main ${PROJECT_NAME}-lib)

  gtest_discover_tests(
    ${PROJECT_NAME}-tests
    DISCOVERY_TIMEOUT 60
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    PROPERTIES
    ENVIRONMENT "TEST_CODEGEN_COMPILE=${TEST_CODEGEN_COMPILE}"
    ENVIRONMENT "TEST_CODEGEN_MEMCHECK=${TEST_CODEGEN_MEMCHECK}"
    ENVIRONMENT "TEST_CODEGEN_PLATFORM=${TEST_CODEGEN_PLATFORM}"
  )
endif ()

if (EXISTS "${PROJECT_SOURCE_DIR}/lab")
  add_executable(lab lab/lab.c)
  target_include_directories(lab PRIVATE "$ENV{DEPS_DIR}/include")
  target_link_directories(lab PRIVATE "$ENV{DEPS_DIR}/lib")
  target_link_libraries(lab PRIVATE crypto ssl)
endif ()
