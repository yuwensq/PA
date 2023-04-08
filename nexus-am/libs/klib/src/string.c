#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  return 0;
}

char *strcpy(char *dst, const char *src)
{
  return NULL;
}

char *strncpy(char *dst, const char *src, size_t n)
{
  return NULL;
}

char *strcat(char *dst, const char *src)
{
  return NULL;
}

int strcmp(const char *s1, const char *s2)
{
  while (*s1 && *s2 && *s1 == *s2)
  {
    s1++;
    s2++;
  }
  if (*s1 - *s2 > 0)
    return 1;
  else if (*s1 - *s2 < 0)
    return -1;
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  return 0;
}

void *memset(void *v, int c, size_t n)
{
  uint8_t *mem;
  uint8_t byte = (c & 0xff);
  for (mem = v; 0 < n; ++mem, --n)
  {
    *mem = byte;
  }
  return v;
}

void *memcpy(void *out, const void *in, size_t n)
{
  return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  return 0;
}

#endif
