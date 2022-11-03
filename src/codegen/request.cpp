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
  R"(char *request_stringifyHeaders (struct _{array_request_Header} headers, struct _{url_URL} *url, _{struct buffer} data) {)" EOL
  R"(  _{bool} has_content_length = _{false};)" EOL
  R"(  _{bool} has_host = _{false};)" EOL
  R"(  char *d = _{NULL};)" EOL
  R"(  _{size_t} l = 0;)" EOL
  R"(  for (_{size_t} i = 0; i < headers.l; i++) {)" EOL
  R"(    struct _{request_Header} *h = headers.d[i];)" EOL
  R"(    char *name = _{str_cstr}(h->__THE_0_name);)" EOL
  R"(    for (_{size_t} j = 0; j < h->__THE_0_name.l; j++) name[j] = (char) _{tolower}(name[j]);)" EOL
  R"(    if (_{strcmp}(name, "content-length") == 0) has_content_length = _{true};)" EOL
  R"(    else if (_{strcmp}(name, "host") == 0) has_host = _{true};)" EOL
  R"(    _{free}(name);)" EOL
  R"(    d = _{re_alloc}(d, l + h->__THE_0_name.l + 2 + h->__THE_0_value.l + 3);)" EOL
  R"(    _{memcpy}(&d[l], h->__THE_0_name.d, h->__THE_0_name.l);)" EOL
  R"(    _{memcpy}(&d[l + h->__THE_0_name.l], ": ", 2);)" EOL
  R"(    _{memcpy}(&d[l + h->__THE_0_name.l + 2], h->__THE_0_value.d, h->__THE_0_value.l);)" EOL
  R"(    _{memcpy}(&d[l + h->__THE_0_name.l + 2 + h->__THE_0_value.l], "\r\n", 3);)" EOL
  R"(    l += h->__THE_0_name.l + 2 + h->__THE_0_value.l + 2;)" EOL
  R"(  })" EOL
  R"(  if (!has_host) {)" EOL
  R"(    char *h = _{str_cstr}(url->__THE_0_hostname);)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, "Host: %s\r\n", h);)" EOL
  R"(    d = _{re_alloc}(d, l + z + 1);)" EOL
  R"(    _{sprintf}(&d[l], "Host: %s\r\n", h);)" EOL
  R"(    l += z;)" EOL
  R"(    _{free}(h);)" EOL
  R"(  })" EOL
  R"(  if (!has_content_length) {)" EOL
  R"(    _{size_t} z = _{snprintf}(_{NULL}, 0, "Content-Length: %zu\r\n", data.l);)" EOL
  R"(    d = _{re_alloc}(d, l + z + 1);)" EOL
  R"(    _{sprintf}(&d[l], "Content-Length: %zu\r\n", data.l);)" EOL
  R"(    l += z;)" EOL
  R"(  })" EOL
  R"(  return d;)" EOL
  R"(})" EOL,

  R"(void request_close (struct _{request_Request} **r) {)" EOL
  R"(  _{struct request} *req = (void *) *r;)" EOL
  R"(  if (req->ssl != _{NULL}) {)" EOL
  R"(    _{SSL_CTX_free}(req->ctx);)" EOL
  R"(    _{SSL_free}(req->ssl);)" EOL
  R"(  } else if (req->fd != 0) {)" EOL
  R"(    #ifdef _{THE_OS_WINDOWS})" EOL
  R"(      _{closesocket}(req->fd);)" EOL
  R"(    #else)" EOL
  R"(      _{close}(req->fd);)" EOL
  R"(    #endif)" EOL
  R"(  })" EOL
  R"(  req->fd = 0;)" EOL
  R"(  req->ctx = _{NULL};)" EOL
  R"(  req->ssl = _{NULL};)" EOL
  R"(})" EOL,

  R"(struct _{request_Request} *request_open (_{struct str} method, _{struct str} u, _{struct buffer} data, struct _{array_request_Header} headers) {)" EOL
  R"(  struct _{url_URL} *url = _{url_parse}(u);)" EOL
  R"(  if ()" EOL
  R"(    !(url->__THE_0_protocol.l == 5 && _{memcmp}(url->__THE_0_protocol.d, "http:", 5) == 0) &&)" EOL
  R"(    !(url->__THE_0_protocol.l == 6 && _{memcmp}(url->__THE_0_protocol.d, "https:", 6) == 0))" EOL
  R"(  ) {)" EOL
  // todo test
  R"(    char *protocol = _{str_cstr}(url->__THE_0_protocol);)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: can't perform request with protocol `%s`" _{THE_EOL}, protocol);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  } else if (url->__THE_0_port.l > 6) {)" EOL
  // todo test
  R"(    char *port = _{str_cstr}(url->__THE_0_port);)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: invalid port `%s`" _{THE_EOL}, port);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  char port[10];)" EOL
  R"(  _{memcpy}(port, url->__THE_0_protocol.l == 6 ? "443" : "80", url->__THE_0_protocol.l == 6 ? 3 : 2);)" EOL
  R"(  port[url->__THE_0_protocol.l == 6 ? 3 : 2] = '\0';)" EOL
  R"(  if (url->__THE_0_port.l != 0) {)" EOL
  R"(    _{memcpy}(port, url->__THE_0_port.d, url->__THE_0_port.l);)" EOL
  R"(    unsigned long p = _{strtoul}(port, _{NULL}, 10);)" EOL
  R"(    if (p > 65535) {)" EOL
  // todo test
  R"(      _{fprintf}(_{stderr}, "Error: invalid port `%s`" _{THE_EOL}, port);)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    if (!_{lib_ws2_init}) {)" EOL
  R"(      _{WSADATA} w;)" EOL
  R"(      if (_{WSAStartup}(_{MAKEWORD}(2, 2), &w) != 0) {)" EOL
  R"(        _{fprintf}(_{stderr}, "Error: failed to initialize use of Windows Sockets DLL" _{THE_EOL});)" EOL
  R"(        _{exit}(_{EXIT_FAILURE});)" EOL
  R"(      })" EOL
  R"(      _{lib_ws2_init} = _{true};)" EOL
  R"(    })" EOL
  R"(  #endif)" EOL
  R"(  char *hostname = _{str_cstr}(url->__THE_0_hostname);)" EOL
  R"(  _{struct addrinfo} *addr = _{NULL};)" EOL
  R"(  _{struct addrinfo} hints;)" EOL
  R"(  _{memset}(&hints, 0, sizeof(hints));)" EOL
  R"(  hints.ai_family = _{AF_INET};)" EOL
  R"(  hints.ai_socktype = _{SOCK_STREAM};)" EOL
  R"(  hints.ai_protocol = _{IPPROTO_TCP};)" EOL
  R"(  if (_{getaddrinfo}(hostname, port, &hints, &addr) != 0) {)" EOL
  // todo test
  R"(    _{fprintf}(_{stderr}, "Error: failed to resolve hostname address" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{free}(hostname);)" EOL
  R"(  _{struct request} *req = _{alloc}(sizeof(_{struct request}));)" EOL
  R"(  req->fd = _{socket}(addr->ai_family, addr->ai_socktype, addr->ai_protocol);)" EOL
  R"(  req->ctx = _{NULL};)" EOL
  R"(  req->ssl = _{NULL};)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} socket_res = req->fd != _{INVALID_SOCKET};)" EOL
  R"(  #else)" EOL
  R"(    _{bool} socket_res = req->fd != -1;)" EOL
  R"(  #endif)" EOL
  R"(  if (!socket_res) {)" EOL
  // todo test
  R"(    _{fprintf}(_{stderr}, "Error: failed to create socket" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{bool} connect_res = _{connect}(req->fd, addr->ai_addr, (int) addr->ai_addrlen) != _{SOCKET_ERROR};)" EOL
  R"(  #else)" EOL
  R"(    _{bool} connect_res = _{connect}(req->fd, addr->ai_addr, addr->ai_addrlen) != -1;)" EOL
  R"(  #endif)" EOL
  R"(  if (!connect_res) {)" EOL
  // todo test
  R"(    char *origin = _{str_cstr}(url->__THE_0_origin);)" EOL
  R"(    _{fprintf}(_{stderr}, "Error: failed to connect to `%s`" _{THE_EOL}, origin);)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{freeaddrinfo}(addr);)" EOL
  R"(  if (_{strcmp}(port, "443") == 0) {)" EOL
  R"(    if (!_{lib_openssl_init}) {)" EOL
  R"(      _{SSL_library_init}();)" EOL
  R"(      _{lib_openssl_init} = _{true};)" EOL
  R"(    })" EOL
  R"(    req->ctx = _{SSL_CTX_new}(_{TLS_client_method}());)" EOL
  R"(    if (req->ctx == _{NULL}) {)" EOL
  // todo test
  R"(      _{fprintf}(_{stderr}, "Error: failed to create SSL context" _{THE_EOL});)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    req->ssl = _{SSL_new}(req->ctx);)" EOL
  R"(    _{SSL_set_fd}(req->ssl, (int) req->fd);)" EOL
  R"(    if (_{SSL_connect}(req->ssl) != 1) {)" EOL
  // todo test
  R"(      _{fprintf}(_{stderr}, "Error: failed to connect to socket with SSL" _{THE_EOL});)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  char *req_headers = _{request_stringifyHeaders}(headers, url, data);)" EOL
  R"(  _{array_request_Header_free}(headers);)" EOL
  R"(  char *req_method = _{str_cstr}(method);)" EOL
  R"(  _{str_free}(method);)" EOL
  R"(  char *req_path = _{str_cstr}(url->__THE_0_path);)" EOL
  R"(  char *fmt = "%s %s HTTP/1.1\r\n%s\r\n";)" EOL
  R"(  _{size_t} req_len = _{snprintf}(_{NULL}, 0, fmt, req_method, req_path, req_headers);)" EOL
  R"(  char *request = _{alloc}(req_len + (data.l == 0 ? 0 : data.l + 2) + 1);)" EOL
  R"(  _{sprintf}(request, fmt, req_method, req_path, req_headers);)" EOL
  R"(  _{free}(req_path);)" EOL
  R"(  _{free}(req_method);)" EOL
  R"(  _{free}(req_headers);)" EOL
  R"(  if (data.l != 0) {)" EOL
  R"(    _{memcpy}(&request[req_len], data.d, data.l);)" EOL
  R"(    req_len += data.l;)" EOL
  R"(    _{memcpy}(&request[req_len], "\r\n", 3);)" EOL
  R"(    req_len += 2;)" EOL
  R"(  })" EOL
  R"(  _{buffer_free}(data);)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{SSIZE_T} y = 0, z = 0;)" EOL
  R"(  #else)" EOL
  R"(    _{ssize_t} y = 0, z = 0;)" EOL
  R"(  #endif)" EOL
  R"(  while (y < req_len) {)" EOL
  R"(    z = req->ssl == _{NULL})" EOL
  R"(      ? _{send}(req->fd, &request[y], req_len - y, 0))" EOL
  R"(      : _{SSL_write}(req->ssl, &request[y], (int) (req_len - y));)" EOL
  R"(    if (z == -1) {)" EOL
  // todo test
  R"(      _{fprintf}(_{stderr}, "Error: failed to write to socket" _{THE_EOL});)" EOL
  R"(      _{exit}(_{EXIT_FAILURE});)" EOL
  R"(    })" EOL
  R"(    y += z;)" EOL
  R"(  })" EOL
  R"(  _{free}(request);)" EOL
  R"(  if (req->ssl == _{NULL}) {)" EOL
  R"(    #ifdef _{THE_OS_WINDOWS})" EOL
  R"(      _{shutdown}(req->fd, _{SD_SEND});)" EOL
  R"(    #else)" EOL
  R"(      _{shutdown}(req->fd, 1);)" EOL
  R"(    #endif)" EOL
  R"(  } else {)" EOL
  R"(    _{SSL_shutdown}(req->ssl);)" EOL
  R"(  })" EOL
  R"(  _{url_URL_free}(url);)" EOL
  R"(  return (struct _{request_Request} *) req;)" EOL
  R"(})" EOL,

  R"(struct _{request_Response} *request_read (struct _{request_Request} **r) {)" EOL
  R"(  _{struct request} *req = (void *) *r;)" EOL
  R"(  unsigned char b[1024];)" EOL
  R"(  #ifdef _{THE_OS_WINDOWS})" EOL
  R"(    _{SSIZE_T} y;)" EOL
  R"(  #else)" EOL
  R"(    _{ssize_t} y;)" EOL
  R"(  #endif)" EOL
  R"(  _{struct buffer} data = {_{NULL}, 0};)" EOL
  R"(  while ((y = (req->ssl == _{NULL} ? _{recv}(req->fd, b, sizeof(b), 0) : _{SSL_read}(req->ssl, b, sizeof(b)))) > 0) {)" EOL
  R"(    data.d = _{re_alloc}(data.d, data.l + y);)" EOL
  R"(    _{memcpy}(&data.d[data.l], b, y);)" EOL
  R"(    data.l += y;)" EOL
  R"(  })" EOL
  R"(  if (y < 0) {)" EOL
  // todo test
  R"(    _{fprintf}(_{stderr}, "Error: failed to read from socket" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{size_t} i;)" EOL
  R"(  if (data.l > 8 && (_{memcmp}(data.d, "HTTP/1.0 ", 9) == 0 || _{memcmp}(data.d, "HTTP/1.1 ", 9) == 0)) {)" EOL
  R"(    i = 9;)" EOL
  R"(  } else if (data.l > 6 && (_{memcmp}(data.d, "HTTP/2 ", 7) == 0 || _{memcmp}(data.d, "HTTP/3 ", 7) == 0)) {)" EOL
  R"(    i = 7;)" EOL
  R"(  } else {)" EOL
  // todo test
  R"(    _{fprintf}(_{stderr}, "Error: invalid response HTTP version" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{size_t} status_start = i;)" EOL
  R"(  while (i < data.l && _{isdigit}(data.d[i])) i++;)" EOL
  R"(  if (status_start == i) {)" EOL
  // todo test
  R"(    _{fprintf}(_{stderr}, "Error: invalid response HTTP status code" _{THE_EOL});)" EOL
  R"(    _{exit}(_{EXIT_FAILURE});)" EOL
  R"(  })" EOL
  R"(  _{size_t} status_len = i - status_start;)" EOL
  R"(  char *status = _{alloc}(status_len + 1);)" EOL
  R"(  _{memcpy}(status, &data.d[status_start], status_len);)" EOL
  R"(  status[status_len] = '\0';)" EOL
  R"(  _{int32_t} status_code = (_{int32_t}) _{strtoul}(status, _{NULL}, 10);)" EOL
  R"(  _{free}(status);)" EOL
  R"(  while (i < data.l) {)" EOL
  R"(    if (data.d[i] == '\r' && i + 1 < data.l && data.d[i + 1] == '\n') {)" EOL
  R"(      i += 2;)" EOL
  R"(      break;)" EOL
  R"(    })" EOL
  R"(    i++;)" EOL
  R"(  })" EOL
  R"(  struct _{array_request_Header} headers = {_{NULL}, 0};)" EOL
  R"(  while (_{true}) {)" EOL
  R"(    if (data.d[i] == '\r' && i + 1 < data.l && data.d[i + 1] == '\n') {)" EOL
  R"(      i += 2;)" EOL
  R"(      break;)" EOL
  R"(    })" EOL
  R"(    _{size_t} header_name_start = i;)" EOL
  R"(    _{size_t} header_name_end = 0;)" EOL
  R"(    _{size_t} header_value_start = 0;)" EOL
  R"(    _{size_t} header_value_end = 0;)" EOL
  R"(    while (i < data.l) {)" EOL
  R"(      if (data.d[i] == '\r' && i + 1 < data.l && data.d[i + 1] == '\n') {)" EOL
  R"(        header_value_end = i;)" EOL
  R"(        i += 2;)" EOL
  R"(        break;)" EOL
  R"(      } else if (data.d[i] == ':' && header_name_end == 0) {)" EOL
  R"(        header_name_end = i;)" EOL
  R"(        i++;)" EOL
  R"(        while (i < data.l && data.d[i] == ' ') i++;)" EOL
  R"(        header_value_start = i;)" EOL
  R"(      })" EOL
  R"(      i++;)" EOL
  R"(    })" EOL
  R"(    _{struct str} header_name;)" EOL
  R"(    header_name.l = header_name_end - header_name_start;)" EOL
  R"(    header_name.d = _{alloc}(header_name.l);)" EOL
  R"(    _{memcpy}(header_name.d, &data.d[header_name_start], header_name.l);)" EOL
  R"(    for (_{size_t} j = 0; j < header_name.l; j++) header_name.d[j] = (char) _{tolower}(header_name.d[j]);)" EOL
  R"(    _{struct str} header_value;)" EOL
  R"(    header_value.l = header_value_end - header_value_start;)" EOL
  R"(    header_value.d = _{alloc}(header_value.l);)" EOL
  R"(    _{memcpy}(header_value.d, &data.d[header_value_start], header_value.l);)" EOL
  R"(    headers.d = _{re_alloc}(headers.d, (headers.l + 1) * sizeof(struct _{request_Header} *));)" EOL
  R"(    headers.d[headers.l] = _{request_Header_alloc}(header_name, header_value);)" EOL
  R"(    headers.l++;)" EOL
  R"(  })" EOL
  R"(  for (_{size_t} j = 0; j < headers.l; j++) {)" EOL
  R"(    for (_{size_t} k = j + 1; k < headers.l; k++) {)" EOL
  R"(      _{size_t} count = headers.d[j]->__THE_0_name.l > headers.d[k]->__THE_0_name.l ? headers.d[k]->__THE_0_name.l : headers.d[j]->__THE_0_name.l;)" EOL
  R"(      if (_{memcmp}(headers.d[j]->__THE_0_name.d, headers.d[k]->__THE_0_name.d, count) > 0) {)" EOL
  R"(        struct _{request_Header} *a = headers.d[j];)" EOL
  R"(        headers.d[j] = headers.d[k];)" EOL
  R"(        headers.d[k] = a;)" EOL
  R"(      })" EOL
  R"(    })" EOL
  R"(  })" EOL
  R"(  if (data.l - i > 0) {)" EOL
  R"(    data.l = data.l - i;)" EOL
  R"(    _{memmove}(data.d, &data.d[i], data.l);)" EOL
  R"(    data.d = _{re_alloc}(data.d, data.l);)" EOL
  R"(  } else {)" EOL
  R"(    _{free}(data.d);)" EOL
  R"(    data.d = _{NULL};)" EOL
  R"(    data.l = 0;)" EOL
  R"(  })" EOL
  R"(  return _{request_Response_alloc}(data, status_code, headers);)" EOL
  R"(})" EOL
};
