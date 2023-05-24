typedef struct {
  wchar_t *d;
  size_t c;
  size_t l;
  unsigned char a;
} str_t;

str_t str_alloc (wchar_t *n1) {
  size_t l = wcslen(n1);
  return (str_t) {n1, l, l, 0};
}

void str_free (str_t n1) {
  if (n1.a) free(n1.d);
}

str_t str_realloc (str_t n1, str_t n2) {
  if (n1.a) free(n1.d);
  return n2;
}

str_t str_copy (str_t n1) {
  wchar_t *d = malloc((n1.l + 1) * sizeof(wchar_t));
  memcpy(d, n1.d, (n1.l + 1) * sizeof(wchar_t));
  return (str_t) {d, n1.l, n1.l, 1};
}
