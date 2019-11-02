#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  assert(0);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  assert(0);
  return 0;
}

static size_t print_d(char* dst, int d) {
  assert(dst);

  int negative = (d < 0);
  size_t len = 0;
  size_t offset = 0;
  char buf[16]; // INT_MAX = 2,147,483,647‬, 10 digits

  if (d == 0) {
    dst[0] = '0';
    return 1;
  }

  while (d != 0) {
    buf[len++] = (d % 10) + '0';
    d /= 10; 
  }

  if (negative) {
    dst[0] = '-';
    offset = 1;
  } 
  for (size_t i = 0; i < len; i++) {
    dst[offset + i] = buf[len - i - 1];
  }

  return offset + len;
}

static size_t print_s(char* dst, const char* s) {
  assert(dst && s);
  size_t len = strlen(s);
  
  for (size_t i = 0; i < len; i++) {
    dst[i] = s[i];
  }

  return len;
}

int sprintf(char *out, const char *fmt, ...) {
  assert(out && fmt);

  va_list ap;
  va_start(ap, fmt);

  size_t idx = 0;
  size_t out_idx = 0;
  size_t len = 0;

  while (fmt[idx] != '\0') {
    if (fmt[idx] == '%') {
      char type = fmt[idx+1];
      if (type == 'd') {
        int v = va_arg(ap, int);
        len = print_d(out + out_idx, v);
        out_idx += len;
        idx += 2;
      } else if (type == 's') {
        const char* s = va_arg(ap, char*);
        len = print_s(out + out_idx, s);
        out_idx += len;
        idx += 2;
      } else if (type == '%') {
        out[out_idx] = '%';
        out_idx += 1;
        idx += 2;
      } else {
        // TODO
        assert(0);
      }
    } else {
      out[out_idx++] = fmt[idx++];
    }
  }
  out[out_idx] = '\0';
  return out_idx;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  assert(0);
  return 0;
}

#endif
