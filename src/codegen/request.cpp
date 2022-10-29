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

#include "request.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenRequest = {
  R"(void request_close (struct _{request_Request} *r) {)" EOL
  R"(  _{struct request} *req = (void *) &r[sizeof(struct _{request_Request})];)" EOL
  R"(  if (req->ssl != _{NULL}) {)" EOL
  R"(    _{SSL_CTX_free}(req->ctx);)" EOL
  R"(    _{SSL_free}(req->ssl);)" EOL
  R"(  })" EOL
  R"(  _{close}(req->fd);)" EOL
  R"(})" EOL,

  R"(struct _{request_Request} *request_open (_{struct str} method, _{struct str} u, _{struct buffer} data, struct _{array_request_Header} headers) {)" EOL
  R"(  void *r = _{alloc}(sizeof(struct _{request_Request}) + sizeof(_{struct request}));)" EOL
  R"(  _{struct request} *req = (void *) &r[sizeof(struct _{request_Request})];)" EOL
  R"(  return (struct _{request_Request} *) r;)" EOL
  R"(})" EOL,

  R"(struct _{request_Response} *request_read (struct _{request_Request} *req) {)" EOL
  // todo sort headers after parsing
  R"(  return _{request_Response_alloc}((_{struct buffer}) {_{NULL}, 0}, 0, (struct _{array_request_Header}) {_{NULL}, 0});)" EOL
  R"(})" EOL
};
