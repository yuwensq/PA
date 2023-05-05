#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  char buf[2048] = {};
  va_list args;
  int i = 0;
  va_start(args, fmt);
  i = vsprintf(buf, fmt, args);
  va_end(args);
  char *c = buf;
  while (*c != '\0')
  {
    _putc(*c);
    c++;
  }
  return i;
}

static char *set_num(int num, char *s, int width, int hexi)
{
  int i;
  int len = 0;
  char buf[50];
  if (num == 0)
    buf[len++] = '0';
  while (num)
  {
    if (!hexi)
    {
      buf[len++] = num % 10 + '0';
      num /= 10;
    }
    else
    {
      int low_num = (num % 16);
      buf[len++] = low_num > 9 ? 'a' + low_num - 10 : low_num;
      num /= 16;
    }
  }
  for (i = 0; i < width - len; i++)
  {
    *s++ = '0';
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
  int end;
  int flag;
  int width;
  int hexi = 0;

  for (str = out; *fmt; fmt++)
  {
    if (*fmt != '%')
    {
      *str++ = *fmt;
      continue;
    }
    hexi = 0;
    end = 0;
    flag = 0;
    while (!end)
    {
      switch (*fmt++)
      {
      case '0':
        flag |= 1;
        break;
      default:
        end = 1;
        break;
      }
    }
    width = 0;
    while (*fmt <= '9' && *fmt >= '0')
    {
      width = width * 10 + *fmt - '0';
      fmt++;
    }
    switch (*fmt)
    {
    case 's':
      s = va_arg(ap, char *);
      if (!s)
        s = "<NULL>";
      len = strlen(s);
      for (i = 0; i < width - len; i++)
      {
        *str++ = ' ';
      }
      for (i = 0; i < len; i++)
      {
        *str++ = *s++;
      }
      continue;
    case 'x':
      hexi = 1;
    case 'd':
      num = va_arg(ap, int32_t);
      if (num < 0)
      {
        *str++ = '-';
        num = -num;
        width--;
      }
      str = set_num(num, str, width, hexi);
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
