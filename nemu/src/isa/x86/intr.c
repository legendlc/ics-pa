#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  assert(NO * 8 <= cpu.idtr.limit + 1);
  uint32_t idt_low = vaddr_read(cpu.idtr.base + NO * 8, 4);
  uint32_t idt_high = vaddr_read(cpu.idtr.base + NO * 8 + 4, 4);
  vaddr_t service_addr = (idt_low & 0xFFFF) + (idt_high & 0xFFFF0000);

  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  rtl_j(service_addr);
  
}

bool isa_query_intr(void) {
  return false;
}
