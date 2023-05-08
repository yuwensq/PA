#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  size_t len = 0;
  while (s[len] != '\0')
    len++;
  return len;
}

char *strcpy(char *dst, const char *src)
{
  size_t i;
  for (i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dest, const char *source, size_t count)
{
  char *start = dest;
  while (count && (*dest++ = *source++))
    count--;
  if (count)
    while (--count)
      *dest++ = '\0';
  return (start);
}

char *strcat(char *dst, const char *src)
{
  size_t dst_len = strlen(dst);
  size_t i;
  for (i = 0; src[i] != '\0'; i++)
    dst[dst_len + i] = src[i];
  dst[dst_len + i] = '\0';
  return dst;
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
  if (NULL == out || NULL == in)
  {
    return NULL;
  }

  void *ret = out;

  if (out <= in || (char *)out >= (char *)in + n)
  {
    // 没有内存重叠，从低地址开始复制
    while (n--)
    {
      *(char *)out = *(char *)in;
      out = (char *)out + 1;
      in = (char *)in + 1;
    }
  }
  else
  {
    // 有内存重叠，从高地址开始复制
    in = (char *)in + n - 1;
    out = (char *)out + n - 1;
    while (n--)
    {
      *(char *)out = *(char *)in;
      out = (char *)out - 1;
      in = (char *)in - 1;
    }
  }
  return ret;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  const uint8_t *a = s1;
  const uint8_t *b = s2;
  size_t i = 0;
  for (i = 0; i < n && a[i] == b[i]; i++)
    ;
  if (i == n)
    return 0;
  if (a[i] - b[i] > 0)
    return 1;
  else if (a[i] - b[i] < 0)
    return -1;
  return 0;
}

#endif
