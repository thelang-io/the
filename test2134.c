#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
  #define THE_OS_WINDOWS
  #define THE_EOL "\r\n"
  #define THE_PATH_SEP "\\"
#else
  #if defined(__APPLE__)
    #define THE_OS_MACOS
  #elif defined(__linux__)
    #define THE_OS_LINUX
  #endif
  #define THE_EOL "\n"
  #define THE_PATH_SEP "/"
#endif

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE_YAMLNodeMap_0 1
#define TYPE_YAMLNodeMapping_0 2
#define TYPE_YAMLNodeScalar_0 3
#define TYPE_YAMLNodeSeq_0 4

enum __THE_1_YAMLTokenType_0 {
  __THE_0_YAMLTokenTypeSDWhitespace_0,
  __THE_0_YAMLTokenTypeSDEOL_0,
  __THE_0_YAMLTokenTypeSDColon_0,
  __THE_0_YAMLTokenTypeSDMinus_0,
  __THE_0_YAMLTokenTypeSDLBrack_0,
  __THE_0_YAMLTokenTypeSDRBrack_0,
  __THE_0_YAMLTokenTypeSDLBrace_0,
  __THE_0_YAMLTokenTypeSDRBrace_0,
  __THE_0_YAMLTokenTypeSDComma_0,
  __THE_0_YAMLTokenTypeSDText_0,
  __THE_0_YAMLTokenTypeSDComment_0
};

struct buffer {
  unsigned char *d;
  size_t l;
};
struct str {
  char *d;
  size_t l;
};

struct __THE_1_YAMLToken_0;
struct __THE_1_array_YAMLNodeMapping;
struct __THE_1_YAMLNodeMap_0;
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE;
struct __THE_1_YAMLNodeMapping_0;
struct __THE_1_YAMLNodeScalar_0;
struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE;
struct __THE_1_YAMLNodeSeq_0;
struct __THE_1_YAMLState_0;
struct __THE_1_array_YAMLToken;
struct __THE_1_YAML_0;
struct __THE_1_fn$1017;
struct __THE_1_fn$1017P;
struct __THE_1_fn$1015;
struct __THE_1_fn$1015P;
struct __THE_1_fn$1011;
struct __THE_1_fn$1009;
struct __THE_1_YAML_parse_0X;
struct __THE_1_fn$1018;
struct __THE_1_fn$1018P;
struct __THE_1_YAMLNode_stringify_0X;
struct __THE_1_fn$1010;
struct __THE_1_fn$1012;
struct __THE_1_fn$1013;
struct __THE_1_fn$1014;
struct __THE_1_fn$1014P;
struct __THE_1_YAMLSD_nextTkIndent_0X;
struct __THE_1_YAMLSD_nextTkIs_0X;
struct __THE_1_array_char;
struct __THE_1_fn$1016;
struct __THE_1_fn$1016P;
struct __THE_1_YAMLSD_parse_0X;

struct __THE_1_YAMLToken_0 {
  const enum __THE_1_YAMLTokenType_0 __THE_0_t;
  const struct str __THE_0_val;
};
struct __THE_1_array_YAMLNodeMapping {
  struct __THE_1_YAMLNodeMapping_0 **d;
  size_t l;
};
struct __THE_1_YAMLNodeMap_0 {
  const struct __THE_1_array_YAMLNodeMapping __THE_0_mappings;
};
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE {
  int t;
  union {
    struct __THE_1_YAMLNodeMap_0 *v1;
    struct __THE_1_YAMLNodeMapping_0 *v2;
    struct __THE_1_YAMLNodeScalar_0 *v3;
    struct __THE_1_YAMLNodeSeq_0 *v4;
  };
};
struct __THE_1_YAMLNodeMapping_0 {
  const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_key;
  const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_0_value;
};
struct __THE_1_YAMLNodeScalar_0 {
  const struct str __THE_0_value;
};
struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *d;
  size_t l;
};
struct __THE_1_YAMLNodeSeq_0 {
  const struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_items;
};
struct __THE_1_YAMLState_0 {
  int32_t __THE_0_pos;
};
struct __THE_1_array_YAMLToken {
  struct __THE_1_YAMLToken_0 **d;
  size_t l;
};
struct __THE_1_YAML_0 {
  const struct str __THE_0_content;
  struct __THE_1_array_YAMLToken __THE_0_tokens;
  struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_nodes;
  struct __THE_1_YAMLState_0 *__THE_0_state;
};
struct __THE_1_fn$1017 {
  struct __THE_1_YAML_0 *(*f) (void *, struct __THE_1_fn$1017P);
  void *x;
  size_t l;
};
struct __THE_1_fn$1017P {
  struct str n0;
};
struct __THE_1_fn$1015 {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *(*f) (void *, struct __THE_1_YAML_0 **, struct __THE_1_fn$1015P);
  void *x;
  size_t l;
};
struct __THE_1_fn$1015P {
  unsigned char o0;
  int32_t n0;
};
struct __THE_1_fn$1011 {
  void (*f) (void *, struct __THE_1_YAML_0 **);
  void *x;
  size_t l;
};
struct __THE_1_fn$1009 {
  bool (*f) (void *, const struct __THE_1_YAML_0 **);
  void *x;
  size_t l;
};
struct __THE_1_YAML_parse_0X {
  const struct __THE_1_fn$1015 *__THE_0_YAMLSD_parse_0;
  const struct __THE_1_fn$1011 *__THE_0_YAMLSD_tokenize_0;
  const struct __THE_1_fn$1009 *__THE_0_YAMLSD_hasNextTk_0;
  const struct __THE_1_fn$1011 *__THE_0_YAMLSDreset_0;
};
struct __THE_1_fn$1018 {
  struct str (*f) (void *, struct __THE_1_fn$1018P);
  void *x;
  size_t l;
};
struct __THE_1_fn$1018P {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n0;
  unsigned char o1;
  int32_t n1;
};
struct __THE_1_YAMLNode_stringify_0X {
  const struct __THE_1_fn$1018 *__THE_0_YAMLNode_stringify_0;
};
struct __THE_1_fn$1010 {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE (*f) (void *, struct __THE_1_YAML_0 **);
  void *x;
  size_t l;
};
struct __THE_1_fn$1012 {
  struct __THE_1_YAMLToken_0 *(*f) (void *, struct __THE_1_YAML_0 **);
  void *x;
  size_t l;
};
struct __THE_1_fn$1013 {
  int32_t (*f) (void *, const struct __THE_1_YAML_0 **);
  void *x;
  size_t l;
};
struct __THE_1_fn$1014 {
  bool (*f) (void *, struct __THE_1_YAML_0 **, struct __THE_1_fn$1014P);
  void *x;
  size_t l;
};
struct __THE_1_fn$1014P {
  enum __THE_1_YAMLTokenType_0 n0;
};
struct __THE_1_YAMLSD_nextTkIndent_0X {
  const struct __THE_1_fn$1014 *__THE_0_YAMLSD_nextTkIs_0;
  const struct __THE_1_fn$1009 *__THE_0_YAMLSD_hasNextTk_0;
};
struct __THE_1_YAMLSD_nextTkIs_0X {
  const struct __THE_1_fn$1012 *__THE_0_YAMLSD_nextTk_0;
  const struct __THE_1_fn$1009 *__THE_0_YAMLSD_hasNextTk_0;
};
struct __THE_1_array_char {
  char *d;
  size_t l;
};
struct __THE_1_fn$1016 {
  struct str (*f) (void *, struct __THE_1_YAML_0 **, struct __THE_1_fn$1016P);
  void *x;
  size_t l;
};
struct __THE_1_fn$1016P {
  enum __THE_1_YAMLTokenType_0 n0;
};
struct __THE_1_YAMLSD_parse_0X {
  const struct __THE_1_fn$1016 *__THE_0_YAMLSD_stringifyTill_0;
  const struct __THE_1_fn$1015 *__THE_0_YAMLSD_parse_0;
  const struct __THE_1_fn$1014 *__THE_0_YAMLSD_nextTkIs_0;
  const struct __THE_1_fn$1013 *__THE_0_YAMLSD_nextTkIndent_0;
  const struct __THE_1_fn$1012 *__THE_0_YAMLSD_nextTk_0;
};

