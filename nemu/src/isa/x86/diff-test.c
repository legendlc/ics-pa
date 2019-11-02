#include "nemu.h"
#include "monitor/diff-test.h"

void isa_reg_display_ex(CPU_state* c);

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (ref_r->pc != cpu.pc) {
    printf("ref $eip = %x, dut $eip = %x, origin pc = 0x%x\n", ref_r->pc, cpu.pc, pc);
    return false;
  }

  if (memcmp(ref_r, &cpu, 32) != 0) {
    printf("\nRef register State diffs:\n");
    isa_reg_display_ex(ref_r);
    printf("\n");
    return false;
  }

  return true;
}

void isa_difftest_attach(void) {
}
