#
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

cmake_minimum_required(VERSION 3.14)
project(the LANGUAGES CXX)

option(BUILD_TESTS "Build test programs" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
  sources

  src/Error.cpp src/Error.hpp
  src/Reader.cpp src/Reader.hpp
  src/version.hpp
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

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTS)
  include(CTest)
  include(FetchContent)
  include(GoogleTest)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
  )

  FetchContent_MakeAvailable(googletest)

  set(
    tests

    tests/ErrorTest.cpp
    tests/ReaderTest.cpp
  )

  add_library(${PROJECT_NAME}-lib SHARED ${sources})
  target_add_flags(${PROJECT_NAME}-lib)

  foreach (test ${tests})
    string(REGEX REPLACE "^tests/([A-Za-z_/-]+).cpp$" "\\1" test_name ${test})
    string(REPLACE "/" "-" test_name ${test_name})

    add_executable(${test_name} ${test})
    target_add_flags(${test_name})
    target_link_libraries(${test_name} PRIVATE gtest_main ${PROJECT_NAME}-lib)
    gtest_discover_tests(${test_name})
  endforeach()
endif()
