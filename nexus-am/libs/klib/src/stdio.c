#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  return 0;
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
    case 'd': // 十进制
      num = va_arg(ap, int32_t);
      while (num)
      {
        *str++ = '0' + (num % 10);
        num /= 10;
      }
      continue;
    default:
      break;
    }
  }
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
