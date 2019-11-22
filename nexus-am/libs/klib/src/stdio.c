#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

typedef void (*putc_func_t) (char*, char);

static void putc_stdio(char* dst, char ch) {
  _putc(ch);
}

static void putc_buf(char* dst, char ch) {
  assert(dst);
  *dst = ch;
}

static size_t print_d(putc_func_t f, char* dst, int d, char padding, int padding_width) {
  int negative = (d < 0);
  int len = 0;
  int offset = 0;
  char buf[16]; // INT_MAX = 2,147,483,647‬, 10 digits

  if (d == 0) {
    f(dst, '0');
    return 1;
  }

  while (d != 0) {
    buf[len++] = (d % 10) + '0';
    d /= 10; 
  }

  if (negative) {
    offset = padding_width - (len + 1) <= 0 ? 0 : padding_width - (len + 1);
  } else {
    offset = padding_width - len <= 0 ? 0 : padding_width - len;
  }
  for (int i = 0; i < offset; i++) {
    f(dst + i, padding);
  }

  if (negative) {
    f(dst + offset, '-');
    offset += 1;
  }
  for (size_t i = 0; i < len; i++) {
    f(dst + offset + i, buf[len - i - 1]);
  }
  offset += len;

  assert(offset > 0);
  return offset;
}

static size_t print_x(putc_func_t f, char* dst, unsigned int d, char padding, int padding_width, int lowercase) {
  int len = 0;
  int offset = 0;
  char buf[16]; // UINT_MAX = 0xFFFFFFFF, 8 chars

  if (d == 0) {
    f(dst, '0');
    return 1;
  }

  while (d != 0) {
    if (d % 16 < 10) {
      buf[len] = '0' + (d % 16);
    } else {
      if (lowercase) {
        buf[len] = 'a' + (d % 16 - 10);
      } else {
        buf[len] = 'A' + (d % 16 - 10);
      }
    }
    len++;
    d /= 16;
  }

  offset = padding_width - len <= 0 ? 0 : padding_width - len;
  for (int i = 0; i < offset; i++) {
    f(dst + i, padding);
  }

  for (size_t i = 0; i < len; i++) {
    f(dst + offset + i, buf[len - i - 1]);
  }
  offset += len;

  assert(offset > 0);
  return offset;
}

static size_t print_s(putc_func_t f, char* dst, const char* s, char padding, int padding_width) {
  assert(s);
  size_t len = strlen(s);
  size_t offset = 0;
  
  if (len < padding_width) {
    offset = padding_width - len;
    for (int i = 0; i < offset; i++) {
      f(dst + i, padding);
    }
  }

  for (size_t i = 0; i < len; i++) {
    f(dst + offset + i, s[i]);
  }

  return len + offset;
}

static size_t get_padding(const char* fmt, char* padding) {
  assert(fmt && padding);
  size_t idx = 0;
  if (fmt[idx] == '0') {
    *padding = '0';
    return 1;
  } else {
    *padding = ' ';
    return 0;
  }
}

static size_t get_padding_width(const char* fmt, int* width) {
  assert(fmt && width);
  int w = 0;
  size_t idx = 0;
  while (fmt[idx] >= '0' && fmt[idx] <= '9') {
    w = w * 10 + (fmt[idx] - '0');
    idx++;
  }

  *width = w;
  return idx;
}

int vsprintf_ex(putc_func_t f, char *out, const char *fmt, va_list ap) {
  size_t idx = 0;
  size_t out_idx = 0;
  size_t len = 0;

  while (fmt[idx] != '\0') {
    if (fmt[idx] == '%') {
      idx++;

      char padding = ' ';
      int padding_width = 0;
      len = get_padding(fmt + idx, &padding);
      idx += len;
      len = get_padding_width(fmt + idx, &padding_width);
      idx += len;

      char type = fmt[idx];
      if (type == 'd') {
        int v = va_arg(ap, int);
        len = print_d(f, out + out_idx, v, padding, padding_width);
        out_idx += len;
        idx++;
      } else if (type == 'x' || type == 'X') {
        unsigned int v = va_arg(ap, unsigned int);
        len = print_x(f, out + out_idx, v, padding, padding_width, type == 'x');
        out_idx += len;
        idx++;
      } else if (type == 'p') { 
        void* v = va_arg(ap, void*);
        len = print_x(f, out + out_idx, (unsigned long)v, '0', 8, 1);
        out_idx += len;
        idx++;
      } else if (type == 's') {
        const char* s = va_arg(ap, char*);
        len = print_s(f, out + out_idx, s, padding, padding_width);
        out_idx += len;
        idx++;
      } else if (type == '%') {
        f(out + out_idx, '%');
        out_idx += 1;
        idx++;
      } else if (type == '0') {

      } else {
        // TODO
        assert(0);
      }
    } else {
      f(out + out_idx, fmt[idx]);
      out_idx++;
      idx++;
    }
  }
  f(out + out_idx, '\0');

  return out_idx;
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  size_t len = vsprintf_ex(putc_stdio, NULL, fmt, ap);
  va_end(ap);
  return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  size_t out_idx = vsprintf_ex(putc_buf, out, fmt, ap);
  return out_idx;
}

int sprintf(char *out, const char *fmt, ...) {
  assert(out && fmt);

  size_t out_len = 0;
  va_list ap;
  va_start(ap, fmt);

  out_len = vsprintf(out, fmt, ap);
  va_end(ap);

  return out_len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  assert(0);
  return 0;
}

#endif
