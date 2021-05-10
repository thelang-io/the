#
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

cmake_minimum_required(VERSION 3.14)
include(FetchContent)

FetchContent_Declare(libduc GIT_REPOSITORY https://github.com/delasy/libduc.git GIT_TAG v4.7.1)
FetchContent_MakeAvailable(libduc)

file(
  STRINGS src/version.h VERSION_STRING
  LIMIT_COUNT 1
  REGEX "^#define VERSION "
)

if (VERSION_STRING MATCHES "\"([0-9]+)\\.([0-9]+)\\.([0-9]+)\"")
  set(VERSION_MAJOR ${CMAKE_MATCH_1})
  set(VERSION_MINOR ${CMAKE_MATCH_2})
  set(VERSION_PATCH ${CMAKE_MATCH_3})
elseif (VERSION_STRING MATCHES "\"([0-9]+)\\.([0-9]+)\"")
  set(VERSION_MAJOR ${CMAKE_MATCH_1})
  set(VERSION_MINOR ${CMAKE_MATCH_2})
  set(VERSION_PATCH "0")
else ()
  message(FATAL_ERROR "Couldn't parse version from src/version.h")
endif ()

if (VERSION_PATCH STREQUAL "0")
  project(the VERSION ${VERSION_MAJOR}.${VERSION_MINOR} LANGUAGES C)
else ()
  project(the VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} LANGUAGES C)
endif ()

option(BUILD_TESTS "Build test programs" OFF)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wconversion -Werror -Wextra -Wshadow -Wundef -Wunreachable-code -pedantic-errors")

set(
  sources

  src/lexer/comment-block.c src/lexer/comment-block.h
  src/lexer/comment-line.c src/lexer/comment-line.h
  src/lexer/lit-char.c src/lexer/lit-char.h
  src/lexer/lit-float.c src/lexer/lit-float.h
  src/lexer/lit-id.c src/lexer/lit-id.h
  src/lexer/lit-int-bin.c src/lexer/lit-int-bin.h
  src/lexer/lit-int-dec.c src/lexer/lit-int-dec.h
  src/lexer/lit-int-hex.c src/lexer/lit-int-hex.h
  src/lexer/lit-int-oct.c src/lexer/lit-int-oct.h
  src/lexer/lit-str.c src/lexer/lit-str.h
  src/lexer/op.c src/lexer/op.h
  src/lexer/ws.c src/lexer/ws.h
  src/ast.c src/ast.h
  src/codegen.c src/codegen.h
  src/codegen-macos.c src/codegen-macos.h
  src/lexer.c src/lexer.h
  src/parser.c src/parser.h
  src/version.h
)

add_executable(${PROJECT_NAME} ${sources} src/main.c)
target_include_directories(${PROJECT_NAME} PRIVATE "${libduc_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE duc_a)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
  set(CTEST_MEMORYCHECK_COMMAND valgrind)
  set(MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1 --leak-check=full --show-leak-kinds=all --tool=memcheck --track-origins=yes")

  include(CTest)
  add_library(${PROJECT_NAME}-lib_a STATIC ${sources})
  target_include_directories(${PROJECT_NAME}-lib_a PRIVATE "${libduc_SOURCE_DIR}/include")
  target_link_libraries(${PROJECT_NAME}-lib_a PRIVATE duc_a)

  set(
    tests

    tests/lexer/comment-block-test.c
    tests/lexer/comment-line-test.c
    tests/lexer/lit-char-test.c
    tests/lexer/lit-float-test.c
    tests/lexer/lit-id-test.c
    tests/lexer/lit-int-bin-test.c
    tests/lexer/lit-int-dec-test.c
    tests/lexer/lit-int-hex-test.c
    tests/lexer/lit-int-oct-test.c
    tests/lexer/lit-str-test.c
    tests/lexer/op-test.c
    tests/lexer/ws-test.c
    tests/ast-test.c
    tests/codegen-test.c
    tests/codegen-macos-test.c
    tests/lexer-test.c
    tests/parser-test.c
  )

  foreach (test_file ${tests})
    string(LENGTH ${test_file} test_len)
    math(EXPR test_len "${test_len} - 13")
    string(SUBSTRING ${test_file} 6 ${test_len} test)
    string(REPLACE "/" "-" test ${test})
    set(test_files ${test_file})
    set(test_name ${PROJECT_NAME}-test-${test})

    if (test_file MATCHES tests/lexer*)
      list(APPEND test_files tests/lexer-test.h)
    endif()

    add_executable(${test_name} ${test_files} tests/testing.h)
    target_include_directories(${test_name} PRIVATE "${libduc_SOURCE_DIR}/include")
    target_link_libraries(${test_name} PRIVATE duc ${PROJECT_NAME}-lib_a)
    add_test(${test} ${PROJECT_NAME}-test-${test})
  endforeach ()
endif ()
