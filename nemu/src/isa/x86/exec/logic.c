#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test)
{
  // TODO();
  rtl_and(&s0, &id_dest->val, &id_src->val);

  rtl_update_ZFSF(&s0, id_dest->width);
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(test);
}

make_EHelper(and)
{
  // TODO();
  rtl_and(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(and);
}

make_EHelper(xor)
{
  // TODO();
  rtl_xor(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(xor);
}

make_EHelper(or)
{
  // TODO();
  rtl_or(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  rtl_li(&s1, 0);
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(or);
}

make_EHelper(sar)
{
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl)
{
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr)
{
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(shrd)
{
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  rtl_li(&s1, id_src2->width);
  rtl_shli(&s1, &s1, 3);
  rtl_subi(&s1, &s1, id_src->val);
  rtl_shl(&s1, &id_src2->val, &s1);
  rtl_or(&s0, &s0, &s1);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template3(shrd);
}

make_EHelper(setcc)
{
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not )
{
  // TODO();
  rtl_not(&s0, &id_dest->val);
  operand_write(id_dest, &s0);

  print_asm_template1(not );
}

make_EHelper(rol)
{
  rtl_li(&s0, id_dest->val);
  rtl_shri(&s1, &s0, id_dest->width * 8 - id_src->val);
  rtl_shli(&s0, &s0, id_src->val);
  rtl_or(&s0, &s1, &s0);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template1(rol);
}