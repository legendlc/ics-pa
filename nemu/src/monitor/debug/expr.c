#include <stdlib.h>
#include <stdio.h>
#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_DEC,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"\\*", '*'},         // multiply
  {"/", '/'},           // divide
  {"==", TK_EQ},        // equal
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"[[:digit:]]+", TK_DEC},     // decimal number
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

#define MAX_NR_TOKEN (32)
#define MAX_TOKEN_LEN (32)

typedef struct token {
  int type;
  char str[MAX_TOKEN_LEN];
} Token;

static Token tokens[MAX_NR_TOKEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            // i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          default: 
            if (nr_token >= MAX_NR_TOKEN) {
              panic("Token number up to limit");
            } else if (substr_len >= MAX_TOKEN_LEN) {
              panic("Token length up to limit");
            }
            
            tokens[nr_token].type = rules[i].token_type;
            // strlen(substr) < MAX_TOKEN_LEN
            snprintf(tokens[nr_token].str, MAX_TOKEN_LEN, "%.*s", substr_len, substr_start);
            nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// validate all brackets in expression are matched
static bool check_brackets(int begin, int end) {
  int balance = 0;
  for (int i = begin; i <= end; i++) {
    if (tokens[i].type == '(') {
      balance++;
    } else if (tokens[i].type == ')') {
      balance--;
    }
    if (balance < 0) {
      return false;
    }
  }

  return (balance == 0);
}

static int find_matched_parenthesis(int begin, int end) {
  Assert(tokens[begin].type == '(', "arg");

  int balance = 0;
  int index = -1;
  for (int i = begin; i <= end; i++) {
    if (tokens[i].type == '(') {
      balance++;
    } else if (tokens[i].type == ')') {
      balance--;
      if (balance == 0) {
        index = i;
        break;
      }
    }
  }

  return index;
}

// check surrounded by a pair of matched bracket
static bool check_surrounded_by_matched_brackets(int begin, int end) {
  Assert(0 <= begin && end > begin && end < nr_token, "Token range");

  if (tokens[begin].type != '(' || tokens[end].type != ')') {
    return false;
  }

  return find_matched_parenthesis(begin, end) == end;
}

static bool is_operand(int type) {
  return type == '+' || type == '-' || type == '*' || type == '/';
}

static int operand_priority(int type) {
  if (type == '+' || type == '-') {
    return 1;
  } else if (type == '*' || type == '/') {
    return 2;
  } else {
    UNREACHABLE();
  }

  return 0;
}

// find main operand in an expression
//     1. lowest priority
static int find_main_operand(int begin, int end) {
  if (end < begin) {
    return -1;
  }

  int operand = -1;
  int index = begin;

  while (index <= end) {
    if (tokens[index].type == '(') {
      // skip any token in parentheses
      index = find_matched_parenthesis(index, end) + 1;
      continue;
    } else if (is_operand(tokens[index].type)) {
      if (operand < 0
        || operand_priority(tokens[index].type) <= operand_priority(tokens[operand].type)) {
        operand = index;
      }
      index++;
    } else {
      index++;
    }
  }

  return operand;
}

static bool calc(int begin, int end, uint32_t *result) {
  if (begin > end) {
    return false;
  }

  // only 1 token, it could only be number
  if (end == begin) {
    if (tokens[begin].type == TK_DEC) {
      *result = strtoul(tokens[begin].str, NULL, 0);
      return true;
    } else {
      return false;
    }
  }

  // expression is surrounded by a pair of matched brackets
  if (check_surrounded_by_matched_brackets(begin, end)) {
    return calc(begin + 1, end - 1, result);
  }

  // find main operand
  uint32_t left_result = 0;
  uint32_t right_result = 0;
  int operand = -1;

  if (0 > (operand = find_main_operand(begin, end))) {
    return false;
  }

  if (calc(begin, operand - 1, &left_result) 
    && calc(operand + 1, end, &right_result)) {
      switch (tokens[operand].type)
      {
      case '+':
        *result = left_result + right_result;
        break;
      case '-':
        *result = left_result - right_result;
        break;
      case '*':
        *result = left_result * right_result;
        break;
      case '/':
        *result = left_result / right_result;
        break;
      default:
        panic("Unreachable");
      }
  } else {
    return false;
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  // printf("Get %d tokens\n", nr_token);
  if (nr_token == 0) {
    *success = true;
    return 0;
  }

  if (!check_brackets(0, nr_token - 1)) {
    *success = false;
    return 0;
  }

  uint32_t result = 0;
  *success = calc(0, nr_token - 1, &result);
  if (!(*success)) {
    return 0;
  }

  return result;
}

void expr_test() {
  // testcase is generated by the following command
  // $ gen-expr [N] 2>/dev/null 1>./expr_testcases
  const char* testcase_file = "./tools/gen-expr/expr_testcases";
  size_t buf_len = 1024; 
  char* buf = malloc(buf_len);
  FILE* f = NULL;
  int testcase_cnt = 0;
  int success_cnt = 0;

  if ((f = fopen(testcase_file, "r")) != NULL) {
    bool success = false;
    ssize_t line_len;
    while ((line_len = getline(&buf, &buf_len, f)) > 0) {
      testcase_cnt++;
      // line format: "result expr\n"
      uint32_t expect = strtoul(buf, NULL, 0);
      uint32_t result;

      // find start of expression
      int index = 0;
      while (buf[index++] != ' ') {}
      // remove the trailing '\n'
      buf[line_len - 1] = '\0';

      result = expr(buf + index, &success);
      Assert(success, "valid expression");
      if (expect == result) {
        success_cnt++;
      } else {
        printf("[fail] %s = %u, expect %u\n", buf + index, result, expect);
      }
    }

    fclose(f);
   }

  free(buf);
  Log("[expr] %d/%d testcases passed\n", success_cnt, testcase_cnt);
}
