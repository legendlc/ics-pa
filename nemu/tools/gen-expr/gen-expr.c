#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NUM_DIGITS 3
#define MAX_SPACE_NUM  3
#define MAX_TOKEN 32
#define BUF_SIZE 65536

#define NR_OPERATOR_NUMERIC 4
#define NR_OPERATOR_LOGICAL 3
#define OPERATOR_NUMERIC_FREQ 3
#define OPERATOR_LOGICAL_FREQ 1
#define RAND_OPERATOR_SUM (NR_OPERATOR_NUMERIC * OPERATOR_NUMERIC_FREQ \
                              + NR_OPERATOR_LOGICAL * OPERATOR_LOGICAL_FREQ)
#define RAND_OPERATOR_LOGICAL_BASE (NR_OPERATOR_NUMERIC * OPERATOR_NUMERIC_FREQ)

static char buf[BUF_SIZE];
static int buf_len = 0;
static int token_num = 0;
static bool buf_full = false;

static inline int choose(int n) {
  return rand() % n;
}

static void gen_rand_operator() {
  if (buf_full) {
    return;
  }

  int operator_len = 0;
  int operator = choose(RAND_OPERATOR_SUM);
  if (operator < RAND_OPERATOR_LOGICAL_BASE) {
    // +-*/
    operator_len = 1;
  } else {
    // == or !=
    operator_len = 2;
  }

  if (buf_len + operator_len >= BUF_SIZE) {
      buf_full = true;
      return;
  }

  if (operator < RAND_OPERATOR_LOGICAL_BASE) {
    switch (operator / OPERATOR_NUMERIC_FREQ) {
      case 0:
        buf[buf_len] = '+';
        break;
      case 1:
        buf[buf_len] = '-';
        break;
      case 2:
        buf[buf_len] = '*';
        break;
      case 3:
        buf[buf_len] = '/';
        break;
      default:
        assert(0);
    }
  } else {
    switch ((operator - RAND_OPERATOR_LOGICAL_BASE) / OPERATOR_LOGICAL_FREQ)
    {
      case 0:
        buf[buf_len] = '=';
        buf[buf_len + 1] = '=';
        break;
      case 1:
        buf[buf_len] = '!';
        buf[buf_len + 1] = '=';
        break;
      case 2:
        buf[buf_len] = '&';
        buf[buf_len + 1] = '&';
        break;
      default:
        assert(0);
    }
  }
  token_num++;
  buf_len += operator_len;
}

static void gen_rand_spaces(int n) {
  if (buf_full) {
    return;
  }

  int cnt = choose(n);
  if (buf_len + cnt >= BUF_SIZE) {
    buf_full = true;
  }

  for (int i = 0; i < cnt; i++) {
    buf[buf_len + i] = ' ';
  }

  buf_len += cnt;
}

static inline void gen_rand_num(int max_digits) {
  if (buf_full) {
    return;
  }

  int digits = choose(max_digits) + 1;
  int radix = choose(2);

  if (radix == 0) {
    // decimal
    if (buf_len + digits >= BUF_SIZE) {
      buf_full = true;
      return;
    }

    for (int i = 0; i < digits; i++) {
      if (i == 0 && digits > 1) {
        buf[buf_len + i] = choose(9) + 1 + '0';
      } else {
        buf[buf_len + i] = choose(10) + '0';
      }
    }
    buf_len += digits;
    token_num++;
  } else {
    // hexadecimal
    if (buf_len + digits + 2 >= BUF_SIZE) {
      buf_full = true;
      return;
    }

    buf[buf_len] = '0';
    buf[buf_len + 1] = 'x';
    for (int i = 0; i < digits; i++) {
      buf[buf_len + 2 + i] = choose(10) + '0';
    }
    buf_len += (digits + 2);
    token_num++;
  }
}

static inline void gen_char(char ch) {
  if (buf_full) {
    return;
  }

  if (buf_len + 1 >= BUF_SIZE) {
    buf_full = true;
    return;
  }

  token_num++;
  buf[buf_len++] = ch;
}

static void _gen_rand_expr() {
  if (buf_full) {
    return;
  }

  switch (choose(3))
  {
  case 0:
    gen_rand_num(MAX_NUM_DIGITS);
    break;
  case 1:
    gen_char('(');
    gen_rand_spaces(MAX_SPACE_NUM);
    _gen_rand_expr();
    gen_rand_spaces(MAX_SPACE_NUM);
    gen_char(')');
    break;
  case 2:
    _gen_rand_expr();
    gen_rand_spaces(MAX_SPACE_NUM);
    gen_rand_operator();
    gen_rand_spaces(MAX_SPACE_NUM);
    _gen_rand_expr();
    break;
  default:
    assert(0);
  }
}

static inline void gen_rand_expr() {
  buf_len = 0;
  buf[buf_len] = '\0';
  buf_full = false;
  token_num = 0;
  _gen_rand_expr();

  if (!buf_full) {
    buf[buf_len] = '\0';
  } else {
    buf[0] = '\0';
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i = 0;
  while (i < loop) {
    gen_rand_expr();
    if (buf_full || token_num > MAX_TOKEN) {
      continue;
    }

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if (1 == fscanf(fp, "%d", &result)) {
      printf("%u %s\n", result, buf);
      i++;
    }
    pclose(fp);
  }
  return 0;
}
