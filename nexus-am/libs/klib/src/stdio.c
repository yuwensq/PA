#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  char buf[65535] = {};
  va_list args;
  int i = 0;
  va_start(args, fmt);
  i = vsprintf(buf, fmt, args);
  // va_end(args);
  // char *c = buf;
  // while (*c != '\0')
  // {
  //   _putc(*c);
  // }
  // return i;
  return i + buf[0];
}

static char *set_num(int num, char *s)
{
  int len = 0;
  char buf[50];
  while (num)
  {
    buf[len++] = num % 10 + '0';
    num /= 10;
  }
  while (len > 0)
  {
    *s++ = buf[--len];
  }
  return s;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  int len, i;
  unsigned long long num;
  char *str;
  const char *s;

  for (str = out; *fmt; fmt++)
  {
    if (*fmt != '%')
    {
      *str++ = *fmt;
      continue;
    }
    fmt++;
    switch (*fmt)
    {
    case 's':
      s = va_arg(ap, char *);
      if (!s)
        s = "<NULL>";
      len = strlen(s);
      for (i = 0; i < len; i++)
      {
        *str++ = *s++;
      }
      continue;
    case 'd':
      num = va_arg(ap, int32_t);
      if (num < 0)
      {
        *str++ = '-';
        num = -num;
      }
      str = set_num(num, str);
      continue;
    default:
      break;
    }
  }
  _putc('1');
  *str = '\0';
  return str - out;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list args;
  int i;
  va_start(args, fmt);
  i = vsprintf(out, fmt, args);
  va_end(args);
  return i;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  return 0;
}

#endif
