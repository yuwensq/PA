#include "common.h"
#include "syscall.h"
#include "proc.h"
#include "fs.h"

extern void naive_uload(PCB *pcb, const char *filename);

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
    // naive_uload(NULL, "/bin/init");
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
    // c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
  }
  break;
  case SYS_brk:
    c->GPRx = 0;
    break;
  case SYS_open:
    c->GPRx = fs_open((void *)a[1], a[2], a[3]);
    break;
  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], a[2], a[3]);
    break;
  case SYS_close:
    c->GPRx = fs_close(a[1]);
    break;
  case SYS_execve:
    naive_uload(NULL, (const char*)a[1]);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
