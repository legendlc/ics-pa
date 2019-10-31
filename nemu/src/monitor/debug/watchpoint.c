#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

static WP* _new_wp() {
  if (free_ == NULL) {
    return NULL;
  }

  WP* wp = free_;
  free_ = free_->next;
  wp->next = NULL;

  if (head == NULL) {
    head = wp;
  } else {
    WP* tmp = head;
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = wp;
  }

  return wp;
}

WP* new_wp(char* e) {
  Assert(e != NULL, "arg");
  if (strlen(e) >= WP_EXPR_MAX_LEN) {
    printf("EXPR length too long\n");
    return NULL;
  }

  uint32_t result;
  bool success;
  result = expr(e, &success);
  if (!success) {
    printf("Invalid expression\n");
    return NULL;
  }

  WP* wp;
  if ((wp = _new_wp()) == NULL) {
    printf("Watch point num up to limit\n");
    return NULL;
  }
  strcpy(wp->expr, e);
  wp->prev_value = result;

  return wp;
}

void _free_wp(WP *wp) {
  Assert(wp != NULL, "arg");
  WP* tmp = head;

  while (tmp != NULL && tmp->next != wp) {
    tmp = tmp->next;
  }
  if (tmp != NULL) {
    // tmp->next == wp
    tmp->next = tmp->next->next;
  } else {
    // wp == head
    head = wp->next;
  }
  wp->next = free_;
  free_ = wp;
}

void delete_wp(int no) {
  WP* tmp = head;
  while (tmp != NULL) {
    if (tmp->NO == no) {
      _free_wp(tmp);
      break;
    }
    tmp = tmp->next;
  }
}

void display_wp() {
  WP* tmp = head;
  while (tmp != NULL) {
    printf("[%d] %s -> 0x%x/%u/%d\n", tmp->NO, tmp->expr, 
      tmp->prev_value, tmp->prev_value, (int32_t)tmp->prev_value);
    tmp = tmp->next;
  }
}

void test_wp() {
  WP* wp = NULL;
  for (int i = 0; i < NR_WP; i++) {
    wp = _new_wp();
    assert(wp != NULL);
  }
  wp = _new_wp();
  assert(wp == NULL);

  for (int i = 0; i < NR_WP; i++) {
    delete_wp(i);
  }

  for (int i = 0; i < NR_WP; i++) {
    wp = _new_wp();
    assert(wp != NULL);
  }
  wp = _new_wp();
  assert(wp == NULL);

  for (int i = NR_WP - 1; i >= 0; i--) {
    delete_wp(i);
  }
}

bool refresh_wp() {
  WP* wp = head;
  bool has_change = false;
  while (wp != NULL) {
    bool success;
    uint32_t new_value = expr(wp->expr, &success);
    has_change = has_change || (new_value != wp->prev_value);
    wp->prev_value = new_value;

    wp = wp->next;
  }

  return has_change;
}
