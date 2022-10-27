#include <openssl/ssl.h>
#include <openssl/err.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct request {
  int fd;
  SSL_CTX *ctx;
  SSL *ssl;
};

struct request_parsed_url {
  char *hostname;
  unsigned short port;
  char *path;
};

struct request_header {
  char *name;
  char *value;
};

char *request_parse_headers (struct request_header *headers, size_t headers_len, char *hostname, size_t data_len) {
  bool has_content_length = false;
  bool has_host = false;
  size_t name_len = 0;
  char *name = NULL;
  char *d = NULL;
  size_t l = 0;

  for (size_t i = 0; i < headers_len; i++) {
    struct request_header header = headers[i];
    size_t header_name_len = strlen(header.name);
    size_t header_value_len = strlen(header.value);

    if (name_len < header_name_len) {
      name_len = header_name_len;
      name = realloc(name, name_len + 1);
    }

    strcpy(name, header.name);
    for (size_t j = 0; j < header_name_len; j++) name[j] = (char) tolower(name[j]);

    if (strcmp(name, "content-length") == 0) {
      has_content_length = true;
    } else if (strcmp(name, "host") == 0) {
      has_host = true;
    }

    d = realloc(d, l + header_name_len + 2 + header_value_len + 3);
    memcpy(&d[l], header.name, header_name_len);
    memcpy(&d[l + header_name_len], ": ", 2);
    memcpy(&d[l + header_name_len + 2], header.value, header_value_len);
    memcpy(&d[l + header_name_len + 2 + header_value_len], "\r\n", 3);
    l += header_name_len + 2 + header_value_len + 2;
  }

  if (!has_host) {
    size_t z = snprintf(NULL, 0, "Host: %s\r\n", hostname);
    d = realloc(d, l + z + 1);
    sprintf(&d[l], "Host: %s\r\n", hostname);
    l += z;
  }

  if (!has_content_length) {
    size_t z = snprintf(NULL, 0, "Content-Length: %zu\r\n", data_len);
    d = realloc(d, l + z + 1);
    sprintf(&d[l], "Content-Length: %zu\r\n", data_len);
    l += z;
  }

  return d;
}

struct request_parsed_url request_parseUrl (char *url) {
  size_t url_len = strlen(url);
  size_t protocol_len = 0;

  if (url_len >= 8 && memcmp(url, "https://", 8) == 0) {
    protocol_len = 8;
  } else if (url_len >= 7 && memcmp(url, "http://", 7) == 0) {
    protocol_len = 7;
  } else {
    fprintf(stderr, "bad protocol\n");
    exit(EXIT_FAILURE);
  }

  unsigned short port = protocol_len == 8 ? 443 : 80;
  bool host_has_port = false;
  size_t hostname_end = 0;
  size_t host_port_end = 0;

  for (size_t i = protocol_len; i < url_len; i++) {
    char ch = url[i];

    if (ch == ':' && host_has_port) {
      fprintf(stderr, "ipv6 hostname addresses are not supported\n");
      exit(EXIT_FAILURE);
    } else if (ch == ':') {
      host_has_port = true;
      hostname_end = i;
    } else if (ch == '/' && host_has_port) {
      host_port_end = i;
      break;
    } else if (ch == '/') {
      hostname_end = i;
      break;
    } else if (!isalnum(ch) && ch != '-' && ch != '.') {
      fprintf(stderr, "malformed request url hostname\n");
      exit(EXIT_FAILURE);
    }
  }

  if (hostname_end == 0) {
    hostname_end = url_len;
  }

  size_t hostname_len = hostname_end - protocol_len;
  char *hostname = malloc(hostname_len + 1);
  memcpy(hostname, &url[protocol_len], hostname_len);
  hostname[hostname_len] = '\0';

  if (host_has_port) {
    size_t host_port_len = host_port_end - hostname_end;
    char *host_port = malloc(host_port_len + 1);
    memcpy(host_port, &url[hostname_end], host_port_len);
    host_port[host_port_len] = '\0';
    port = strtoul(host_port, NULL, 10);
  }

  size_t host_end = host_has_port ? host_port_end : hostname_end;
  size_t path_len = url_len - host_end;
  char *path;

  if (path_len == 0) {
    path = malloc(2);
    path[0] = '/';
    path[1] = '\0';
  } else {
    path = malloc(path_len + 1);
    memcpy(path, &url[host_end], path_len);
    path[path_len] = '\0';
  }

