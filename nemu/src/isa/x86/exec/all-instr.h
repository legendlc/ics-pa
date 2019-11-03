#include "cpu/exec.h"

// logic
make_EHelper(test);
make_EHelper(not);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(and);
make_EHelper(setcc);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
// arith
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sub);
make_EHelper(sbb);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(idiv);
make_EHelper(neg);
// data-mov
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(leave);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(cltd);
// control
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jcc);
make_EHelper(jmp);
// special
make_EHelper(operand_size);
make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(nop);
// system
make_EHelper(out);
