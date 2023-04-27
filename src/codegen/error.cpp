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

#include "error.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenError = {
  R"(void error_assign (_{err_state_t} *state, int id, void *ctx) {)" EOL
  R"(  state->id = id;)" EOL
  R"(  state->ctx = ctx;)" EOL
  R"(  *((_{struct str} *) &((struct _{error_Error} *) state->ctx)->__THE_0_stack) = _{error_stack_str}(state);)" EOL
  R"(})" EOL,

  R"(void error_stack_pop (_{err_state_t} *state) {)" EOL
  R"(  state->stack_idx--;)" EOL
  R"(})" EOL,

  R"(void error_stack_pos (_{err_state_t} *state, int line, int col) {)" EOL
  R"(  state->stack[state->stack_idx - 1].line = line;)" EOL
  R"(  state->stack[state->stack_idx - 1].col = col;)" EOL
  R"(})" EOL,

  R"(void error_stack_push (_{err_state_t} *state, char *file, char *name) {)" EOL
  R"(  state->stack[state->stack_idx++] = (_{err_stack_t}) {file, name, 0, 0};)" EOL
  R"(})" EOL,

  R"~(_{struct str} error_stack_str (_{err_state_t} *state) {)~" EOL
  R"~(  _{struct str} message = ((struct _{error_Error} *) state->ctx)->__THE_0_message;)~" EOL
  R"~(  _{size_t} l = message.l;)~" EOL
  R"~(  char *d = _{malloc}(l);)~" EOL
  R"~(  if (d == _{NULL}) {)~" EOL
  R"~(    _{fprintf}(_{stderr}, "Fatal Error: failed to allocate %zu bytes" _{THE_EOL}, l);)~" EOL
  R"~(    _{exit}(_{EXIT_FAILURE});)~" EOL
  R"~(  })~" EOL
  R"~(  _{memcpy}(d, message.d, l);)~" EOL
  R"~(  for (int i = state->stack_idx - 1; i >= 0; i--) {)~" EOL
  R"~(    _{err_stack_t} it = state->stack[i];)~" EOL
  R"~(    _{size_t} z;)~" EOL
  R"~(    char *fmt;)~" EOL
  R"~(    if (it.col == 0 && it.line == 0) {)~" EOL
  R"~(      fmt = _{THE_EOL} "  at %s (%s)";)~" EOL
  R"~(      z = _{snprintf}(_{NULL}, 0, fmt, it.name, it.file);)~" EOL
  R"~(    } else if (it.col == 0) {)~" EOL
  R"~(      fmt = _{THE_EOL} "  at %s (%s:%d)";)~" EOL
  R"~(      z = _{snprintf}(_{NULL}, 0, fmt, it.name, it.file, it.line);)~" EOL
  R"~(    } else {)~" EOL
  R"~(      fmt = _{THE_EOL} "  at %s (%s:%d:%d)";)~" EOL
  R"~(      z = _{snprintf}(_{NULL}, 0, fmt, it.name, it.file, it.line, it.col);)~" EOL
  R"~(    })~" EOL
  R"~(    d = _{realloc}(d, l + z);)~" EOL
  R"~(    if (d == _{NULL}) {)~" EOL
  R"~(      _{fprintf}(_{stderr}, "Fatal Error: failed to reallocate %zu bytes" _{THE_EOL}, l + z);)~" EOL
  R"~(      _{exit}(_{EXIT_FAILURE});)~" EOL
  R"~(    })~" EOL
  R"~(    if (it.col == 0 && it.line == 0) {)~" EOL
  R"~(      _{sprintf}(&d[l], fmt, it.name, it.file);)~" EOL
  R"~(    } else if (it.col == 0) {)~" EOL
  R"~(      _{sprintf}(&d[l], fmt, it.name, it.file, it.line);)~" EOL
  R"~(    } else {)~" EOL
  R"~(      _{sprintf}(&d[l], fmt, it.name, it.file, it.line, it.col);)~" EOL
  R"~(    })~" EOL
  R"~(    l += z;)~" EOL
  R"~(  })~" EOL
  R"~(  return (_{struct str}) {d, l};)~" EOL
  R"~(})~" EOL,

  R"(void error_unset (_{err_state_t} *state) {)" EOL
  R"(  state->id = -1;)" EOL
  R"(})" EOL,

  R"(struct _{error_Error} *new_error (_{struct str} n1) {)" EOL
  R"(  return _{error_Error_alloc}(n1, (_{struct str}) {_{NULL}, 0});)" EOL
  R"(})" EOL
};
