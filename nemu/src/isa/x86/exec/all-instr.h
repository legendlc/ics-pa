#include "cpu/exec.h"

// logic
make_EHelper(test);
make_EHelper(xor);
make_EHelper(and);
make_EHelper(setcc);
// arith
make_EHelper(add);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(inc);
// data-mov
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(leave);
make_EHelper(lea);
make_EHelper(movzx);
// control
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jcc);
// special
make_EHelper(operand_size);
make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(nop);
