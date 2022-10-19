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

#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include <../src/monitor/sdb/sdb.h>
#include <../src/monitor/ftracer.h>
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 32
bool wp_state;
CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;
void device_update();
#ifdef CONFIG_ITRACE_COND
#define RINGBUF_LINES 64
#define PRINTBUF_SIZE 128
char instr_ringbuf[RINGBUF_LINES][PRINTBUF_SIZE];
long ringbuf_end = 0;
static char last_instr[PRINTBUF_SIZE];
// functab_node *functab_head;
static void ringbuf_display()
{
  strncpy(instr_ringbuf[ringbuf_end++ % RINGBUF_LINES], last_instr, PRINTBUF_SIZE);
  for (int i = ringbuf_end >= RINGBUF_LINES ? ringbuf_end : 0;
       i < ringbuf_end + (ringbuf_end >= RINGBUF_LINES ? RINGBUF_LINES : 0);
       ++i)
  {
    printf("%s\n", instr_ringbuf[i % RINGBUF_LINES]);
  }
}

#define ir_write(...) IFDEF(                                                                 \
    CONFIG_TARGET_NATIVE_ELF,                                                                \
    do {                                                                                     \
      extern FILE *log_fp;                                                                   \
      extern bool log_enable();                                                              \
      if (log_enable())                                                                      \
      {                                                                                      \
        strncpy(instr_ringbuf[ringbuf_end++ % RINGBUF_LINES], _this->logbuf, PRINTBUF_SIZE); \
        ;                                                                                    \
      }                                                                                      \
    } while (0))
#endif

static void trace_and_difftest(Decode *_this, vaddr_t dnpc)
{
  // printf("%s\n",_this->logbuf);
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND)
  {
    log_write("%s\n", _this->logbuf);
    ir_write("%s\n", _this->logbuf);
  }
#endif
  if (g_print_step)
  {
    IFDEF(CONFIG_ITRACE, puts(_this->logbuf));
  }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
#ifdef CONFIG_ITRACE_COND
  if (nemu_state.state == NEMU_ABORT)
    ringbuf_display();
#endif
#ifdef CONFIG_FTRACE
  static int call_level = 0;
  if (functab_head)
  {printf("%s\n",_this->logbuf);
    // ret pseudo, jalr x0, 0(x1)
    char *jalr1 = strstr(_this->logbuf, "jalr");
    char *jal = strstr(_this->logbuf, "jal");
    if (jalr1)
    {
      functab_node *funcitem = functab_find(_this->pc);
      functab_node *funcitem2 = functab_find(dnpc);
      log_write("0x%08X:", _this->pc);
      for (int i = 0; i < call_level; ++i)
        log_write(" ");
      log_write("ret  [%s] <- [%s]\n", funcitem2 ? funcitem2->name : "???", funcitem ? funcitem->name : "???");
      call_level--;
    }
    // call - jal ra, imm or jalr ra, $x
    if ((jalr1 || jal) && 1)
    {
      functab_node *funcitem = functab_find(dnpc);
      functab_node *funcitem2 = functab_find(_this->pc);
      log_write("0x%08X:", _this->pc);
      call_level++;
      for (int i = 0; i < call_level; ++i)
        log_write(" ");
      log_write("call [%s] -> [%s]\n", funcitem2 ? funcitem2->name : "???", funcitem ? funcitem->name : "???");
    }
  }

#endif
#ifdef CONFIG_WATCHPOINT
  wp_state = hook();
  if (wp_state)
  {
    nemu_state.state = NEMU_STOP;
  }
#endif
}

static void exec_once(Decode *s, vaddr_t pc)
{
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i--)
  {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0)
    space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
              MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);

#endif
}

static void execute(uint64_t n)
{
  Decode s;
  for (; n > 0; n--)
  {
    exec_once(&s, cpu.pc);

    g_nr_guest_inst++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING)
      break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

static void statistic()
{
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0)
    Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else
    Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg()
{
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n)
{
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state)
  {
  case NEMU_END:
  case NEMU_ABORT:
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  default:
    nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state)
  {
  case NEMU_RUNNING:
    nemu_state.state = NEMU_STOP;
    break;

  case NEMU_END:

  case NEMU_ABORT:

    Log("nemu: %s at pc = " FMT_WORD,
        (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) : (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
        nemu_state.halt_pc);
    // fall through
#ifdef CONFIG_ITRACE
    if (nemu_state.state == NEMU_ABORT && nemu_state.state != NEMU_STOP && nemu_state.halt_ret != 0)
    {
      ringbuf_display();
    }
#endif
  case NEMU_QUIT:
    // #ifdef CONFIG_ITRACE
    //     if (nemu_state.state != NEMU_STOP && nemu_state.state != NEMU_QUIT)
    //     {
    //       ringbuf_display();
    //     }
    // #endif
    statistic();
  }
}
