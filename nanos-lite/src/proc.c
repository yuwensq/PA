#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j++;
    _yield();
  }
}

void naive_uload(PCB *, const char *);
void context_kload(PCB *pcb, void *entry);

void init_proc()
{
  context_kload(&pcb[0], hello_fun);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/pal");
}

_Context *schedule(_Context *prev)
{
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  printf("%x\n", current->cp);
  return current->cp;
}