void *alloc (size_t);
struct str buffer_to_str (struct buffer);
bool char_isWhitespace (char);
struct str char_repeat (char, int32_t);
struct str char_str (char);
struct buffer fs_readFileSync (struct str);
void print (FILE *, const char *, ...);
void *re_alloc (void *, size_t);
struct str str_alloc (const char *);
char *str_at (struct str, int32_t);
struct str str_concat_cstr (struct str, const char *);
struct str str_concat_str (struct str, struct str);
struct str str_copy (const struct str);
char *str_cstr (const struct str);
bool str_empty (struct str);
void str_free (struct str);
size_t str_len (struct str);
bool str_ne_str (struct str, struct str);
struct str str_realloc (struct str, struct str);
struct str str_slice (struct str, unsigned char, int32_t, unsigned char, int32_t);
struct str str_trim (struct str);
struct str str_trimStart (struct str);
struct __THE_1_YAMLToken_0 *__THE_1_YAMLToken_0_alloc (enum __THE_1_YAMLTokenType_0, struct str);
struct __THE_1_YAMLToken_0 *__THE_1_YAMLToken_0_copy (const struct __THE_1_YAMLToken_0 *);
void __THE_1_YAMLToken_0_free (struct __THE_1_YAMLToken_0 *);
struct __THE_1_array_YAMLNodeMapping __THE_1_array_YAMLNodeMapping_alloc (size_t, ...);
struct __THE_1_YAMLNodeMapping_0 **__THE_1_array_YAMLNodeMapping_at (struct __THE_1_array_YAMLNodeMapping, int32_t);
struct __THE_1_array_YAMLNodeMapping __THE_1_array_YAMLNodeMapping_copy (const struct __THE_1_array_YAMLNodeMapping);
void __THE_1_array_YAMLNodeMapping_free (struct __THE_1_array_YAMLNodeMapping);
size_t __THE_1_array_YAMLNodeMapping_len (struct __THE_1_array_YAMLNodeMapping);
bool __THE_1_array_YAMLNodeMapping_ne (struct __THE_1_array_YAMLNodeMapping, struct __THE_1_array_YAMLNodeMapping);
void __THE_1_array_YAMLNodeMapping_push (struct __THE_1_array_YAMLNodeMapping *, struct __THE_1_array_YAMLNodeMapping);
struct __THE_1_YAMLNodeMap_0 *__THE_1_YAMLNodeMap_0_alloc (struct __THE_1_array_YAMLNodeMapping);
struct __THE_1_YAMLNodeMap_0 *__THE_1_YAMLNodeMap_0_copy (const struct __THE_1_YAMLNodeMap_0 *);
void __THE_1_YAMLNodeMap_0_free (struct __THE_1_YAMLNodeMap_0 *);
bool __THE_1_YAMLNodeMap_0_ne (struct __THE_1_YAMLNodeMap_0 *, struct __THE_1_YAMLNodeMap_0 *);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc (int, ...);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy (const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
void __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
bool __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy (const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *);
void __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *);
bool __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *);
struct __THE_1_YAMLNodeMapping_0 *__THE_1_YAMLNodeMapping_0_alloc (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *);
struct __THE_1_YAMLNodeMapping_0 *__THE_1_YAMLNodeMapping_0_copy (const struct __THE_1_YAMLNodeMapping_0 *);
void __THE_1_YAMLNodeMapping_0_free (struct __THE_1_YAMLNodeMapping_0 *);
bool __THE_1_YAMLNodeMapping_0_ne (struct __THE_1_YAMLNodeMapping_0 *, struct __THE_1_YAMLNodeMapping_0 *);
struct __THE_1_YAMLNodeScalar_0 *__THE_1_YAMLNodeScalar_0_alloc (struct str);
struct __THE_1_YAMLNodeScalar_0 *__THE_1_YAMLNodeScalar_0_copy (const struct __THE_1_YAMLNodeScalar_0 *);
void __THE_1_YAMLNodeScalar_0_free (struct __THE_1_YAMLNodeScalar_0 *);
bool __THE_1_YAMLNodeScalar_0_ne (struct __THE_1_YAMLNodeScalar_0 *, struct __THE_1_YAMLNodeScalar_0 *);
struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc (size_t, ...);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_at (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE, int32_t);
struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy (const struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
bool __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_empty (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
void __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
size_t __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_len (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
bool __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE, struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
void __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_push (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *, struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
struct __THE_1_YAMLNodeSeq_0 *__THE_1_YAMLNodeSeq_0_alloc (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
struct __THE_1_YAMLNodeSeq_0 *__THE_1_YAMLNodeSeq_0_copy (const struct __THE_1_YAMLNodeSeq_0 *);
void __THE_1_YAMLNodeSeq_0_free (struct __THE_1_YAMLNodeSeq_0 *);
bool __THE_1_YAMLNodeSeq_0_ne (struct __THE_1_YAMLNodeSeq_0 *, struct __THE_1_YAMLNodeSeq_0 *);
struct __THE_1_YAMLState_0 *__THE_1_YAMLState_0_alloc (int32_t);
struct __THE_1_YAMLState_0 *__THE_1_YAMLState_0_copy (const struct __THE_1_YAMLState_0 *);
void __THE_1_YAMLState_0_free (struct __THE_1_YAMLState_0 *);
struct __THE_1_YAMLState_0 *__THE_1_YAMLState_0_realloc (struct __THE_1_YAMLState_0 *, struct __THE_1_YAMLState_0 *);
struct __THE_1_array_YAMLToken __THE_1_array_YAMLToken_alloc (size_t, ...);
struct __THE_1_YAMLToken_0 **__THE_1_array_YAMLToken_at (struct __THE_1_array_YAMLToken, int32_t);
struct __THE_1_array_YAMLToken __THE_1_array_YAMLToken_copy (const struct __THE_1_array_YAMLToken);
void __THE_1_array_YAMLToken_free (struct __THE_1_array_YAMLToken);
size_t __THE_1_array_YAMLToken_len (struct __THE_1_array_YAMLToken);
void __THE_1_array_YAMLToken_push (struct __THE_1_array_YAMLToken *, struct __THE_1_array_YAMLToken);
struct __THE_1_YAML_0 *__THE_1_YAML_0_alloc (struct str, struct __THE_1_array_YAMLToken, struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE, struct __THE_1_YAMLState_0 *);
struct __THE_1_YAML_0 *__THE_1_YAML_0_copy (const struct __THE_1_YAML_0 *);
void __THE_1_YAML_0_free (struct __THE_1_YAML_0 *);
void __THE_1_fn$1017_free (struct __THE_1_fn$1017);
void __THE_1_fn$1015_free (struct __THE_1_fn$1015);
struct __THE_1_YAML_0 *__THE_1_YAML_parse_0 (void *, struct __THE_1_fn$1017P);
void __THE_1_YAML_parse_0_alloc (struct __THE_1_fn$1017 *, struct __THE_1_YAML_parse_0X);
void __THE_1_fn$1018_free (struct __THE_1_fn$1018);
struct str __THE_1_YAMLNode_stringify_0 (void *, struct __THE_1_fn$1018P);
void __THE_1_YAMLNode_stringify_0_alloc (struct __THE_1_fn$1018 *, struct __THE_1_YAMLNode_stringify_0X);
bool __THE_1_YAMLSDhasNext_0 (void *, const struct __THE_1_YAML_0 **);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_YAMLSDnext_0 (void *, struct __THE_1_YAML_0 **);
void __THE_1_YAMLSDreset_0 (void *, struct __THE_1_YAML_0 **);
bool __THE_1_YAMLSD_hasNextTk_0 (void *, const struct __THE_1_YAML_0 **);
struct __THE_1_YAMLToken_0 *__THE_1_YAMLSD_nextTk_0 (void *, struct __THE_1_YAML_0 **);
void __THE_1_fn$1013_free (struct __THE_1_fn$1013);
void __THE_1_fn$1014_free (struct __THE_1_fn$1014);
int32_t __THE_1_YAMLSD_nextTkIndent_0 (void *, const struct __THE_1_YAML_0 **);
void __THE_1_YAMLSD_nextTkIndent_0_alloc (struct __THE_1_fn$1013 *, struct __THE_1_YAMLSD_nextTkIndent_0X);
bool __THE_1_YAMLSD_nextTkIs_0 (void *, struct __THE_1_YAML_0 **, struct __THE_1_fn$1014P);
void __THE_1_YAMLSD_nextTkIs_0_alloc (struct __THE_1_fn$1014 *, struct __THE_1_YAMLSD_nextTkIs_0X);
struct __THE_1_array_char __THE_1_array_char_alloc (size_t, ...);
bool __THE_1_array_char_contains (struct __THE_1_array_char, char);
struct __THE_1_array_char __THE_1_array_char_copy (const struct __THE_1_array_char);
void __THE_1_array_char_free (struct __THE_1_array_char);
void __THE_1_YAMLSD_tokenize_0 (void *, struct __THE_1_YAML_0 **);
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_YAMLSD_parse_0 (void *, struct __THE_1_YAML_0 **, struct __THE_1_fn$1015P);
void __THE_1_YAMLSD_parse_0_alloc (struct __THE_1_fn$1015 *, struct __THE_1_YAMLSD_parse_0X);
struct str __THE_1_YAMLSD_stringifyTill_0 (void *, struct __THE_1_YAML_0 **, struct __THE_1_fn$1016P);

void *alloc (size_t l) {
  void *r = malloc(l);
  if (r == NULL) {
    fprintf(stderr, "Error: failed to allocate %zu bytes" THE_EOL, l);
    exit(EXIT_FAILURE);
  }
  return r;
}
struct str buffer_to_str (struct buffer b) {
  return (struct str) {(char *) b.d, b.l};
}
bool char_isWhitespace (char c) {
  return isspace(c);
}
struct str char_repeat (char c, int32_t k) {
  if (k <= 0) return str_alloc("");
  size_t l = (size_t) k;
  char *d = alloc(l);
  size_t i = 0;
  while (i < l) d[i++] = c;
  return (struct str) {d, l};
}
struct str char_str (char c) {
  char buf[2] = {c, '\0'};
  return str_alloc(buf);
}
struct buffer fs_readFileSync (struct str s) {
  char *c = str_cstr(s);
  FILE *f = fopen(c, "rb");
  if (f == NULL) {
    fprintf(stderr, "Error: failed to open file `%s` for reading" THE_EOL, c);
    exit(EXIT_FAILURE);
  }
  unsigned char *d = NULL;
  unsigned char b[4096];
  size_t l = 0;
  size_t y;
  while ((y = fread(b, 1, sizeof(b), f)) > 0) {
    d = re_alloc(d, l + y);
    memcpy(&d[l], b, y);
    l += y;
  }
  fclose(f);
  free(c);
  str_free((struct str) s);
  return (struct buffer) {d, l};
}
void print (FILE *stream, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char buf[512];
  struct str s;
  while (*fmt) {
    switch (*fmt++) {
      case 't': fputs(va_arg(args, int) ? "true" : "false", stream); break;
      case 'b': sprintf(buf, "%u", va_arg(args, unsigned)); fputs(buf, stream); break;
      case 'c': fputc(va_arg(args, int), stream); break;
      case 'e':
      case 'f':
      case 'g': snprintf(buf, 512, "%f", va_arg(args, double)); fputs(buf, stream); break;
      case 'h':
      case 'j':
      case 'v':
      case 'w': sprintf(buf, "%d", va_arg(args, int)); fputs(buf, stream); break;
      case 'i':
      case 'k': sprintf(buf, "%" PRId32, va_arg(args, int32_t)); fputs(buf, stream); break;
      case 'l': sprintf(buf, "%" PRId64, va_arg(args, int64_t)); fputs(buf, stream); break;
      case 'p': sprintf(buf, "%p", va_arg(args, void *)); fputs(buf, stream); break;
      case 's': s = va_arg(args, struct str); fwrite(s.d, 1, s.l, stream); str_free(s); break;
      case 'u': sprintf(buf, "%" PRIu32, va_arg(args, uint32_t)); fputs(buf, stream); break;
      case 'y': sprintf(buf, "%" PRIu64, va_arg(args, uint64_t)); fputs(buf, stream); break;
      case 'z': fputs(va_arg(args, char *), stream); break;
    }
  }
  va_end(args);
}
void *re_alloc (void *d, size_t l) {
  void *r = realloc(d, l);
  if (r == NULL) {
    fprintf(stderr, "Error: failed to reallocate %zu bytes" THE_EOL, l);
    exit(EXIT_FAILURE);
  }
  return r;
}
struct str str_alloc (const char *r) {
  size_t l = strlen(r);
  char *d = alloc(l);
  memcpy(d, r, l);
  return (struct str) {d, l};
}
char *str_at (struct str s, int32_t i) {
  if ((i >= 0 && i >= s.l) || (i < 0 && i < -((int32_t) s.l))) {
    fprintf(stderr, "Error: index %" PRId32 " out of string bounds" THE_EOL, i);
    exit(EXIT_FAILURE);
  }
  return i < 0 ? &s.d[s.l + i] : &s.d[i];
}
struct str str_concat_cstr (struct str s, const char *r) {
  size_t l = s.l + strlen(r);
  char *d = alloc(l);
  memcpy(d, s.d, s.l);
  memcpy(&d[s.l], r, l - s.l);
  free(s.d);
  return (struct str) {d, l};
}
struct str str_concat_str (struct str s1, struct str s2) {
  size_t l = s1.l + s2.l;
  char *d = alloc(l);
  memcpy(d, s1.d, s1.l);
  memcpy(&d[s1.l], s2.d, s2.l);
  free(s1.d);
  free(s2.d);
  return (struct str) {d, l};
}
struct str str_copy (const struct str s) {
  char *d = alloc(s.l);
  memcpy(d, s.d, s.l);
  return (struct str) {d, s.l};
}
char *str_cstr (const struct str s) {
  char *d = alloc(s.l + 1);
  memcpy(d, s.d, s.l);
  d[s.l] = '\0';
  return d;
}
bool str_empty (struct str s) {
  bool r = s.l == 0;
  free(s.d);
  return r;
}
void str_free (struct str s) {
  free(s.d);
}
size_t str_len (struct str s) {
  size_t l = s.l;
  free(s.d);
  return l;
}
bool str_ne_str (struct str s1, struct str s2) {
  bool r = s1.l != s2.l || memcmp(s1.d, s2.d, s1.l) != 0;
  free(s1.d);
  free(s2.d);
  return r;
}
struct str str_realloc (struct str s1, struct str s2) {
  free(s1.d);
  return s2;
}
struct str str_slice (struct str s, unsigned char o1, int32_t n1, unsigned char o2, int32_t n2) {
  int32_t i1 = o1 == 0 ? 0 : (int32_t) (n1 < 0 ? (n1 < -((int32_t) s.l) ? 0 : n1 + s.l) : (n1 > s.l ? s.l : n1));
  int32_t i2 = o2 == 0 ? (int32_t) s.l : (int32_t) (n2 < 0 ? (n2 < -((int32_t) s.l) ? 0 : n2 + s.l) : (n2 > s.l ? s.l : n2));
  if (i1 >= i2 || i1 >= s.l) {
    free(s.d);
    return str_alloc("");
  }
  size_t l = i2 - i1;
  char *d = alloc(l);
  for (size_t i = 0; i1 < i2; i1++) d[i++] = s.d[i1];
  free(s.d);
  return (struct str) {d, l};
}
struct str str_trim (struct str s) {
  if (s.l == 0) return s;
  size_t i = 0;
  size_t j = s.l;
  while (i < s.l && isspace(s.d[i])) i++;
  while (j >= 0 && isspace(s.d[j - 1])) {
    j--;
    if (j == 0) break;
  }
  if (i >= j) {
    free(s.d);
    return str_alloc("");
  }
  size_t l = j - i;
  char *r = alloc(l);
  for (size_t k = 0; k < l;) r[k++] = s.d[i++];
  free(s.d);
  return (struct str) {r, l};
}
struct str str_trimStart (struct str s) {
  if (s.l == 0) return s;
  size_t i = 0;
  while (i < s.l && isspace(s.d[i])) i++;
  if (i >= s.l) {
    free(s.d);
    s = (struct str) {NULL, 0};
  } else {
    memmove(s.d, &s.d[i], s.l - i);
    s.l -= i;
    s.d = re_alloc(s.d, s.l);
  }
  return s;
}
struct __THE_1_YAMLToken_0 *__THE_1_YAMLToken_0_alloc (enum __THE_1_YAMLTokenType_0 __THE_0_t, struct str __THE_0_val) {
  struct __THE_1_YAMLToken_0 *r = alloc(sizeof(struct __THE_1_YAMLToken_0));
  struct __THE_1_YAMLToken_0 s = {__THE_0_t, __THE_0_val};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLToken_0));
  return r;
}
struct __THE_1_YAMLToken_0 *__THE_1_YAMLToken_0_copy (const struct __THE_1_YAMLToken_0 *n) {
  struct __THE_1_YAMLToken_0 *r = alloc(sizeof(struct __THE_1_YAMLToken_0));
  struct __THE_1_YAMLToken_0 s = {n->__THE_0_t, str_copy(n->__THE_0_val)};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLToken_0));
  return r;
}
void __THE_1_YAMLToken_0_free (struct __THE_1_YAMLToken_0 *n) {
  str_free((struct str) n->__THE_0_val);
  free(n);
}
struct __THE_1_array_YAMLNodeMapping __THE_1_array_YAMLNodeMapping_alloc (size_t x, ...) {
  if (x == 0) return (struct __THE_1_array_YAMLNodeMapping) {NULL, 0};
  struct __THE_1_YAMLNodeMapping_0 **d = alloc(x * sizeof(struct __THE_1_YAMLNodeMapping_0 *));
  va_list args;
  va_start(args, x);
  for (size_t i = 0; i < x; i++) d[i] = va_arg(args, struct __THE_1_YAMLNodeMapping_0 *);
  va_end(args);
  return (struct __THE_1_array_YAMLNodeMapping) {d, x};
}
struct __THE_1_YAMLNodeMapping_0 **__THE_1_array_YAMLNodeMapping_at (struct __THE_1_array_YAMLNodeMapping n, int32_t i) {
  if ((i >= 0 && i >= n.l) || (i < 0 && i < -((int32_t) n.l))) {
    fprintf(stderr, "Error: index %" PRId32 " out of array bounds" THE_EOL, i);
    exit(EXIT_FAILURE);
  }
  return i < 0 ? &n.d[n.l + i] : &n.d[i];
}
struct __THE_1_array_YAMLNodeMapping __THE_1_array_YAMLNodeMapping_copy (const struct __THE_1_array_YAMLNodeMapping n) {
  if (n.l == 0) return (struct __THE_1_array_YAMLNodeMapping) {NULL, 0};
  struct __THE_1_YAMLNodeMapping_0 **d = alloc(n.l * sizeof(struct __THE_1_YAMLNodeMapping_0 *));
  for (size_t i = 0; i < n.l; i++) d[i] = __THE_1_YAMLNodeMapping_0_copy(n.d[i]);
  return (struct __THE_1_array_YAMLNodeMapping) {d, n.l};
}
void __THE_1_array_YAMLNodeMapping_free (struct __THE_1_array_YAMLNodeMapping n) {
  for (size_t i = 0; i < n.l; i++) __THE_1_YAMLNodeMapping_0_free((struct __THE_1_YAMLNodeMapping_0 *) n.d[i]);
  free(n.d);
}
size_t __THE_1_array_YAMLNodeMapping_len (struct __THE_1_array_YAMLNodeMapping n) {
  size_t l = n.l;
  __THE_1_array_YAMLNodeMapping_free((struct __THE_1_array_YAMLNodeMapping) n);
  return l;
}
bool __THE_1_array_YAMLNodeMapping_ne (struct __THE_1_array_YAMLNodeMapping n1, struct __THE_1_array_YAMLNodeMapping n2) {
  bool r = n1.l != n2.l;
  if (!r) {
    for (size_t i = 0; i < n1.l; i++) {
      if (__THE_1_YAMLNodeMapping_0_ne(__THE_1_YAMLNodeMapping_0_copy(n1.d[i]), __THE_1_YAMLNodeMapping_0_copy(n2.d[i]))) {
        r = true;
        break;
      }
    }
  }
  __THE_1_array_YAMLNodeMapping_free((struct __THE_1_array_YAMLNodeMapping) n1);
  __THE_1_array_YAMLNodeMapping_free((struct __THE_1_array_YAMLNodeMapping) n2);
  return r;
}
void __THE_1_array_YAMLNodeMapping_push (struct __THE_1_array_YAMLNodeMapping *n, struct __THE_1_array_YAMLNodeMapping m) {
  if (m.l == 0) return;
  n->l += m.l;
  n->d = re_alloc(n->d, n->l * sizeof(struct __THE_1_YAMLNodeMapping_0 *));
  size_t k = 0;
  for (size_t i = n->l - m.l; i < n->l; i++) n->d[i] = m.d[k++];
  free(m.d);
}
struct __THE_1_YAMLNodeMap_0 *__THE_1_YAMLNodeMap_0_alloc (struct __THE_1_array_YAMLNodeMapping __THE_0_mappings) {
  struct __THE_1_YAMLNodeMap_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeMap_0));
  struct __THE_1_YAMLNodeMap_0 s = {__THE_0_mappings};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeMap_0));
  return r;
}
struct __THE_1_YAMLNodeMap_0 *__THE_1_YAMLNodeMap_0_copy (const struct __THE_1_YAMLNodeMap_0 *n) {
  struct __THE_1_YAMLNodeMap_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeMap_0));
  struct __THE_1_YAMLNodeMap_0 s = {__THE_1_array_YAMLNodeMapping_copy(n->__THE_0_mappings)};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeMap_0));
  return r;
}
void __THE_1_YAMLNodeMap_0_free (struct __THE_1_YAMLNodeMap_0 *n) {
  __THE_1_array_YAMLNodeMapping_free((struct __THE_1_array_YAMLNodeMapping) n->__THE_0_mappings);
  free(n);
}
bool __THE_1_YAMLNodeMap_0_ne (struct __THE_1_YAMLNodeMap_0 *n1, struct __THE_1_YAMLNodeMap_0 *n2) {
  bool r = __THE_1_array_YAMLNodeMapping_ne(__THE_1_array_YAMLNodeMapping_copy(n1->__THE_0_mappings), __THE_1_array_YAMLNodeMapping_copy(n2->__THE_0_mappings));
  __THE_1_YAMLNodeMap_0_free((struct __THE_1_YAMLNodeMap_0 *) n1);
  __THE_1_YAMLNodeMap_0_free((struct __THE_1_YAMLNodeMap_0 *) n2);
  return r;
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc (int t, ...) {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE r = {t};
  va_list args;
  va_start(args, t);
  if (t == TYPE_YAMLNodeMap_0) r.v1 = va_arg(args, struct __THE_1_YAMLNodeMap_0 *);
  if (t == TYPE_YAMLNodeMapping_0) r.v2 = va_arg(args, struct __THE_1_YAMLNodeMapping_0 *);
  if (t == TYPE_YAMLNodeScalar_0) r.v3 = va_arg(args, struct __THE_1_YAMLNodeScalar_0 *);
  if (t == TYPE_YAMLNodeSeq_0) r.v4 = va_arg(args, struct __THE_1_YAMLNodeSeq_0 *);
  va_end(args);
  return r;
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy (const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE r = {n.t};
  if (n.t == TYPE_YAMLNodeMap_0) r.v1 = __THE_1_YAMLNodeMap_0_copy(n.v1);
  if (n.t == TYPE_YAMLNodeMapping_0) r.v2 = __THE_1_YAMLNodeMapping_0_copy(n.v2);
  if (n.t == TYPE_YAMLNodeScalar_0) r.v3 = __THE_1_YAMLNodeScalar_0_copy(n.v3);
  if (n.t == TYPE_YAMLNodeSeq_0) r.v4 = __THE_1_YAMLNodeSeq_0_copy(n.v4);
  return r;
}
void __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  if (n.t == TYPE_YAMLNodeMap_0) __THE_1_YAMLNodeMap_0_free((struct __THE_1_YAMLNodeMap_0 *) n.v1);
  if (n.t == TYPE_YAMLNodeMapping_0) __THE_1_YAMLNodeMapping_0_free((struct __THE_1_YAMLNodeMapping_0 *) n.v2);
  if (n.t == TYPE_YAMLNodeScalar_0) __THE_1_YAMLNodeScalar_0_free((struct __THE_1_YAMLNodeScalar_0 *) n.v3);
  if (n.t == TYPE_YAMLNodeSeq_0) __THE_1_YAMLNodeSeq_0_free((struct __THE_1_YAMLNodeSeq_0 *) n.v4);
}
bool __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n1, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n2) {
  bool r = n1.t != n2.t;
  if (!r) {
    if (n1.t == TYPE_YAMLNodeMap_0) r = __THE_1_YAMLNodeMap_0_ne(__THE_1_YAMLNodeMap_0_copy(n1.v1), __THE_1_YAMLNodeMap_0_copy(n2.v1));
    if (n1.t == TYPE_YAMLNodeMapping_0) r = __THE_1_YAMLNodeMapping_0_ne(__THE_1_YAMLNodeMapping_0_copy(n1.v2), __THE_1_YAMLNodeMapping_0_copy(n2.v2));
    if (n1.t == TYPE_YAMLNodeScalar_0) r = __THE_1_YAMLNodeScalar_0_ne(__THE_1_YAMLNodeScalar_0_copy(n1.v3), __THE_1_YAMLNodeScalar_0_copy(n2.v3));
    if (n1.t == TYPE_YAMLNodeSeq_0) r = __THE_1_YAMLNodeSeq_0_ne(__THE_1_YAMLNodeSeq_0_copy(n1.v4), __THE_1_YAMLNodeSeq_0_copy(n2.v4));
  }
  __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n1);
  __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n2);
  return r;
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *r = alloc(sizeof(struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE));
  *r = n;
  return r;
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy (const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *n) {
  if (n == NULL) return NULL;
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *r = alloc(sizeof(struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE));
  *r = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*n);
  return r;
}
void __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *n) {
  if (n == NULL) return;
  __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) *n);
  free(n);
}
bool __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *n1, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *n2) {
  bool r = (n1 == NULL || n2 == NULL) ? n1 != NULL || n2 != NULL : __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*n1), __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*n2));
  __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *) n1);
  __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *) n2);
  return r;
}
struct __THE_1_YAMLNodeMapping_0 *__THE_1_YAMLNodeMapping_0_alloc (struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_key, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_0_value) {
  struct __THE_1_YAMLNodeMapping_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeMapping_0));
  struct __THE_1_YAMLNodeMapping_0 s = {__THE_0_key, __THE_0_value};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeMapping_0));
  return r;
}
struct __THE_1_YAMLNodeMapping_0 *__THE_1_YAMLNodeMapping_0_copy (const struct __THE_1_YAMLNodeMapping_0 *n) {
  struct __THE_1_YAMLNodeMapping_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeMapping_0));
  struct __THE_1_YAMLNodeMapping_0 s = {__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n->__THE_0_key), __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n->__THE_0_value)};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeMapping_0));
  return r;
}
void __THE_1_YAMLNodeMapping_0_free (struct __THE_1_YAMLNodeMapping_0 *n) {
  __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n->__THE_0_key);
  __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *) n->__THE_0_value);
  free(n);
}
bool __THE_1_YAMLNodeMapping_0_ne (struct __THE_1_YAMLNodeMapping_0 *n1, struct __THE_1_YAMLNodeMapping_0 *n2) {
  bool r = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n1->__THE_0_key), __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n2->__THE_0_key)) || __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n1->__THE_0_value), __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n2->__THE_0_value));
  __THE_1_YAMLNodeMapping_0_free((struct __THE_1_YAMLNodeMapping_0 *) n1);
  __THE_1_YAMLNodeMapping_0_free((struct __THE_1_YAMLNodeMapping_0 *) n2);
  return r;
}
struct __THE_1_YAMLNodeScalar_0 *__THE_1_YAMLNodeScalar_0_alloc (struct str __THE_0_value) {
  struct __THE_1_YAMLNodeScalar_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeScalar_0));
  struct __THE_1_YAMLNodeScalar_0 s = {__THE_0_value};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeScalar_0));
  return r;
}
struct __THE_1_YAMLNodeScalar_0 *__THE_1_YAMLNodeScalar_0_copy (const struct __THE_1_YAMLNodeScalar_0 *n) {
  struct __THE_1_YAMLNodeScalar_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeScalar_0));
  struct __THE_1_YAMLNodeScalar_0 s = {str_copy(n->__THE_0_value)};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeScalar_0));
  return r;
}
void __THE_1_YAMLNodeScalar_0_free (struct __THE_1_YAMLNodeScalar_0 *n) {
  str_free((struct str) n->__THE_0_value);
  free(n);
}
bool __THE_1_YAMLNodeScalar_0_ne (struct __THE_1_YAMLNodeScalar_0 *n1, struct __THE_1_YAMLNodeScalar_0 *n2) {
  bool r = str_ne_str(str_copy(n1->__THE_0_value), str_copy(n2->__THE_0_value));
  __THE_1_YAMLNodeScalar_0_free((struct __THE_1_YAMLNodeScalar_0 *) n1);
  __THE_1_YAMLNodeScalar_0_free((struct __THE_1_YAMLNodeScalar_0 *) n2);
  return r;
}
struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc (size_t x, ...) {
  if (x == 0) return (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) {NULL, 0};
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *d = alloc(x * sizeof(struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE));
  va_list args;
  va_start(args, x);
  for (size_t i = 0; i < x; i++) d[i] = va_arg(args, struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE);
  va_end(args);
  return (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) {d, x};
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_at (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n, int32_t i) {
  if ((i >= 0 && i >= n.l) || (i < 0 && i < -((int32_t) n.l))) {
    fprintf(stderr, "Error: index %" PRId32 " out of array bounds" THE_EOL, i);
    exit(EXIT_FAILURE);
  }
  return i < 0 ? &n.d[n.l + i] : &n.d[i];
}
struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy (const struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  if (n.l == 0) return (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) {NULL, 0};
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *d = alloc(n.l * sizeof(struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE));
  for (size_t i = 0; i < n.l; i++) d[i] = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n.d[i]);
  return (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) {d, n.l};
}
bool __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_empty (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  bool r = n.l == 0;
  __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n);
  return r;
}
void __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  for (size_t i = 0; i < n.l; i++) __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n.d[i]);
  free(n.d);
}
size_t __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_len (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n) {
  size_t l = n.l;
  __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n);
  return l;
}
bool __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n1, struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE n2) {
  bool r = n1.l != n2.l;
  if (!r) {
    for (size_t i = 0; i < n1.l; i++) {
      if (__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n1.d[i]), __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n2.d[i]))) {
        r = true;
        break;
      }
    }
  }
  __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n1);
  __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n2);
  return r;
}
void __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_push (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *n, struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE m) {
  if (m.l == 0) return;
  n->l += m.l;
  n->d = re_alloc(n->d, n->l * sizeof(struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE));
  size_t k = 0;
  for (size_t i = n->l - m.l; i < n->l; i++) n->d[i] = m.d[k++];
  free(m.d);
}
struct __THE_1_YAMLNodeSeq_0 *__THE_1_YAMLNodeSeq_0_alloc (struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_items) {
  struct __THE_1_YAMLNodeSeq_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeSeq_0));
  struct __THE_1_YAMLNodeSeq_0 s = {__THE_0_items};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeSeq_0));
  return r;
}
struct __THE_1_YAMLNodeSeq_0 *__THE_1_YAMLNodeSeq_0_copy (const struct __THE_1_YAMLNodeSeq_0 *n) {
  struct __THE_1_YAMLNodeSeq_0 *r = alloc(sizeof(struct __THE_1_YAMLNodeSeq_0));
  struct __THE_1_YAMLNodeSeq_0 s = {__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n->__THE_0_items)};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLNodeSeq_0));
  return r;
}
void __THE_1_YAMLNodeSeq_0_free (struct __THE_1_YAMLNodeSeq_0 *n) {
  __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n->__THE_0_items);
  free(n);
}
bool __THE_1_YAMLNodeSeq_0_ne (struct __THE_1_YAMLNodeSeq_0 *n1, struct __THE_1_YAMLNodeSeq_0 *n2) {
  bool r = __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n1->__THE_0_items), __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n2->__THE_0_items));
  __THE_1_YAMLNodeSeq_0_free((struct __THE_1_YAMLNodeSeq_0 *) n1);
  __THE_1_YAMLNodeSeq_0_free((struct __THE_1_YAMLNodeSeq_0 *) n2);
  return r;
}
struct __THE_1_YAMLState_0 *__THE_1_YAMLState_0_alloc (int32_t __THE_0_pos) {
  struct __THE_1_YAMLState_0 *r = alloc(sizeof(struct __THE_1_YAMLState_0));
  struct __THE_1_YAMLState_0 s = {__THE_0_pos};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLState_0));
  return r;
}
struct __THE_1_YAMLState_0 *__THE_1_YAMLState_0_copy (const struct __THE_1_YAMLState_0 *n) {
  struct __THE_1_YAMLState_0 *r = alloc(sizeof(struct __THE_1_YAMLState_0));
  struct __THE_1_YAMLState_0 s = {n->__THE_0_pos};
  memcpy(r, &s, sizeof(struct __THE_1_YAMLState_0));
  return r;
}
void __THE_1_YAMLState_0_free (struct __THE_1_YAMLState_0 *n) {
  free(n);
}
struct __THE_1_YAMLState_0 *__THE_1_YAMLState_0_realloc (struct __THE_1_YAMLState_0 *n1, struct __THE_1_YAMLState_0 *n2) {
  __THE_1_YAMLState_0_free((struct __THE_1_YAMLState_0 *) n1);
  return n2;
}
struct __THE_1_array_YAMLToken __THE_1_array_YAMLToken_alloc (size_t x, ...) {
  if (x == 0) return (struct __THE_1_array_YAMLToken) {NULL, 0};
  struct __THE_1_YAMLToken_0 **d = alloc(x * sizeof(struct __THE_1_YAMLToken_0 *));
  va_list args;
  va_start(args, x);
  for (size_t i = 0; i < x; i++) d[i] = va_arg(args, struct __THE_1_YAMLToken_0 *);
  va_end(args);
  return (struct __THE_1_array_YAMLToken) {d, x};
}
struct __THE_1_YAMLToken_0 **__THE_1_array_YAMLToken_at (struct __THE_1_array_YAMLToken n, int32_t i) {
  if ((i >= 0 && i >= n.l) || (i < 0 && i < -((int32_t) n.l))) {
    fprintf(stderr, "Error: index %" PRId32 " out of array bounds" THE_EOL, i);
    exit(EXIT_FAILURE);
  }
  return i < 0 ? &n.d[n.l + i] : &n.d[i];
}
struct __THE_1_array_YAMLToken __THE_1_array_YAMLToken_copy (const struct __THE_1_array_YAMLToken n) {
  if (n.l == 0) return (struct __THE_1_array_YAMLToken) {NULL, 0};
  struct __THE_1_YAMLToken_0 **d = alloc(n.l * sizeof(struct __THE_1_YAMLToken_0 *));
  for (size_t i = 0; i < n.l; i++) d[i] = __THE_1_YAMLToken_0_copy(n.d[i]);
  return (struct __THE_1_array_YAMLToken) {d, n.l};
}
void __THE_1_array_YAMLToken_free (struct __THE_1_array_YAMLToken n) {
  for (size_t i = 0; i < n.l; i++) __THE_1_YAMLToken_0_free((struct __THE_1_YAMLToken_0 *) n.d[i]);
  free(n.d);
}
size_t __THE_1_array_YAMLToken_len (struct __THE_1_array_YAMLToken n) {
  size_t l = n.l;
  __THE_1_array_YAMLToken_free((struct __THE_1_array_YAMLToken) n);
  return l;
}
void __THE_1_array_YAMLToken_push (struct __THE_1_array_YAMLToken *n, struct __THE_1_array_YAMLToken m) {
  if (m.l == 0) return;
  n->l += m.l;
  n->d = re_alloc(n->d, n->l * sizeof(struct __THE_1_YAMLToken_0 *));
  size_t k = 0;
  for (size_t i = n->l - m.l; i < n->l; i++) n->d[i] = m.d[k++];
  free(m.d);
}
struct __THE_1_YAML_0 *__THE_1_YAML_0_alloc (struct str __THE_0_content, struct __THE_1_array_YAMLToken __THE_0_tokens, struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_nodes, struct __THE_1_YAMLState_0 *__THE_0_state) {
  struct __THE_1_YAML_0 *r = alloc(sizeof(struct __THE_1_YAML_0));
  struct __THE_1_YAML_0 s = {__THE_0_content, __THE_0_tokens, __THE_0_nodes, __THE_0_state};
  memcpy(r, &s, sizeof(struct __THE_1_YAML_0));
  return r;
}
struct __THE_1_YAML_0 *__THE_1_YAML_0_copy (const struct __THE_1_YAML_0 *n) {
  struct __THE_1_YAML_0 *r = alloc(sizeof(struct __THE_1_YAML_0));
  struct __THE_1_YAML_0 s = {str_copy(n->__THE_0_content), __THE_1_array_YAMLToken_copy(n->__THE_0_tokens), __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(n->__THE_0_nodes), __THE_1_YAMLState_0_copy(n->__THE_0_state)};
  memcpy(r, &s, sizeof(struct __THE_1_YAML_0));
  return r;
}
void __THE_1_YAML_0_free (struct __THE_1_YAML_0 *n) {
  str_free((struct str) n->__THE_0_content);
  __THE_1_array_YAMLToken_free((struct __THE_1_array_YAMLToken) n->__THE_0_tokens);
  __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) n->__THE_0_nodes);
  __THE_1_YAMLState_0_free((struct __THE_1_YAMLState_0 *) n->__THE_0_state);
  free(n);
}
void __THE_1_fn$1017_free (struct __THE_1_fn$1017 n) {
  if (n.x != NULL) free(n.x);
}
void __THE_1_fn$1015_free (struct __THE_1_fn$1015 n) {
  if (n.x != NULL) free(n.x);
}
struct __THE_1_YAML_0 *__THE_1_YAML_parse_0 (void *px, struct __THE_1_fn$1017P p) {
  struct __THE_1_YAML_0 *v;
  struct __THE_1_YAML_parse_0X *x = px;
  const struct __THE_1_fn$1015 *__THE_0_YAMLSD_parse_0 = x->__THE_0_YAMLSD_parse_0;
  const struct __THE_1_fn$1011 *__THE_0_YAMLSD_tokenize_0 = x->__THE_0_YAMLSD_tokenize_0;
  const struct __THE_1_fn$1009 *__THE_0_YAMLSD_hasNextTk_0 = x->__THE_0_YAMLSD_hasNextTk_0;
  const struct __THE_1_fn$1011 *__THE_0_YAMLSDreset_0 = x->__THE_0_YAMLSDreset_0;
  const struct str __THE_0_path_0 = p.n0;
  struct __THE_1_YAML_0 *__THE_0_result_0 = __THE_1_YAML_0_alloc(buffer_to_str(fs_readFileSync(str_copy(__THE_0_path_0))), __THE_1_array_YAMLToken_alloc(0), __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(0), __THE_1_YAMLState_0_alloc(0));
  (*__THE_0_YAMLSD_tokenize_0).f((*__THE_0_YAMLSD_tokenize_0).x, &__THE_0_result_0);
  while ((*__THE_0_YAMLSD_hasNextTk_0).f((*__THE_0_YAMLSD_hasNextTk_0).x, &__THE_0_result_0)) {
    const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_0_node_0 = (*__THE_0_YAMLSD_parse_0).f((*__THE_0_YAMLSD_parse_0).x, &__THE_0_result_0, (struct __THE_1_fn$1015P) {0, 0});
    if (__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_node_0), NULL)) {
      __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_push(&__THE_0_result_0->__THE_0_nodes, __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(1, __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*__THE_0_node_0)));
    }
    __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *) __THE_0_node_0);
  }
  (*__THE_0_YAMLSDreset_0).f((*__THE_0_YAMLSDreset_0).x, &__THE_0_result_0);
  v = __THE_1_YAML_0_copy(__THE_0_result_0);
  __THE_1_YAML_0_free((struct __THE_1_YAML_0 *) __THE_0_result_0);
  str_free((struct str) __THE_0_path_0);
  return v;
}
void __THE_1_YAML_parse_0_alloc (struct __THE_1_fn$1017 *n, struct __THE_1_YAML_parse_0X x) {
  size_t l = sizeof(struct __THE_1_YAML_parse_0X);
  struct __THE_1_YAML_parse_0X *r = alloc(l);
  memcpy(r, &x, l);
  n->f = &__THE_1_YAML_parse_0;
  n->x = r;
  n->l = l;
}
void __THE_1_fn$1018_free (struct __THE_1_fn$1018 n) {
  if (n.x != NULL) free(n.x);
}
struct str __THE_1_YAMLNode_stringify_0 (void *px, struct __THE_1_fn$1018P p) {
  struct str v;
  struct __THE_1_YAMLNode_stringify_0X *x = px;
  const struct __THE_1_fn$1018 *__THE_0_YAMLNode_stringify_0 = x->__THE_0_YAMLNode_stringify_0;
  const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_node_0 = p.n0;
  const int32_t __THE_0_indent_0 = p.o1 == 1 ? p.n1 : 0;
  struct str __THE_0_result_0 = str_alloc("");
  if (__THE_0_node_0.t == TYPE_YAMLNodeMap_0) {
    for (int32_t __THE_0_i_0 = 0; __THE_0_i_0 < __THE_1_array_YAMLNodeMapping_len(__THE_1_array_YAMLNodeMapping_copy(__THE_0_node_0.v1->__THE_0_mappings)); __THE_0_i_0++) {
      const struct __THE_1_YAMLNodeMapping_0 *__THE_0_n1_0 = __THE_1_YAMLNodeMapping_0_copy(*__THE_1_array_YAMLNodeMapping_at(__THE_0_node_0.v1->__THE_0_mappings, __THE_0_i_0));
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeMapping_0, __THE_1_YAMLNodeMapping_0_copy(__THE_0_n1_0));
      __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), (*__THE_0_YAMLNode_stringify_0).f((*__THE_0_YAMLNode_stringify_0).x, (struct __THE_1_fn$1018P) {__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n_0), 1, __THE_0_indent_0 + 2})));
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n_0);
      __THE_1_YAMLNodeMapping_0_free((struct __THE_1_YAMLNodeMapping_0 *) __THE_0_n1_0);
    }
  }
  if (__THE_0_node_0.t == TYPE_YAMLNodeMapping_0) {
    __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_concat_cstr(str_concat_str(char_repeat(' ', __THE_0_indent_0), (*__THE_0_YAMLNode_stringify_0).f((*__THE_0_YAMLNode_stringify_0).x, (struct __THE_1_fn$1018P) {__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_node_0.v2->__THE_0_key), 0, 0})), ":")));
    if (__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_node_0.v2->__THE_0_value), NULL)) {
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_val_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*__THE_0_node_0.v2->__THE_0_value);
      const bool __THE_0_isValComplex_0 = (__THE_0_val_0.t == TYPE_YAMLNodeMap_0) || (__THE_0_val_0.t == TYPE_YAMLNodeSeq_0);
      __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_concat_str((__THE_0_isValComplex_0 ? str_alloc(THE_EOL) : str_alloc(" ")), (*__THE_0_YAMLNode_stringify_0).f((*__THE_0_YAMLNode_stringify_0).x, (struct __THE_1_fn$1018P) {__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_val_0), 0, 0}))));
      if (!__THE_0_isValComplex_0) {
        __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_alloc(THE_EOL)));
      }
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_val_0);
    } else {
      __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_alloc(THE_EOL)));
    }
  }
  if (__THE_0_node_0.t == TYPE_YAMLNodeScalar_0) {
    __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_copy(__THE_0_node_0.v3->__THE_0_value)));
  }
  if (__THE_0_node_0.t == TYPE_YAMLNodeSeq_0) {
    for (int32_t __THE_0_i_0 = 0; __THE_0_i_0 < __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_len(__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_node_0.v4->__THE_0_items)); __THE_0_i_0++) {
      __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), char_repeat(' ', __THE_0_indent_0 + 2)));
      __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_trimStart((*__THE_0_YAMLNode_stringify_0).f((*__THE_0_YAMLNode_stringify_0).x, (struct __THE_1_fn$1018P) {__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_at(__THE_0_node_0.v4->__THE_0_items, __THE_0_i_0)), 1, __THE_0_indent_0 + 2}))));
    }
  }
  v = str_copy(__THE_0_result_0);
  str_free((struct str) __THE_0_result_0);
  __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_node_0);
  return v;
}
void __THE_1_YAMLNode_stringify_0_alloc (struct __THE_1_fn$1018 *n, struct __THE_1_YAMLNode_stringify_0X x) {
  size_t l = sizeof(struct __THE_1_YAMLNode_stringify_0X);
  struct __THE_1_YAMLNode_stringify_0X *r = alloc(l);
  memcpy(r, &x, l);
  n->f = &__THE_1_YAMLNode_stringify_0;
  n->x = r;
  n->l = l;
}
bool __THE_1_YAMLSDhasNext_0 (void *px, const struct __THE_1_YAML_0 **__THE_0_self_0) {
  return (*__THE_0_self_0)->__THE_0_state->__THE_0_pos < __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_len(__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy((*__THE_0_self_0)->__THE_0_nodes));
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_1_YAMLSDnext_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0) {
  return __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_at((*__THE_0_self_0)->__THE_0_nodes, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos++));
}
void __THE_1_YAMLSDreset_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0) {
  (*__THE_0_self_0)->__THE_0_state = __THE_1_YAMLState_0_realloc((*__THE_0_self_0)->__THE_0_state, __THE_1_YAMLState_0_alloc(0));
}
bool __THE_1_YAMLSD_hasNextTk_0 (void *px, const struct __THE_1_YAML_0 **__THE_0_self_0) {
  int32_t __THE_0_pos_0 = (*__THE_0_self_0)->__THE_0_state->__THE_0_pos;
  const int32_t __THE_0_tokensLen_0 = __THE_1_array_YAMLToken_len(__THE_1_array_YAMLToken_copy((*__THE_0_self_0)->__THE_0_tokens));
  while (__THE_0_pos_0 < __THE_0_tokensLen_0 && (*__THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, __THE_0_pos_0))->__THE_0_t == __THE_0_YAMLTokenTypeSDWhitespace_0) {
    __THE_0_pos_0++;
  }
  return __THE_0_pos_0 < __THE_0_tokensLen_0;
}
struct __THE_1_YAMLToken_0 *__THE_1_YAMLSD_nextTk_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0) {
  const int32_t __THE_0_tokensLen_0 = __THE_1_array_YAMLToken_len(__THE_1_array_YAMLToken_copy((*__THE_0_self_0)->__THE_0_tokens));
  while ((*__THE_0_self_0)->__THE_0_state->__THE_0_pos < __THE_0_tokensLen_0 && (*__THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos))->__THE_0_t == __THE_0_YAMLTokenTypeSDWhitespace_0) {
    (*__THE_0_self_0)->__THE_0_state->__THE_0_pos++;
  }
  return __THE_1_YAMLToken_0_copy(*__THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos++));
}
void __THE_1_fn$1013_free (struct __THE_1_fn$1013 n) {
  if (n.x != NULL) free(n.x);
}
void __THE_1_fn$1014_free (struct __THE_1_fn$1014 n) {
  if (n.x != NULL) free(n.x);
}
int32_t __THE_1_YAMLSD_nextTkIndent_0 (void *px, const struct __THE_1_YAML_0 **__THE_0_self_0) {
  struct __THE_1_YAMLSD_nextTkIndent_0X *x = px;
  const struct __THE_1_fn$1014 *__THE_0_YAMLSD_nextTkIs_0 = x->__THE_0_YAMLSD_nextTkIs_0;
  const struct __THE_1_fn$1009 *__THE_0_YAMLSD_hasNextTk_0 = x->__THE_0_YAMLSD_hasNextTk_0;
  while ((*__THE_0_YAMLSD_nextTkIs_0).f((*__THE_0_YAMLSD_nextTkIs_0).x, __THE_0_self_0, (struct __THE_1_fn$1014P) {__THE_0_YAMLTokenTypeSDEOL_0})) {
  }
  int32_t __THE_0_indent_0 = 0;
  if (!(*__THE_0_YAMLSD_hasNextTk_0).f((*__THE_0_YAMLSD_hasNextTk_0).x, __THE_0_self_0)) {
    return __THE_0_indent_0;
  }
  const struct __THE_1_YAMLToken_0 **__THE_0_tk_0 = __THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos);
  if ((*__THE_0_tk_0)->__THE_0_t == __THE_0_YAMLTokenTypeSDWhitespace_0) {
    const int32_t __THE_0_tkValLen_0 = str_len(str_copy((*__THE_0_tk_0)->__THE_0_val));
    for (int32_t __THE_0_i_0 = 0; __THE_0_i_0 < __THE_0_tkValLen_0 && *str_at((*__THE_0_tk_0)->__THE_0_val, __THE_0_i_0) == ' '; __THE_0_i_0++) {
      __THE_0_indent_0 += 1;
    }
  }
  return __THE_0_indent_0;
}
void __THE_1_YAMLSD_nextTkIndent_0_alloc (struct __THE_1_fn$1013 *n, struct __THE_1_YAMLSD_nextTkIndent_0X x) {
  size_t l = sizeof(struct __THE_1_YAMLSD_nextTkIndent_0X);
  struct __THE_1_YAMLSD_nextTkIndent_0X *r = alloc(l);
  memcpy(r, &x, l);
  n->f = &__THE_1_YAMLSD_nextTkIndent_0;
  n->x = r;
  n->l = l;
}
bool __THE_1_YAMLSD_nextTkIs_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0, struct __THE_1_fn$1014P p) {
  unsigned char r = 0;
  bool v;
  struct __THE_1_YAMLSD_nextTkIs_0X *x = px;
  const struct __THE_1_fn$1012 *__THE_0_YAMLSD_nextTk_0 = x->__THE_0_YAMLSD_nextTk_0;
  const struct __THE_1_fn$1009 *__THE_0_YAMLSD_hasNextTk_0 = x->__THE_0_YAMLSD_hasNextTk_0;
  const enum __THE_1_YAMLTokenType_0 __THE_0_t_0 = p.n0;
  if (!(*__THE_0_YAMLSD_hasNextTk_0).f((*__THE_0_YAMLSD_hasNextTk_0).x, __THE_0_self_0)) {
    return false;
  }
  const int32_t __THE_0_pos_0 = (*__THE_0_self_0)->__THE_0_state->__THE_0_pos;
  const struct __THE_1_YAMLToken_0 *__THE_0_tk_0 = (*__THE_0_YAMLSD_nextTk_0).f((*__THE_0_YAMLSD_nextTk_0).x, __THE_0_self_0);
  if (__THE_0_tk_0->__THE_0_t == __THE_0_t_0) {
    r = 1;
    v = true;
    goto L5;
  }
  (*__THE_0_self_0)->__THE_0_state->__THE_0_pos = __THE_0_pos_0;
  v = false;
L5:
  __THE_1_YAMLToken_0_free((struct __THE_1_YAMLToken_0 *) __THE_0_tk_0);
  return v;
}
void __THE_1_YAMLSD_nextTkIs_0_alloc (struct __THE_1_fn$1014 *n, struct __THE_1_YAMLSD_nextTkIs_0X x) {
  size_t l = sizeof(struct __THE_1_YAMLSD_nextTkIs_0X);
  struct __THE_1_YAMLSD_nextTkIs_0X *r = alloc(l);
  memcpy(r, &x, l);
  n->f = &__THE_1_YAMLSD_nextTkIs_0;
  n->x = r;
  n->l = l;
}
struct __THE_1_array_char __THE_1_array_char_alloc (size_t x, ...) {
  if (x == 0) return (struct __THE_1_array_char) {NULL, 0};
  char *d = alloc(x * sizeof(char));
  va_list args;
  va_start(args, x);
  for (size_t i = 0; i < x; i++) d[i] = va_arg(args, int);
  va_end(args);
  return (struct __THE_1_array_char) {d, x};
}
bool __THE_1_array_char_contains (struct __THE_1_array_char self, char n1) {
  bool r = false;
  for (size_t i = 0; i < self.l; i++) {
    if (self.d[i] == n1) {
      r = true;
      break;
    }
  }
  __THE_1_array_char_free((struct __THE_1_array_char) self);
  return r;
}
struct __THE_1_array_char __THE_1_array_char_copy (const struct __THE_1_array_char n) {
  if (n.l == 0) return (struct __THE_1_array_char) {NULL, 0};
  char *d = alloc(n.l * sizeof(char));
  for (size_t i = 0; i < n.l; i++) d[i] = n.d[i];
  return (struct __THE_1_array_char) {d, n.l};
}
void __THE_1_array_char_free (struct __THE_1_array_char n) {
  free(n.d);
}
void __THE_1_YAMLSD_tokenize_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0) {
  const struct __THE_1_array_char __THE_0_textStopChars_0 = __THE_1_array_char_alloc(7, ':', '-', '[', ']', '{', '}', ',');
  const int32_t __THE_0_contentLen_0 = str_len(str_copy((*__THE_0_self_0)->__THE_0_content));
  for (int32_t __THE_0_i_0 = 0; __THE_0_i_0 < __THE_0_contentLen_0; __THE_0_i_0++) {
    const char *__THE_0_ch_0 = str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0);
    if (*__THE_0_ch_0 == ':') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDColon_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == '-') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDMinus_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == '[') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDLBrack_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == ']') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDRBrack_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == '{') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDLBrace_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == '}') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDRBrace_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == ',') {
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDComma_0, char_str(*__THE_0_ch_0))));
    } else if (*__THE_0_ch_0 == '#') {
      const int32_t __THE_0_startIdx_0 = __THE_0_i_0;
      if (*__THE_0_ch_0 == '\r' && __THE_0_i_0 + 1 < __THE_0_contentLen_0 && *str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0 + 1) == '\n') {
        __THE_0_i_0++;
      }
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDEOL_0, str_slice(str_copy((*__THE_0_self_0)->__THE_0_content), 1, __THE_0_startIdx_0, 1, __THE_0_i_0 + 1))));
    } else if (*__THE_0_ch_0 == '\r' || *__THE_0_ch_0 == '\n') {
      while (__THE_0_i_0 + 1 < __THE_0_contentLen_0 && *str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0 + 1) != '\n') {
        __THE_0_i_0++;
      }
    } else if (char_isWhitespace(*__THE_0_ch_0)) {
      const int32_t __THE_0_startIdx_0 = __THE_0_i_0;
      while (__THE_0_i_0 + 1 < __THE_0_contentLen_0 && char_isWhitespace(*str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0 + 1)) && *str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0 + 1) != '\r' && *str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0 + 1) != '\n') {
        __THE_0_i_0++;
      }
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDWhitespace_0, str_slice(str_copy((*__THE_0_self_0)->__THE_0_content), 1, __THE_0_startIdx_0, 1, __THE_0_i_0 + 1))));
    } else {
      const int32_t __THE_0_startIdx_0 = __THE_0_i_0;
      while (__THE_0_i_0 + 1 < __THE_0_contentLen_0) {
        const char *__THE_0_ch_1 = str_at((*__THE_0_self_0)->__THE_0_content, __THE_0_i_0 + 1);
        if (char_isWhitespace(*__THE_0_ch_1) || __THE_1_array_char_contains(__THE_1_array_char_copy(__THE_0_textStopChars_0), *__THE_0_ch_1)) {
          break;
        }
        __THE_0_i_0++;
      }
      __THE_1_array_YAMLToken_push(&(*__THE_0_self_0)->__THE_0_tokens, __THE_1_array_YAMLToken_alloc(1, __THE_1_YAMLToken_0_alloc(__THE_0_YAMLTokenTypeSDText_0, str_slice(str_copy((*__THE_0_self_0)->__THE_0_content), 1, __THE_0_startIdx_0, 1, __THE_0_i_0 + 1))));
    }
  }
  __THE_1_array_char_free((struct __THE_1_array_char) __THE_0_textStopChars_0);
}
struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_1_YAMLSD_parse_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0, struct __THE_1_fn$1015P p) {
  unsigned char r = 0;
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *v;
  struct __THE_1_YAMLSD_parse_0X *x = px;
  const struct __THE_1_fn$1016 *__THE_0_YAMLSD_stringifyTill_0 = x->__THE_0_YAMLSD_stringifyTill_0;
  const struct __THE_1_fn$1015 *__THE_0_YAMLSD_parse_0 = x->__THE_0_YAMLSD_parse_0;
  const struct __THE_1_fn$1014 *__THE_0_YAMLSD_nextTkIs_0 = x->__THE_0_YAMLSD_nextTkIs_0;
  const struct __THE_1_fn$1013 *__THE_0_YAMLSD_nextTkIndent_0 = x->__THE_0_YAMLSD_nextTkIndent_0;
  const struct __THE_1_fn$1012 *__THE_0_YAMLSD_nextTk_0 = x->__THE_0_YAMLSD_nextTk_0;
  const int32_t __THE_0_indent_0 = p.o0 == 1 ? p.n0 : 0;
  struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_0_node_0 = NULL;
  const struct __THE_1_YAMLToken_0 *__THE_0_tk_0 = (*__THE_0_YAMLSD_nextTk_0).f((*__THE_0_YAMLSD_nextTk_0).x, __THE_0_self_0);
  if (__THE_0_tk_0->__THE_0_t == __THE_0_YAMLTokenTypeSDMinus_0) {
    const struct str __THE_0_value_0 = (*__THE_0_YAMLSD_stringifyTill_0).f((*__THE_0_YAMLSD_stringifyTill_0).x, __THE_0_self_0, (struct __THE_1_fn$1016P) {__THE_0_YAMLTokenTypeSDEOL_0});
    const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeScalar_0, __THE_1_YAMLNodeScalar_0_alloc(str_trim(str_copy(__THE_0_value_0))));
    r = 1;
    v = __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n_0));
    __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n_0);
    str_free((struct str) __THE_0_value_0);
    if (r == 1) goto L7;
  }
  if (__THE_0_tk_0->__THE_0_t == __THE_0_YAMLTokenTypeSDText_0) {
    const struct str __THE_0_key_0 = str_concat_str(str_copy(__THE_0_tk_0->__THE_0_val), (*__THE_0_YAMLSD_stringifyTill_0).f((*__THE_0_YAMLSD_stringifyTill_0).x, __THE_0_self_0, (struct __THE_1_fn$1016P) {__THE_0_YAMLTokenTypeSDColon_0}));
    if (!(*__THE_0_YAMLSD_nextTkIs_0).f((*__THE_0_YAMLSD_nextTkIs_0).x, __THE_0_self_0, (struct __THE_1_fn$1014P) {__THE_0_YAMLTokenTypeSDColon_0})) {
      goto L9;
    }
    const struct str __THE_0_value_0 = (*__THE_0_YAMLSD_stringifyTill_0).f((*__THE_0_YAMLSD_stringifyTill_0).x, __THE_0_self_0, (struct __THE_1_fn$1016P) {__THE_0_YAMLTokenTypeSDEOL_0});
    const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_keyNode_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeScalar_0, __THE_1_YAMLNodeScalar_0_alloc(str_trim(str_copy(__THE_0_key_0))));
    if (!str_empty(str_copy(__THE_0_value_0))) {
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_valueNode_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeScalar_0, __THE_1_YAMLNodeScalar_0_alloc(str_trim(str_copy(__THE_0_value_0))));
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeMapping_0, __THE_1_YAMLNodeMapping_0_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_keyNode_0), __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_valueNode_0))));
      r = 1;
      v = __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n_0));
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n_0);
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_valueNode_0);
      if (r == 1) goto L10;
    }
    const int32_t __THE_0_nexTkIndent_0 = (*__THE_0_YAMLSD_nextTkIndent_0).f((*__THE_0_YAMLSD_nextTkIndent_0).x, __THE_0_self_0);
    struct __THE_1_array_YAMLNodeMapping __THE_0_mappings_0 = __THE_1_array_YAMLNodeMapping_alloc(0);
    struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_nodes_0 = __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(0);
    bool __THE_0_mappingsOnly_0 = true;
    if (__THE_0_nexTkIndent_0 > __THE_0_indent_0) {
      while ((*__THE_0_YAMLSD_nextTkIndent_0).f((*__THE_0_YAMLSD_nextTkIndent_0).x, __THE_0_self_0) >= __THE_0_nexTkIndent_0) {
        const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *__THE_0_childNode_0 = (*__THE_0_YAMLSD_parse_0).f((*__THE_0_YAMLSD_parse_0).x, __THE_0_self_0, (struct __THE_1_fn$1015P) {1, __THE_0_nexTkIndent_0});
        if (__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_ne(__THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_childNode_0), NULL)) {
          const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_actualChildNode_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(*__THE_0_childNode_0);
          __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_push(&__THE_0_nodes_0, __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(1, __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_actualChildNode_0)));
          if (__THE_0_actualChildNode_0.t == TYPE_YAMLNodeMapping_0) {
            __THE_1_array_YAMLNodeMapping_push(&__THE_0_mappings_0, __THE_1_array_YAMLNodeMapping_alloc(1, __THE_1_YAMLNodeMapping_0_copy(__THE_0_actualChildNode_0.v2)));
          } else {
            __THE_0_mappingsOnly_0 = false;
          }
          __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_actualChildNode_0);
        }
        __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *) __THE_0_childNode_0);
      }
    }
    if (__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_empty(__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_nodes_0))) {
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeMapping_0, __THE_1_YAMLNodeMapping_0_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_keyNode_0), NULL));
      r = 1;
      v = __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n_0));
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n_0);
      if (r == 1) goto L12;
    } else if (__THE_0_mappingsOnly_0) {
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n1_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeMap_0, __THE_1_YAMLNodeMap_0_alloc(__THE_1_array_YAMLNodeMapping_copy(__THE_0_mappings_0)));
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n2_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeMapping_0, __THE_1_YAMLNodeMapping_0_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_keyNode_0), __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n1_0))));
      r = 1;
      v = __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n2_0));
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n2_0);
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n1_0);
      if (r == 1) goto L12;
    } else {
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n1_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeSeq_0, __THE_1_YAMLNodeSeq_0_alloc(__THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_nodes_0)));
      const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_n2_0 = __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(TYPE_YAMLNodeMapping_0, __THE_1_YAMLNodeMapping_0_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_keyNode_0), __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n1_0))));
      r = 1;
      v = __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_alloc(__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_n2_0));
      goto L17;
