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
    Log("Hello World from Nanos-lite for the %dth time! from: %s", j, (char *)arg);
    j++;
    _yield();
  }
}

void naive_uload(PCB *, const char *);
void context_kload(PCB *pcb, void *entry, void *arg);
void context_uload(PCB *pcb, const char *filename, int argc, char *const argv[], char *const envp[]);

char *arg[] = {"/bin/pal", "--skip"};

void init_proc()
{
  // context_kload(&pcb[0], hello_fun, "kernel thread 1");
  context_uload(&pcb[0], "/bin/pal", 1, arg, NULL);
  context_uload(&pcb[1], "/bin/hello", 1, arg, NULL);
  // context_kload(&pcb[1], hello_fun, "kernel thread 2");
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/pal");
}

_Context *schedule(_Context *prev)
{
  static int pcb1_times = 0;
  current->cp = prev;
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  current = &pcb[0];
  if (pcb1_times == 2) {
    current = &pcb[1];
    pcb1_times = 0;
  }
  pcb1_times ++;
  // current = &pcb[0];
  // Log("%x %x", pcb[0].cp, pcb[0].cp->as);
  return current->cp;
}
