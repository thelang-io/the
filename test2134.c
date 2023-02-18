#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pair_str_str {
  char *f;
  char *s;
};

struct map_str_str {
  struct pair_str_str *d;
  size_t l;
  size_t c;
};

_Noreturn void panic (const char *message) {
  fprintf(stderr, "Error: %s\n", message);
  exit(1);
}

struct map_str_str map_str_str_alloc (int x, ...) {
  struct pair_str_str *d = malloc(sizeof(struct pair_str_str) * x);
  va_list args;
  va_start(args, x);

  for (int i = 0; i < x; i++) {
    d[i] = (struct pair_str_str) {va_arg(args, char *), va_arg(args, char *)};
  }

  va_end(args);
  return (struct map_str_str) {d, x, x};
}

void map_str_str_free (struct map_str_str *n) {
  free(n->d);
}

struct map_str_str *map_str_str_set (struct map_str_str *n, char *k, char *v) {
  n->l++;

  if (n->c < n->l) {
    n->c = n->l;
    n->d = realloc(n->d, sizeof(struct pair_str_str) * n->c);
    printf("realloc\n");
  }

  n->d[n->l - 1] = (struct pair_str_str) {k, v};
  return n;
}

char *map_str_str_get (struct map_str_str *n, char *k) {
  for (size_t i = 0; i < n->l; i++) {
    struct pair_str_str it = n->d[i];

    if (strcmp(it.f, k) == 0) {
      return it.s;
    }
  }

  panic("map key was not found");
}

bool map_str_str_has (struct map_str_str *n, char *k) {
  for (size_t i = 0; i < n->l; i++) {
    struct pair_str_str it = n->d[i];

    if (strcmp(it.f, k) == 0) {
      return true;
    }
  }

  return false;
}

struct map_str_str *map_str_str_remove (struct map_str_str *n, char *k) {
  for (size_t i = 0; i < n->l; i++) {
    if (strcmp(n->d[i].f, k) == 0) {
      memmove(&n->d[i], &n->d[i + 1], (--n->l - i) * sizeof(struct pair_str_str));
      return n;
    }
  }

  panic("map key was not found");
}

struct map_str_str *map_str_str_clear (struct map_str_str *n) {
  n->l = 0;
  return n;
}

bool map_str_str_empty (struct map_str_str *n) {
  return n->l == 0;
}

struct map_str_str *map_str_str_reserve (struct map_str_str *n, int c) {
  if (n->c < c) {
    n->c = c;
    n->d = realloc(n->d, n->c * sizeof(struct pair_str_str));
    printf("realloc\n");
  }

  return n;
}

struct map_str_str *map_str_str_shrink (struct map_str_str *n) {
  if (n->c > n->l) {
    n->c = n->l;
    n->d = realloc(n->d, n->c * sizeof(struct pair_str_str));
    printf("realloc\n");
  }

  return n;
}

void print_map_str_str (const char *prefix, struct map_str_str *n) {
  printf("%s|%zu|%zu|%s {", prefix, n->c, n->l, map_str_str_empty(n) ? "true" : "false");

  for (size_t i = 0; i < n->l; i++) {
    struct pair_str_str it = n->d[i];
    printf("%s\"%s\": \"%s\"", i == 0 ? "" : ", ", it.f, it.s);
  }

  printf("}\n");
}

int main () {
  struct map_str_str test0 = map_str_str_alloc(0);
  print_map_str_str("test0", &test0);
  map_str_str_free(&test0);

  struct map_str_str test1 = map_str_str_alloc(3, "key1", "value1", "key2", "value2", "key3", "value3");
  print_map_str_str("test1", &test1);
  map_str_str_free(&test1);

  struct map_str_str test = map_str_str_alloc(1, "key", "value");
  print_map_str_str("test", &test);

  printf("clear\n");
  map_str_str_clear(&test);
  print_map_str_str("test", &test);

  printf("set key1\n");
  map_str_str_set(&test, "key1", "value1");
  print_map_str_str("test", &test);

  printf("set key2\n");
  map_str_str_set(&test, "key2", "value2");
  print_map_str_str("test", &test);

  printf("reserve 0\n");
  map_str_str_reserve(&test, 0);
  print_map_str_str("test", &test);

  printf("reserve 1000\n");
  map_str_str_reserve(&test, 1000);
  print_map_str_str("test", &test);

  printf("shrink\n");
  map_str_str_shrink(&test);
  print_map_str_str("test", &test);

  printf("remove key2\n");
  map_str_str_remove(&test, "key2");
  print_map_str_str("test", &test);

  printf("\nget key1: %s\n", map_str_str_get(&test, "key1"));
  printf("has key1: %s\n", map_str_str_has(&test, "key1") ? "true" : "false");
  printf("has key2: %s\n", map_str_str_has(&test, "key2") ? "true" : "false");
  printf("has key3: %s\n", map_str_str_has(&test, "key3") ? "true" : "false");

  map_str_str_free(&test);
}
