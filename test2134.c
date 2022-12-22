#include <stdio.h>

int main () {
  char *result;
  #if defined(WIN32)
    result = "WIN32";
  #elif defined(_WIN32)
    result = "_WIN32";
  #elif defined(__WIN32)
    result = "__WIN32";
  #elif defined(__WIN32__)
    result = "__WIN32__";
  #else
    result = "unknown";
  #endif

  printf("Macros: %s\n", result);
}
