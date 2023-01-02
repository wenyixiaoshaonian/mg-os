#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  // puts("printf Not implemented");
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  // puts("vsprintf Not implemented");
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  // puts("sprintf Not implemented");
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  // puts("snprintf Not implemented");
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  // puts("vsnprintf Not implemented");
  return 0;
}

#endif
