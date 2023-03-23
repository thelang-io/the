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

#include "random.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenRandom = {
  R"(double random_randomFloat (unsigned char o1, double n1, unsigned char o2, double n2) {)" EOL
  R"(  double a = o1 == 0 ? 0 : n1;)" EOL
  R"(  double b = o2 == 0 ? 1 : n2;)" EOL
  R"(  unsigned char d[4];)" EOL
  R"(  if (_{RAND_bytes}(d, 4) != 1) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to generate random bytes" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{uint32_t} r = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];)" EOL
  R"(  return a + r / ((double) _{UINT32_MAX} / (b - a));)" EOL
  R"(})" EOL,

  R"(_{int32_t} random_randomInt (unsigned char o1, _{int32_t} n1, unsigned char o2, _{int32_t} n2) {)" EOL
  R"(  _{int64_t} a = o1 == 0 ? _{INT32_MIN} : n1;)" EOL
  R"(  _{int64_t} b = o2 == 0 ? _{INT32_MAX} : n2;)" EOL
  R"(  unsigned char d[4];)" EOL
  R"(  if (_{RAND_bytes}(d, 4) != 1) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to generate random bytes" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{uint32_t} r = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];)" EOL
  R"(  return (int32_t) (a + r / (_{UINT32_MAX} / (b - a + 1) + 1));)" EOL
  R"(})" EOL,

  R"(_{struct str} random_randomStr (unsigned char o1, _{struct str} n1, unsigned char o2, _{int32_t} n2) {)" EOL
  R"(  _{struct str} a = o1 == 0 ? _{str_alloc}("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") : n1;)" EOL
  R"(  _{int32_t} l = o2 == 0 ? 32 : n2;)" EOL
  R"(  unsigned char *d = _{alloc}(l);)" EOL
  R"(  if (_{RAND_bytes}(d, l) != 1) {)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to generate random bytes" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  for (_{size_t} i = 0; i < l; i++) d[i] = a.d[(_{size_t}) _{round}((double) d[i] / 0xFF * (double) (a.l - 1))];)" EOL
  R"(  _{struct str} r = _{str_calloc}((void *) d, l);)" EOL
  R"(  _{free}(d);)" EOL
  R"(  return r;)" EOL
  R"(})" EOL
};
