#include "cpu/exec.h"

make_EHelper(lidt) {
  TODO();

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  TODO();

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  if (id_dest->width == 1) {
    id_dest->val = pio_read_b(id_src->val);
  } else if (id_dest->width == 2) {
    id_dest->val = pio_read_w(id_src->val);
  } else if (id_dest->width == 4) {
    id_dest->val = pio_read_l(id_src->val);
  } else {
    assert(0);
  }
  operand_write(id_dest, &id_dest->val);

  print_asm_template2(in);
}

make_EHelper(out) {
  if (id_src->width == 1) {
    pio_write_b(id_dest->val, id_src->val);
  } else if (id_src->width == 2) {
    pio_write_w(id_dest->val, id_src->val);
  } else if (id_src->width == 4) {
    pio_write_l(id_dest->val, id_src->val);
  } else {
    assert(0);
  }

  print_asm_template2(out);
}
