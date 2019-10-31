#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define WP_EXPR_MAX_LEN 128

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char expr[WP_EXPR_MAX_LEN];
  uint32_t prev_value;
  bool changed;

} WP;

WP* new_wp(char* expr);
void delete_wp(int no);
void display_wp();
void test_wp();
bool refresh_wp();

#endif