L17:
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n2_0);
      __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_n1_0);
      if (r == 1) goto L12;
    }
L12:
    __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_array_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_nodes_0);
    __THE_1_array_YAMLNodeMapping_free((struct __THE_1_array_YAMLNodeMapping) __THE_0_mappings_0);
L10:
    __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_keyNode_0);
    str_free((struct str) __THE_0_value_0);
L9:
    str_free((struct str) __THE_0_key_0);
    if (r == 1) goto L7;
  }
  v = NULL;
L7:
  __THE_1_YAMLToken_0_free((struct __THE_1_YAMLToken_0 *) __THE_0_tk_0);
  __THE_1_opt_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE *) __THE_0_node_0);
  return v;
}
void __THE_1_YAMLSD_parse_0_alloc (struct __THE_1_fn$1015 *n, struct __THE_1_YAMLSD_parse_0X x) {
  size_t l = sizeof(struct __THE_1_YAMLSD_parse_0X);
  struct __THE_1_YAMLSD_parse_0X *r = alloc(l);
  memcpy(r, &x, l);
  n->f = &__THE_1_YAMLSD_parse_0;
  n->x = r;
  n->l = l;
}
struct str __THE_1_YAMLSD_stringifyTill_0 (void *px, struct __THE_1_YAML_0 **__THE_0_self_0, struct __THE_1_fn$1016P p) {
  struct str v;
  const enum __THE_1_YAMLTokenType_0 __THE_0_t_0 = p.n0;
  const int32_t __THE_0_tokensLen_0 = __THE_1_array_YAMLToken_len(__THE_1_array_YAMLToken_copy((*__THE_0_self_0)->__THE_0_tokens));
  struct str __THE_0_result_0 = str_alloc("");
  for (; (*__THE_0_self_0)->__THE_0_state->__THE_0_pos < __THE_0_tokensLen_0; (*__THE_0_self_0)->__THE_0_state->__THE_0_pos++) {
    if (((*__THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos))->__THE_0_t == __THE_0_t_0 || (*__THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos))->__THE_0_t == __THE_0_YAMLTokenTypeSDEOL_0)) {
      break;
    }
    const struct __THE_1_YAMLToken_0 **__THE_0_tk_0 = __THE_1_array_YAMLToken_at((*__THE_0_self_0)->__THE_0_tokens, (*__THE_0_self_0)->__THE_0_state->__THE_0_pos);
    __THE_0_result_0 = str_realloc(__THE_0_result_0, str_concat_str(str_copy(__THE_0_result_0), str_copy((*__THE_0_tk_0)->__THE_0_val)));
  }
  v = str_copy(__THE_0_result_0);
  str_free((struct str) __THE_0_result_0);
  return v;
}

