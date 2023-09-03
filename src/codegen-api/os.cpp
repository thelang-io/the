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
  R"(_{struct str} os_arch (_{err_state_t} *fn_err_state, int line, int col) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{SYSTEM_INFO} info;)" EOL
  R"(    _{GetSystemInfo}(&info);)" EOL
  R"(    switch (info.wProcessorArchitecture) {)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_ALPHA}:)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_ALPHA64}: return _{str_alloc}("alpha");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_AMD64}: return _{str_alloc}("x86_64");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_ARM}: return _{str_alloc}("arm");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_ARM64}: return _{str_alloc}("arm64");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_IA32_ON_WIN64}: return _{str_alloc}("i686");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_IA64}: return _{str_alloc}("ia64");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_INTEL}:)" EOL
  R"(        switch (info.wProcessorLevel) ))" EOL
  R"(          case 4: return _{str_alloc}("i486");)" EOL
  R"(          case 5: return _{str_alloc}("i586");)" EOL
  R"(          default: return _{str_alloc}("i386");)" EOL
  R"(        ))" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_MIPS}: return _{str_alloc}("mips");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_PPC}: return _{str_alloc}("powerpc");)" EOL
  R"(      case _{PROCESSOR_ARCHITECTURE_SHX}: return _{str_alloc}("sh");)" EOL
  R"(    })" EOL
  R"(    return _{str_alloc}("unknown");)" EOL
  R"(  #else)" EOL
  R"(    _{struct utsname} buf;)" EOL
  R"(    if (_{uname}(&buf) < 0) {)" EOL
  R"(      _{error_assign}(fn_err_state, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("failed to retrieve uname information"), (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL
  R"(      _{longjmp}(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
  R"(    })" EOL
  R"(    return _{str_alloc}(buf.machine);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL,

  R"(_{struct str} os_name (_{err_state_t} *fn_err_state, int line, int col) {)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    return _{str_alloc}("Windows");)" EOL
  R"(  #else)" EOL
  R"(    _{struct utsname} buf;)" EOL
  R"(    if (_{uname}(&buf) < 0) {)" EOL
  R"(      _{error_assign}(fn_err_state, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("failed to retrieve uname information"), (_{struct str}) {_{NULL}, 0}), (void (*) (void *)) &_{error_Error_free}, line, col);)" EOL
  R"(      _{longjmp}(fn_err_state->buf_last->buf, fn_err_state->id);)" EOL
  R"(    })" EOL
  R"(    if (_{strcmp}(buf.sysname, "Darwin") == 0) return _{str_alloc}("macOS");)" EOL
  R"(    return _{str_alloc}(buf.sysname);)" EOL
  R"(  #endif)" EOL
  R"(})" EOL
};
