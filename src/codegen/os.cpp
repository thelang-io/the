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

#include "os.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenOS = {
  R"(_{struct str} os_name () {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    return _{str_alloc}("Windows");)" EOL
  R"(  #else)" EOL
  R"(    _{struct utsname} buf;)" EOL
  R"(    if (_{uname}(&buf) < 0) {)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("failed to retrieve uname information"), _{str_alloc}("")));)" EOL
  R"(      _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(    })" EOL
  R"(    if (_{strcmp}(buf.sysname, "Darwin") == 0) return _{str_alloc}("macOS");)" EOL
  R"(    return _{str_alloc}(buf.sysname);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL
};
