/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "date.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenDate = {
  R"(_{uint64_t} date_now () {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{FILETIME} ft;)" EOL
  R"(    _{GetSystemTimePreciseAsFileTime}(&ft);)" EOL
  R"(    return (((_{uint64_t}) ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 1e4 - 116444736e5;)" EOL
  R"(  #else)" EOL
  R"(    _{struct timespec} ts;)" EOL
  R"(    _{clock_gettime}(_{CLOCK_REALTIME}, &ts);)" EOL
  R"(    return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;)" EOL
  R"(  #endif)" EOL
  R"(})" EOL
};
