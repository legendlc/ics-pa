#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&s0);
  rtl_sr(id_dest->reg, &s0, id_dest->width);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  assert(id_dest->width == 4);
  rtl_lr(&s0, R_EBP, id_dest->width);
  rtl_sr(R_ESP, &s0, id_dest->width);
  rtl_pop(&s0);
  rtl_sr(R_EBP, &s0, id_dest->width);

  print_asm("leave");
}

make_EHelper(cltd) {
  rtl_lr(&s0, R_EAX, id_dest->width);
  if (SIGN_BIT(s0, id_dest->width) > 0) {
    rtl_li(&s1, 0xFFFFFFFF);
  } else {
    rtl_li(&s1, 0x0);
  }
  rtl_sr(R_EDX, &s1, id_dest->width);

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_EAX, 1);
    rtl_sext(&s0, &s0, 1);
    rtl_sr(R_EAX, &s0, 2);
  }
  else {
    rtl_lr(&s0, R_EAX, 2);
    rtl_sext(&s0, &s0, 2);
    rtl_sr(R_EAX, &s0, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
