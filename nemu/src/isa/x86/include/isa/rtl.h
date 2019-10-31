#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

#define SIGN_BIT(v, width) (((v) >> (width * 8 - 1)) & 0x1)

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  rtl_lr(&t0, R_ESP, 4);
  rtl_subi(&t0, &t0, 4);
  rtl_sm(&t0, src1, 4);
  rtl_sr(R_ESP, &t0, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lr(&t0, R_ESP, 4);
  rtl_lm(dest, &t0, 4);
  rtl_addi(&t0, &t0, 4);
  rtl_sr(R_ESP, &t0, 4);
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  if (SIGN_BIT(*src1, width) != SIGN_BIT(*src2, width)
    && SIGN_BIT(*res, width) != SIGN_BIT(*src1, width)) {
    *dest = 1;
  } else {
    *dest = 0;
  }
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  if (*res > *src1) {
    rtl_li(dest, 1);
  } else {
    rtl_li(dest, 0);
  }
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  if (SIGN_BIT(*src1, width) == SIGN_BIT(*src2, width)
    && SIGN_BIT(*res, width) != SIGN_BIT(*src1, width)) {
    *dest = 1;
  } else {
    *dest = 0;
  }
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  if (*res < *src1) {
    *dest = 1;
  } else {
    *dest = 0;
  }
}

#define CPU_EFLAGS(f) (cpu.f)

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    if (*src != 0) { \
      CPU_EFLAGS(f) = 1; \
    } else { \
      CPU_EFLAGS(f) = 0; \
    } \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    if (CPU_EFLAGS(f) != 0) { \
      *dest = 1; \
    } else { \
      *dest = 0; \
    } \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  assert(width >= 1 && width <= 4);
  if (((*result) & (0xFFFFFFFF >> ((4 - width) * 8))) == 0) {
    rtl_li(&t0, 1);
  } else {
    rtl_li(&t0, 0);
  }
  rtl_set_ZF(&t0);
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  assert(width >= 1 && width <= 4);
  if (SIGN_BIT(*result, width) == 0) {
    rtl_li(&t0, 0);
  } else {
    rtl_li(&t0, 1);
  }
  rtl_set_SF(&t0);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
