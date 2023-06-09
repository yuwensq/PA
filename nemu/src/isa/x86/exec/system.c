#include "cpu/exec.h"

make_EHelper(lidt)
{
  // TODO();
  cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr)
{
  // TODO();
  if (id_dest->reg == 0)
    cpu.cr0.val = id_src->val;
  else if (id_dest->reg == 3)
  {
    cpu.cr3.val = id_src->val;
  }
  else
    panic("mov_r2cr error");
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r)
{
  // TODO();
  if (id_src->reg == 0)
    operand_write(id_dest, &cpu.cr0.val);
  else if (id_src->reg == 3)
    operand_write(id_dest, &cpu.cr3.val);
  else
    panic("mov_cr2r error");
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

void raise_intr(uint32_t, vaddr_t);

make_EHelper(int)
{
  // TODO();
  raise_intr(id_dest->val, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret)
{
  // TODO();
  rtl_pop(&s0);
  rtl_j(s0);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in)
{
  // TODO();
  switch (id_src->width)
  {
  case 1:
    s0 = pio_read_b(id_src->val);
    break;
  case 2:
    s0 = pio_read_w(id_src->val);
    break;
  case 4:
    s0 = pio_read_l(id_src->val);
    break;
  default:
    Assert(false, "in指令出错了");
    break;
  }
  operand_write(id_dest, &s0);

  print_asm_template2(in);
}

make_EHelper(out)
{
  // TODO();
  switch (id_src->width)
  {
  case 1:
    pio_write_b(id_dest->val, id_src->val);
    break;
  case 2:
    pio_write_w(id_dest->val, id_src->val);
    break;
  case 4:
    pio_write_l(id_dest->val, id_src->val);
    break;
  default:
    Assert(false, "out指令出错了");
    break;
  }

  print_asm_template2(out);
}
