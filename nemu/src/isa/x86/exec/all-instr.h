#include "cpu/exec.h"

// logic
make_EHelper(xor);
// arith
make_EHelper(sub);
// data-mov
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
// control
make_EHelper(call);
make_EHelper(ret);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
