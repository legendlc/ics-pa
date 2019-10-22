#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char* args);
static int cmd_p(char* args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "si [N] - Execute next N instructions, when N is not specified, execute one instruction", cmd_si },
  { "info", "info [r] - show register status\n\tinfo [w] show watch point status", cmd_info },
  { "x", "x N EXPR - Scan memory from EXPR, up to N bytes", cmd_x },
  { "p", "p EXPR - print result of the given expression", cmd_p },

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int steps;

  if (arg == NULL) {
    // default one step
    steps = 1;
  } else {
    // when arg is invalid, strtol returns 0
    steps = strtol(arg, NULL, 0);
  }

  if (steps > 0) {
    cpu_exec(steps);
  } else {
    printf("Invalid argument: %s\n", arg);
  }

  return 0;
}

static int cmd_info(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int steps;

  if (arg == NULL) {
    printf("Lack subcmd\n");
    return 0;
  } else {
    if (strcmp(arg, "r") == 0) {
      isa_reg_display();
    } else if (strcmp(arg, "w") == 0) {
      // TODO: watch point
    } else {
      printf("Invalid subcmd\n");
    }

    return 0;
  }

  if (steps > 0) {
    cpu_exec(steps);
  }

  return 0;
}

#define X_BYTES_PER_LINE 8

static int cmd_x(char* args) {
  /* extract first two argument */
  char* arg = NULL;
  int nbytes = 0;
  vaddr_t addr = 0;

  if ((arg = strtok(NULL, " ")) == NULL) {
    printf("Invalid argument N\n");
    return 0;
  }
  nbytes = strtol(arg, NULL, 0);

  if ((arg = strtok(NULL, " ")) == NULL) {
    printf("Invalid argument EXPR\n");
    return 0;
  }
  // TODO: now EXPR can only be integer
  addr = strtol(arg, NULL, 0);

  if (nbytes > 0) {
    for (int i = 0; i < nbytes / X_BYTES_PER_LINE + (nbytes % X_BYTES_PER_LINE != 0); i++) {
      printf("0x%08x:\t", addr + i * X_BYTES_PER_LINE);

      for (int j = 0; j < X_BYTES_PER_LINE; j++) {
        int offset = i * X_BYTES_PER_LINE + j;
        if (offset >= nbytes) { 
          break; 
        }
        // TODO: check invalid memory addr
        printf("0x%02x", vaddr_read(addr + offset, 1));
        if (j == X_BYTES_PER_LINE - 1 || offset == nbytes - 1) {
          printf("\n");
        } else {
          printf("\t");
        }
      }
    }
  }

  return 0;
}

static int cmd_p(char* args) {
  bool success = false;
  uint32_t result = 0;

  if (args == NULL) {
    return 0;
  }

  result = expr(args, &success);
  if (!success) {
    printf("Invalid expression %s\n", args ? args : "");
  } else {
    printf("%u\n", result);
  }

  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
