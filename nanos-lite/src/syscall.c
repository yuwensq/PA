#include "common.h"
#include "syscall.h"

_Context *do_syscall(_Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0])
  {
  case SYS_yield:
    _yield();
    c->GPRx = 0;
    break;
  case SYS_exit:
    c->GPRx = 0;
    _halt(a[1]);
    break;
  case SYS_write:
  {
    int fd = a[1];
    char *buf = (char *)a[2];
    if (fd != 1 && fd != 2)
      panic("not impletement");
    for (int i = 0; i < a[3]; i++)
      _putc(buf[i]);
    c->GPRx = a[3];
  }
  break;
  case SYS_brk:
    c->GPRx = 0;
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