  return (struct request_parsed_url) {hostname, port, path};
}

struct request request_open (char *method, char *url, unsigned char *data, size_t data_len, struct request_header *headers, size_t headers_len) {
  struct request_parsed_url parsed_url = request_parseUrl(url);
  struct request req = {0, NULL, NULL};

  req.fd = socket(AF_INET, SOCK_STREAM, 0);

  if (req.fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct hostent *hostent = gethostbyname(parsed_url.hostname);

  if (hostent == NULL) {
    fprintf(stderr, "error: gethostbyname(\"%s\")\n", parsed_url.hostname);
    exit(EXIT_FAILURE);
  }

  char *hostname_ip = inet_ntoa(*(struct in_addr *) hostent->h_addr_list[0]);
  in_addr_t in_addr = inet_addr(hostname_ip);
  if (in_addr == (in_addr_t) -1) {
    fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_addr.s_addr = in_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(parsed_url.port);

  if (connect(req.fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  if (parsed_url.port == 443) {
    SSL_library_init();
    req.ctx = SSL_CTX_new(TLS_client_method());

    if (req.ctx == NULL) {
      fprintf(stderr, "error: SSL_CTX_new(method)\n");
      exit(EXIT_FAILURE);
    }

    req.ssl = SSL_new(req.ctx);
    SSL_set_fd(req.ssl, req.fd);
    SSL_set_tlsext_host_name(req.ssl, parsed_url.hostname);
    SSL_set_verify(req.ssl, SSL_VERIFY_NONE, NULL);
    SSL_set1_host(req.ssl, parsed_url.hostname);

    if (SSL_connect(req.ssl) != 1) {
      fprintf(stderr, "error: SSL_connect(req.ssl)\n");
      ERR_print_errors_fp(stderr);
      exit(EXIT_FAILURE);
    }
  }

  char *parsed_headers = request_parse_headers(headers, headers_len, parsed_url.hostname, data_len);
  char *request_template = "%s %s HTTP/1.1\r\n%s\r\n";
  size_t request_len = snprintf(NULL, 0, request_template, method, parsed_url.path, parsed_headers);

  if (request_len == 0) {
    fprintf(stderr, "request length failed\n");
    exit(EXIT_FAILURE);
  }

  char *request = malloc(request_len + (data_len == 0 ? 0 : data_len + 2) + 1);
  sprintf(request, request_template, method, parsed_url.path, parsed_headers);

  if (data_len != 0) {
    memcpy(&request[request_len], data, data_len);
    request_len += data_len;
    memcpy(&request[request_len], "\r\n", 3);
    request_len += 2;
  }

  ssize_t nbytes_write = 0;

  while (nbytes_write < request_len) {
    ssize_t nbytes_last = req.ssl == NULL
      ? write(req.fd, &request[nbytes_write], request_len - nbytes_write)
      : SSL_write(req.ssl, &request[nbytes_write], (int) (request_len - nbytes_write));

    if (nbytes_last == -1) {
      fprintf(stderr, "error: write()\n");
      exit(EXIT_FAILURE);
    }

    nbytes_write += nbytes_last;
  }

  free(request);
  (req.ssl == NULL ? shutdown(req.fd, 1) : SSL_shutdown(req.ssl));

  return req;
}

// data
// status
// headers

char *request_read (struct request req, size_t *l) {
  char d[1024];
  ssize_t y;
  char *r = NULL;
  *l = 0;

  while ((y = (req.ssl == NULL ? read(req.fd, d, sizeof(d)) : SSL_read(req.ssl, d, sizeof(d)))) > 0) {
    r = realloc(r, *l + y + 1);
    memcpy(&r[*l], d, y);
    *l += y;
  }

  if (y < 0) {
    fprintf(stderr, "error: read()\n");
    exit(EXIT_FAILURE);
  }

  if (r != NULL) {
    r[*l] = '\0';
  }

  return r;
}

void request_close (struct request req) {
  if (req.ssl != NULL) {
    SSL_CTX_free(req.ctx);
    SSL_free(req.ssl);
  }

  close(req.fd);
}

int main () {
  struct request req1 = request_open("GET", "https://api.thelang.io/search-docs?q=hello", NULL, 0, NULL, 0);

  size_t l1 = 0;
  char *req1_data = request_read(req1, &l1);

  fwrite(req1_data, 1, l1, stdout);
  request_close(req1);
}
