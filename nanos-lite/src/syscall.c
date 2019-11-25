#include "common.h"
#include "syscall.h"
#include <fs.h>

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  //printk("\t\tSyscall NO %d\n", a[0]);
  switch (a[0]) {
    case SYS_exit:
      _halt(a[1]);
      break;
    case SYS_yield: 
      _yield();
      c->GPRx = 0;
      break;
    case SYS_write:
      if ((int)a[1] == 1 || (int)a[1] == 2) { // stdout or stderr
        for (size_t i = 0; i < (size_t)a[3]; i++) {
          _putc(((char*)(a[2]))[i]);
        }
        c->GPRx = a[3];
      } else if ((int)a[1] > 2) { // files
        c->GPRx = fs_write((int)(a[1]), (void*)(a[2]), (size_t)(a[3]));
      }
      break;
    case SYS_brk:
      // do nothing
      // printk("[syscall] brk for %x bytes\n", (int)(a[1]));
      break;
    case SYS_open:
      c->GPRx = fs_open((const char*)(a[1]), (int)(a[2]), (int)(a[3]));
      break;
    case SYS_read:
      c->GPRx = fs_read((int)(a[1]), (void*)(a[2]), (size_t)(a[3]));
      break;
    case SYS_lseek:
      c->GPRx = fs_lseek((int)(a[1]), (size_t)(a[2]), (int)(a[3]));
      break;
    case SYS_close:
      c->GPRx = fs_close((int)(a[1]));
      break;
    default: 
      panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
