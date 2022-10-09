/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "watchpoint.h"
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args)
{
  int n = 1;
  if (args != NULL)
  {
    sscanf(args, "%d", &n);
  }
  cpu_exec(n);
  // cmd_c(-1)
  return 0;
}

static int cmd_q(char *args)
{
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_info(char *args)
{
  if (args == NULL)
  {
    printf("缺少参数\n");
  }
  else if (strcmp(args, "r") == 0)
  {
    isa_reg_display();
  }
  else if (strcmp(args, "w") == 0)
  {
    print_infowatchpoints();
  }
  else
  {
    printf("未知参数: [%s] \n", args);
  }
  return 0;
}

int hex(char ch)
{
  if (ch >= '0' && ch <= '9')
    return 1;
  if (ch >= 'A' && ch <= 'F')
    return 1;
  if (ch >= 'a' && ch <= 'f')
    return 1;
  return 0;
}

static int cmd_x(char *args)
{

  char *arg = strtok(NULL, " ");
  int n;
  sscanf(arg, "%d", &n);
  // vaddr_t =32uint32
  char *p = strtok(NULL, " ");
  vaddr_t address;
  sscanf(p, "%x", &address);
  for (int i = 0; i < n; i++)
  {
    if (i % 4 == 0)
      printf("\n0x%x:", address);
    printf(" 0x%02x", vaddr_read(address, 1));
    address += 1;
  }
  return 0;
}

static int cmd_p(char *args)
{
  // char *arg = strtok(NULL, "\0");
  bool success;
  uint32_t ans = expr(args, &success);
  if (success)
    printf("%u\n", ans);
  else
    printf("Error in expression\n");
  return 0;
}

static int cmd_w(char *args)
{
  bool success = true;
  WP *point = new_wp(args, &success);
  if (success)
  {
    printf("Created NO.%d expr:%s \n", point->NO, point->expr);
  }
  else
  {
    printf("Wrong in expr.\n");
  }
  return 0;
}

static int cmd_d(char *args)
{
  int NO;
  sscanf(args, "%d", &NO);
  de_wp(NO);
  printf("delete a surveillance point: %d.\n",NO);
  return 0;
}

static int cmd_help(char *args);

static struct
{
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Single-step execution", cmd_si},
    {"info", "r: Print state of Register", cmd_info},
    {"x", "Print program status", cmd_x},
    {"p", "Expression evaluation", cmd_p},
    {"w", "Set up a monitoring point", cmd_w},
    {"d", "Delete a surveillance point", cmd_d},

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode()
{
  is_batch_mode = true;
}

void sdb_mainloop()
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb()
{
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
