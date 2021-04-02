#
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

cmake_minimum_required(VERSION 3.14)
include(FetchContent)

FetchContent_Declare(libduc GIT_REPOSITORY https://github.com/delasy/libduc.git GIT_TAG v4.5.1)
FetchContent_MakeAvailable(libduc)

project(the-lang VERSION 0.0.1 LANGUAGES C)

option(BUILD_TESTS "Build test programs" OFF)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wconversion -Werror -Wextra -Wshadow -Wundef -Wunreachable-code -pedantic-errors")

set(
  sources

  src/ast.c src/ast.h
  src/codegen.c src/codegen.h
  src/codegen-macos.c src/codegen-macos.h
  src/lexer.c src/lexer.h
  src/parser.c src/parser.h
)

add_executable(${PROJECT_NAME} ${sources} src/main.c)
target_include_directories(${PROJECT_NAME} PRIVATE "${libduc_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE duc)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
  set(CTEST_MEMORYCHECK_COMMAND valgrind)
  set(MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1 --leak-check=full --show-leak-kinds=all --tool=memcheck --track-origins=yes")

  include(CTest)
  set(tests ast codegen codegen-macos lexer parser)

  foreach (test ${tests})
    add_executable(${PROJECT_NAME}-test-${test} ${sources} tests/${test}-test.c)
    target_include_directories(${PROJECT_NAME}-test-${test} PRIVATE "${libduc_SOURCE_DIR}/include")
    target_link_libraries(${PROJECT_NAME}-test-${test} PRIVATE duc)
    add_test(${test} ${PROJECT_NAME}-test-${test})
  endforeach ()
endif ()
