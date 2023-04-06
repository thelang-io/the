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
  R"(_{noreturn} void error_throw (_{err_state_t} *state, int id, void *ctx) {)" EOL
  R"(  state->id = id;)" EOL
  R"(  state->ctx = ctx;)" EOL
  R"(})" EOL,

  R"(void error_stack_pos (_{err_state_t} *state, _{size_t} line, _{size_t} col) {)" EOL
  R"(  state->stack[state->stack_idx - 1].line = line;)" EOL
  R"(  state->stack[state->stack_idx - 1].col = col;)" EOL
  R"(})" EOL,

  R"(struct _{error_Error} *new_error (_{struct str} n1) {)" EOL
  R"(  return _{error_Error_alloc}(n1);)" EOL
  R"(})" EOL
};