int main () {
  const struct __THE_1_fn$1017 __THE_0_YAML_parse_0;
  const struct __THE_1_fn$1018 __THE_0_YAMLNode_stringify_0;
  const struct __THE_1_fn$1009 __THE_0_YAMLSDhasNext_0 = (struct __THE_1_fn$1009) {&__THE_1_YAMLSDhasNext_0, NULL, 0};
  const struct __THE_1_fn$1010 __THE_0_YAMLSDnext_0 = (struct __THE_1_fn$1010) {&__THE_1_YAMLSDnext_0, NULL, 0};
  const struct __THE_1_fn$1011 __THE_0_YAMLSDreset_0 = (struct __THE_1_fn$1011) {&__THE_1_YAMLSDreset_0, NULL, 0};
  const struct __THE_1_fn$1009 __THE_0_YAMLSD_hasNextTk_0 = (struct __THE_1_fn$1009) {&__THE_1_YAMLSD_hasNextTk_0, NULL, 0};
  const struct __THE_1_fn$1012 __THE_0_YAMLSD_nextTk_0 = (struct __THE_1_fn$1012) {&__THE_1_YAMLSD_nextTk_0, NULL, 0};
  const struct __THE_1_fn$1013 __THE_0_YAMLSD_nextTkIndent_0;
  const struct __THE_1_fn$1014 __THE_0_YAMLSD_nextTkIs_0;
  const struct __THE_1_fn$1011 __THE_0_YAMLSD_tokenize_0 = (struct __THE_1_fn$1011) {&__THE_1_YAMLSD_tokenize_0, NULL, 0};
  const struct __THE_1_fn$1015 __THE_0_YAMLSD_parse_0;
  const struct __THE_1_fn$1016 __THE_0_YAMLSD_stringifyTill_0 = (struct __THE_1_fn$1016) {&__THE_1_YAMLSD_stringifyTill_0, NULL, 0};
  __THE_1_YAMLSD_nextTkIndent_0_alloc((struct __THE_1_fn$1013 *) &__THE_0_YAMLSD_nextTkIndent_0, (struct __THE_1_YAMLSD_nextTkIndent_0X) {&__THE_0_YAMLSD_nextTkIs_0, &__THE_0_YAMLSD_hasNextTk_0});
  __THE_1_YAMLSD_nextTkIs_0_alloc((struct __THE_1_fn$1014 *) &__THE_0_YAMLSD_nextTkIs_0, (struct __THE_1_YAMLSD_nextTkIs_0X) {&__THE_0_YAMLSD_nextTk_0, &__THE_0_YAMLSD_hasNextTk_0});
  __THE_1_YAMLSD_parse_0_alloc((struct __THE_1_fn$1015 *) &__THE_0_YAMLSD_parse_0, (struct __THE_1_YAMLSD_parse_0X) {&__THE_0_YAMLSD_stringifyTill_0, &__THE_0_YAMLSD_parse_0, &__THE_0_YAMLSD_nextTkIs_0, &__THE_0_YAMLSD_nextTkIndent_0, &__THE_0_YAMLSD_nextTk_0});
  __THE_1_YAML_parse_0_alloc((struct __THE_1_fn$1017 *) &__THE_0_YAML_parse_0, (struct __THE_1_YAML_parse_0X) {&__THE_0_YAMLSD_parse_0, &__THE_0_YAMLSD_tokenize_0, &__THE_0_YAMLSD_hasNextTk_0, &__THE_0_YAMLSDreset_0});
  __THE_1_YAMLNode_stringify_0_alloc((struct __THE_1_fn$1018 *) &__THE_0_YAMLNode_stringify_0, (struct __THE_1_YAMLNode_stringify_0X) {&__THE_0_YAMLNode_stringify_0});
  const struct __THE_1_YAML_0 *__THE_0_yaml_0 = __THE_0_YAML_parse_0.f(__THE_0_YAML_parse_0.x, (struct __THE_1_fn$1017P) {str_alloc("yaml-test.txt")});
  struct str __THE_0_code_0 = str_alloc("");
  while (__THE_0_YAMLSDhasNext_0.f(__THE_0_YAMLSDhasNext_0.x, &__THE_0_yaml_0)) {
    const struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE __THE_0_node_0 = __THE_0_YAMLSDnext_0.f(__THE_0_YAMLSDnext_0.x, &__THE_0_yaml_0);
    __THE_0_code_0 = str_realloc(__THE_0_code_0, str_concat_str(str_copy(__THE_0_code_0), __THE_0_YAMLNode_stringify_0.f(__THE_0_YAMLNode_stringify_0.x, (struct __THE_1_fn$1018P) {__THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_copy(__THE_0_node_0), 0, 0})));
    __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE_free((struct __THE_1_union_YAMLNodeMapUSYAMLNodeMappingUSYAMLNodeScalarUSYAMLNodeSeqUE) __THE_0_node_0);
  }
  if (!str_empty(str_copy(__THE_0_code_0))) {
    print(stdout, "s", str_copy(__THE_0_code_0));
  }
  str_free((struct str) __THE_0_code_0);
  __THE_1_YAML_0_free((struct __THE_1_YAML_0 *) __THE_0_yaml_0);
  __THE_1_fn$1018_free((struct __THE_1_fn$1018) __THE_0_YAMLNode_stringify_0);
  __THE_1_fn$1017_free((struct __THE_1_fn$1017) __THE_0_YAML_parse_0);
  __THE_1_fn$1015_free((struct __THE_1_fn$1015) __THE_0_YAMLSD_parse_0);
  __THE_1_fn$1014_free((struct __THE_1_fn$1014) __THE_0_YAMLSD_nextTkIs_0);
  __THE_1_fn$1013_free((struct __THE_1_fn$1013) __THE_0_YAMLSD_nextTkIndent_0);
}

#pragma clang diagnostic pop
