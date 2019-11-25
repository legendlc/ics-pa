#----|------------entry------------|---irq id---|-----handler-----|
.globl __am_vecsys;    __am_vecsys: pushl $0x80; jmp __am_asm_trap
.globl __am_vectrap;  __am_vectrap: pushl $0x81; jmp __am_asm_trap
.globl __am_irq0;        __am_irq0: pushl   $32; jmp __am_asm_trap
.globl __am_vecnull;  __am_vecnull: pushl   $-1; jmp __am_asm_trap

__am_asm_trap:
  pushal

  pushl $0

  pushl %esp
  call __am_irq_handle
  mov 0x20(%eax), %eax

  addl $4, %esp
  addl $4, %esp

  mov %eax, 0x1c(%esp)
  popal
  addl $4, %esp

  iret
