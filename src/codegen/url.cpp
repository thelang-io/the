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

#include "url.hpp"
#include "../config.hpp"

const std::vector<std::string> codegenURL = {
  R"(struct _{url_URL} *url_parse (_{struct str} s) {)" EOL
  R"(  if (s.l == 0) {)" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("invalid URL"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  })" EOL
  R"(  _{size_t} i = 0;)" EOL
  R"(  for (;; i++) {)" EOL
  R"(    char ch = s.d[i];)" EOL
  R"(    if (ch == ':' && i != 0) {)" EOL
  R"(      i++;)" EOL
  R"(      break;)" EOL
  R"(    } else if (!_{isalnum}(ch) && ch != '.' && ch != '-' && ch != '+') {)" EOL
  R"(      _{str_free}(s);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("invalid URL protocol"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(    } else if (i == s.l - 1) {)" EOL
  R"(      _{str_free}(s);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("invalid URL"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{struct str} protocol;)" EOL
  R"(  protocol.l = i;)" EOL
  R"(  protocol.d = _{alloc}(protocol.l);)" EOL
  R"(  _{memcpy}(protocol.d, s.d, protocol.l);)" EOL
  R"(  while (i < s.l && s.d[i] == '/') i++;)" EOL
  R"(  if (i == s.l) {)" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    return _{url_URL_alloc}(_{str_alloc}(""), protocol, _{str_alloc}(""), _{str_alloc}(""), _{str_alloc}(""), _{str_alloc}(""), _{str_alloc}(""), _{str_alloc}(""), _{str_alloc}(""));)" EOL
  R"(  })" EOL
  R"(  _{size_t} protocol_end = i;)" EOL
  R"(  if ((protocol_end - protocol.l) < 2) {)" EOL
  R"(    i = protocol.l;)" EOL
  R"(    protocol_end = i;)" EOL
  R"(  })" EOL
  R"(  _{size_t} hostname_start = protocol.l == protocol_end ? 0 : i;)" EOL
  R"(  _{size_t} port_start = 0;)" EOL
  R"(  _{size_t} pathname_start = protocol.l == protocol_end ? i : 0;)" EOL
  R"(  _{size_t} search_start = 0;)" EOL
  R"(  _{size_t} hash_start = 0;)" EOL
  R"(  for (;; i++) {)" EOL
  R"(    char ch = s.d[i];)" EOL
  R"(    if (ch == '@' && hostname_start != 0 && pathname_start == 0) {)" EOL
  R"(      _{str_free}(protocol);)" EOL
  R"(      _{str_free}(s);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("URL auth is not supported"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(    } else if (ch == ':' && port_start != 0 && (pathname_start == 0 || search_start == 0 || hash_start == 0)) {)" EOL
  R"(      _{str_free}(protocol);)" EOL
  R"(      _{str_free}(s);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("invalid URL port"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(    })" EOL
  R"(    if (ch == ':' && hostname_start != 0 && pathname_start == 0) port_start = i;)" EOL
  R"(    else if (ch == '/' && pathname_start == 0) pathname_start = i;)" EOL
  R"(    else if (ch == '?' && search_start == 0) search_start = i;)" EOL
  R"(    else if (ch == '#' && hash_start == 0) hash_start = i;)" EOL
  R"(    if (i == s.l - 1) break;)" EOL
  R"(  })" EOL
  R"(  _{struct str} hostname = _{str_alloc}("");)" EOL
  R"(  _{size_t} hostname_end = port_start != 0 ? port_start : pathname_start != 0 ? pathname_start : search_start != 0 ? search_start : hash_start != 0 ? hash_start : s.l;)" EOL
  R"(  if (hostname_start != 0 && hostname_start == hostname_end) {)" EOL
  R"(    _{str_free}(hostname);)" EOL
  R"(    _{str_free}(protocol);)" EOL
  R"(    _{str_free}(s);)" EOL
  R"(    _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("invalid URL hostname"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(    _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(  } else if (hostname_start != 0 && hostname_start != hostname_end) {)" EOL
  R"(    hostname.l = hostname_end - hostname_start;)" EOL
  R"(    hostname.d = _{re_alloc}(hostname.d, hostname.l);)" EOL
  R"(    _{memcpy}(hostname.d, &s.d[hostname_start], hostname.l);)" EOL
  R"(  })" EOL
  R"(  _{struct str} port = _{str_alloc}("");)" EOL
  R"(  _{size_t} port_end = pathname_start != 0 ? pathname_start : search_start != 0 ? search_start : hash_start != 0 ? hash_start : s.l;)" EOL
  R"(  if (port_start != 0 && port_start + 1 != port_end) {)" EOL
  R"(    port.l = port_end - port_start - 1;)" EOL
  R"(    port.d = _{re_alloc}(port.d, port.l);)" EOL
  R"(    _{memcpy}(port.d, &s.d[port_start + 1], port.l);)" EOL
  R"(  })" EOL
  R"(  _{struct str} host = _{str_alloc}("");)" EOL
  R"(  if (hostname.l != 0) {)" EOL
  R"(    host.l = hostname.l + (port.l == 0 ? 0 : port.l + 1);)" EOL
  R"(    host.d = _{re_alloc}(host.d, host.l);)" EOL
  R"(    _{memcpy}(host.d, hostname.d, hostname.l);)" EOL
  R"(    if (port.l != 0) {)" EOL
  R"(      _{memcpy}(&host.d[hostname.l], ":", 1);)" EOL
  R"(      _{memcpy}(&host.d[hostname.l + 1], port.d, port.l);)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{struct str} origin = _{str_alloc}("");)" EOL
  R"(  if (_{memcmp}(protocol.d, "ftp:", 4) == 0 || _{memcmp}(protocol.d, "http:", 5) == 0 || _{memcmp}(protocol.d, "https:", 6) == 0 || _{memcmp}(protocol.d, "ws:", 3) == 0 || _{memcmp}(protocol.d, "wss:", 4) == 0) {)" EOL
  R"(    if (host.l == 0) {)" EOL
  R"(      _{str_free}(origin);)" EOL
  R"(      _{str_free}(host);)" EOL
  R"(      _{str_free}(port);)" EOL
  R"(      _{str_free}(hostname);)" EOL
  R"(      _{str_free}(protocol);)" EOL
  R"(      _{str_free}(s);)" EOL
  R"(      _{error_assign}(&_{err_state}, _{TYPE_error_Error}, (void *) _{error_Error_alloc}(_{str_alloc}("URL origin is not present"), (_{struct str}) {_{NULL}, 0}));)" EOL
  R"(      _{longjmp}(_{err_state}.buf[_{err_state}.buf_idx - 1], _{err_state}.id);)" EOL
  R"(    })" EOL
  R"(    origin.l = protocol.l + 2 + host.l;)" EOL
  R"(    origin.d = _{re_alloc}(origin.d, origin.l);)" EOL
  R"(    _{memcpy}(origin.d, protocol.d, protocol.l);)" EOL
  R"(    _{memcpy}(&origin.d[protocol.l], "//", 2);)" EOL
  R"(    _{memcpy}(&origin.d[protocol.l + 2], host.d, host.l);)" EOL
  R"(  })" EOL
  R"(  _{struct str} pathname = _{str_alloc}("");)" EOL
  R"(  _{size_t} pathname_end = search_start != 0 ? search_start : hash_start != 0 ? hash_start : s.l;)" EOL
  R"(  if (pathname_start != 0 && pathname_start != pathname_end) {)" EOL
  R"(    pathname.l = pathname_end - pathname_start;)" EOL
  R"(    pathname.d = _{re_alloc}(pathname.d, pathname.l);)" EOL
  R"(    _{memcpy}(pathname.d, &s.d[pathname_start], pathname.l);)" EOL
  R"(  } else if (_{memcmp}(protocol.d, "ftp:", 4) == 0 || _{memcmp}(protocol.d, "http:", 5) == 0 || _{memcmp}(protocol.d, "https:", 6) == 0 || _{memcmp}(protocol.d, "ws:", 3) == 0 || _{memcmp}(protocol.d, "wss:", 4) == 0) {)" EOL
  R"(    pathname.l = 1;)" EOL
  R"(    pathname.d = _{re_alloc}(pathname.d, pathname.l);)" EOL
  R"(    _{memcpy}(pathname.d, "/", pathname.l);)" EOL
  R"(  })" EOL
  R"(  _{struct str} search = _{str_alloc}("");)" EOL
  R"(  _{size_t} search_end = hash_start != 0 ? hash_start : s.l;)" EOL
  R"(  if (search_start != 0 && search_start != search_end) {)" EOL
  R"(    search.l = search_end - search_start;)" EOL
  R"(    search.d = _{re_alloc}(search.d, search.l);)" EOL
  R"(    _{memcpy}(search.d, &s.d[search_start], search.l);)" EOL
  R"(  })" EOL
  R"(  _{struct str} path = _{str_alloc}("");)" EOL
  R"(  if (pathname.l != 0 || search.l != 0) {)" EOL
  R"(    path.l = pathname.l + search.l;)" EOL
  R"(    path.d = _{re_alloc}(path.d, path.l);)" EOL
  R"(    if (pathname.l != 0) {)" EOL
  R"(      _{memcpy}(path.d, pathname.d, pathname.l);)" EOL
  R"(      if (search.l != 0) _{memcpy}(&path.d[pathname.l], search.d, search.l);)" EOL
  R"(    } else if (search.l != 0) {)" EOL
  R"(      _{memcpy}(path.d, search.d, search.l);)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  _{struct str} hash = _{str_alloc}("");)" EOL
  R"(  if (hash_start != 0) {)" EOL
  R"(    hash.l = s.l - hash_start;)" EOL
  R"(    hash.d = _{re_alloc}(hash.d, hash.l);)" EOL
  R"(    _{memcpy}(hash.d, &s.d[hash_start], hash.l);)" EOL
  R"(  })" EOL
  R"(  _{str_free}(s);)" EOL
  R"(  return _{url_URL_alloc}(origin, protocol, host, hostname, port, path, pathname, search, hash);)" EOL
  R"(})" EOL
};
