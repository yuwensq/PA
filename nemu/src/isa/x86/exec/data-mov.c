#include "cpu/exec.h"

make_EHelper(mov)
{
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push)
{
  // TODO();
  rtl_sext(&s0, &id_dest->val, id_dest->width);
  rtl_push(&s0);

  print_asm_template1(push);
}

make_EHelper(pop)
{
  // TODO();
  rtl_pop(&s0);
  operand_write(id_dest, &s0);

  print_asm_template1(pop);
}

make_EHelper(pusha)
{
  // TODO();
  rtl_mv(&s0, &cpu.esp);
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&s0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa)
{
  // TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&s0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave)
{
  // TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd)
{
  if (decinfo.isa.is_operand_size_16)
  {
    // TODO();
    rtl_msb(&s0, &cpu.eax, 2);
    rtl_li(&s1, 0);
    rtl_sub(&s1, &s1, &s0);
    rtl_andi(&s1, &s1, 0xffff);
    rtl_andi(&cpu.edx, &cpu.edx, 0xffff0000);
    rtl_or(&cpu.edx, &cpu.edx, &s1);
  }
  else
  {
    // TODO();
    rtl_sari(&cpu.edx, &cpu.eax, 31);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl)
{
  if (decinfo.isa.is_operand_size_16)
  {
    // TODO();
    rtl_msb(&s0, &cpu.eax, 1);
    rtl_li(&s1, 0);
    rtl_sub(&s0, &s1, &s0);
    rtl_shli(&s0, &s0, 24);
    rtl_shri(&s0, &s0, 16);
    rtl_or(&cpu.eax, &cpu.eax, &s0);
  }
  else
  {
    // TODO();
    rtl_sext(&cpu.eax, &cpu.eax, 2);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx)
{
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx)
{
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea)
{
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(movsb)
{
  rtl_lm(&s0, &cpu.esi, 1);
  rtl_sm(&cpu.edi, &s0, 1);
  ++cpu.esi;
  ++cpu.edi;
  print_asm("movsb");
}

make_EHelper(movswd)
{
  rtl_lm(&s0, &cpu.esi, id_dest->width);
  rtl_sm(&cpu.edi, &s0, id_dest->width);
  rtl_addi(&cpu.esi, &cpu.esi, id_dest->width);
  rtl_addi(&cpu.edi, &cpu.edi, id_dest->width);
  print_asm("movswd");
}