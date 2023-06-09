#include "common.h"
#include "am.h"

_Context *do_syscall(_Context *c);
extern _Context *schedule(_Context *prev);

static _Context *do_event(_Event e, _Context *c)
{
  _Context *ret = NULL;
  switch (e.event)
  {
  case _EVENT_YIELD:
    ret = schedule(c);
    // printf("yield\n");
    break;
  case _EVENT_SYSCALL:
    // printf("syscall\n");
    do_syscall(c);
    break;
  case _EVENT_IRQ_TIMER:
    // Log("timer irq");
    _yield();
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return ret;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
