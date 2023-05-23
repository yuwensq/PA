#include "rtl/rtl.h"
#define IRQ_TIMER 32

void raise_intr(uint32_t NO, vaddr_t ret_addr)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  Assert(NO < cpu.idtr.limit, "int number is too large");
  rtl_push(&cpu.eflags);
  cpu.IF = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  uint32_t addr_hi, addr_lo, present;
  addr_lo = vaddr_read(cpu.idtr.base + 8 * NO, 2);
  addr_hi = (vaddr_read(cpu.idtr.base + 8 * NO + 4, 4) & 0xffff0000);
  present = (vaddr_read(cpu.idtr.base + 8 * NO + 5, 1) >> 7);
  Assert(present, "idt entry not exist");
  rtl_j(addr_hi | addr_lo);
}

bool isa_query_intr(void)
{
  if (cpu.INTR == true && cpu.IF == 1)
  {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
}
