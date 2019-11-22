#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  assert(dst && src);

  size_t idx = 0;
  while (src[idx] != '\0') {
    dst[idx] = src[idx];
    idx++;
  }
  dst[idx] = '\0';

  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert(0);
  return NULL;
}

char* strcat(char* dst, const char* src) {
  assert(dst && src);
  size_t dst_len = strlen(dst);
  size_t src_len = strlen(src);
  size_t idx = 0;

  for (idx = 0; idx < src_len; idx++) {
    dst[dst_len + idx] = src[idx];
  }
  dst[dst_len + src_len] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  int idx = 0;
  while (s1[idx] == s2[idx]) {
    if (s1[idx] == '\0') {
      return 0;
    }
    idx++;
  }
  if (s1[idx] < s2[idx]) {
    return -1;
  } else if (s1[idx] > s2[idx]) {
    return 1;
  }
  assert(0);
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(0);
  return 0;
}

void* memset(void* v,int c,size_t n) {
  uint8_t* ptr = v;
  size_t idx = 0;

  for (idx = 0; idx < n; idx++) {
    ptr[idx] = (uint8_t)c;
  }

  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(out && in);
  for (size_t i = 0; i < n; i++) {
    ((unsigned char*)out)[i] = ((unsigned char*)in)[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  int i;
  const uint8_t* ptr1 = s1;
  const uint8_t* ptr2 = s2;
  for (i = 0; i < n; i++) {
    if (ptr1[i] < ptr2[i]) {
      return -1;
    } else if (ptr1[i] > ptr2[i]) {
      return 1;
    }
  }
  return 0;
}

#endif
