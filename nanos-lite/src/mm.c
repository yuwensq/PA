#include "memory.h"
#include "proc.h"

static void *pf = NULL;
extern PCB *current;

void *new_page(size_t nr_page)
{
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p)
{
  panic("not implement yet");
}

#define ROUNDUP(a, sz) ((((uintptr_t)a) + (sz)-1) & ~((sz)-1))

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment)
{
  assert((current->max_brk & 0xfff) == 0);
  const int page_size = 4096;
  uintptr_t new_brk = ROUNDUP(brk + increment, page_size);
  if (new_brk > current->max_brk)
  {
    int page_num = ((new_brk - current->max_brk) / page_size);
    printf("%d\n", page_num);
    for (int i = 0; i < page_num; i++)
    {
      void *p_addr = new_page(1);
      _map(&current->as, (void *)current->max_brk, p_addr, 8);
      current->max_brk += page_size;
    }
  }
  return 0;
}

void init_mm()
{
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
  Log("vme init success");
}
