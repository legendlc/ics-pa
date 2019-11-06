#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&id_dest->val, &id_dest->val, &id_src->val);

  rtl_li(&s0, 0);
  rtl_set_OF(&s0);
  rtl_set_CF(&s0);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);

  rtl_li(&s0, 0);
  rtl_set_OF(&s0);
  rtl_set_CF(&s0);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);

  rtl_li(&s0, 0);
  rtl_set_OF(&s0);
  rtl_set_CF(&s0);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);

  rtl_li(&s0, 0);
  rtl_set_OF(&s0);
  rtl_set_CF(&s0);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  if (id_dest->width == 2) {
    rtl_sar_16(&s0, &id_dest->val, &id_src->val);
  } else if (id_dest->width == 4) {
    rtl_sar(&s0, &id_dest->val, &id_src->val);
  } else {
    assert(0);
  }
  operand_write(id_dest, &s0);
  // unnecessary to update CF and OF in NEMU
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // unnecessary to update CF and OF in NEMU
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  // unnecessary to update CF and OF in NEMU
  rtl_update_ZFSF(&s0, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(rol) {
  if (id_dest->width == 1) {
    rtl_li(&ir, 0xFF);
  } else if (id_dest->width == 2) {
    rtl_li(&ir, 0xFFFF);
  } else if (id_dest->width == 4) {
    rtl_li(&ir, 0xFFFFFFFF);
  } else {
    assert(0);
  }

  int times = (id_src->val) % (id_dest->width * 8);
  for (int i = 0; i < times; i++) {
    rtl_li(&s0, SIGN_BIT(id_dest->val, id_dest->width));
    rtl_get_CF(&s1);
    id_dest->val = (id_dest->val << 1);
    id_dest->val &= ir;
    id_dest->val |= s1;
    rtl_set_CF(&s0);
  }

  operand_write(id_dest, &id_dest->val);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&s0, &id_dest->val);
  operand_write(id_dest, &s0);

  print_asm_template1(not);
}
