#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <sys/time.h>
void exit(int status);
int mm_brk(uintptr_t brk);
int execve(const char *pathname, char *const argv[], char *const envp[]);
void sys_execve(Context *c)
{
  const char *fname = (const char *)c->GPR2;
  char **argv = (char **)c->GPR3;
  char **envp = (char **)c->GPR4;
  c->GPRx = execve(fname, argv, envp);
  // naive_uload(NULL, fname);
  // c->GPRx = 0;
}

void sys_brk(Context *c)
{
  uintptr_t addr = (uintptr_t)(c->GPR2);
  c->GPRx = mm_brk(addr);
  // c->GPRx = 0;
}

static inline intptr_t sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
  uint64_t uptime = io_read(AM_TIMER_UPTIME).us;

  tv->tv_sec = uptime / 1000000;
  tv->tv_usec = uptime % 1000000; // according to man, usec ranges [0, 999999]
  if (tz)
  {
    tz->tz_dsttime = 0;
    tz->tz_minuteswest = 0;
  }
  return 0;
}
void do_syscall(Context *c)
{
  // printf("dd");
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
// #ifdef CONFIG_STRACE
  Log("System call trace\nmcause\t\tGPR1\t\tGPR2\t\tGPR3\t\tGPR4 \n%d\t\t%d\t\t%d\t\t%x\t\t%d", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
// #endif
  switch (a[0])
  {
  case SYS_yield:
    // printf("dd");
    yield();
    c->GPRx = 0;
    break;
  case SYS_exit:
    // printf("dd1");
    exit(0);
    // printf("dd");
    // c->GPRx = 0;
    // naive_uload(NULL, "/bin/nterm");
    // c->GPRx = 0;
    break;
  case SYS_brk:
    sys_brk(c);
    break;
  case SYS_execve:
    sys_execve(c);
    break;
  case SYS_open:
    c->GPRx = fs_open((char *)a[1], a[2], a[3]);
    break;
  case SYS_close:
    c->GPRx = fs_close(a[1]);
    break;
  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], a[2], a[3]);
    break;
  case SYS_write:
    // TODO();
    // printf("11");
    // if (c->GPR2 == 1 || c->GPR2 == 2){
    //   for (int i = 0; i < c->GPR4; ++i){
    //     putch(*(((char *)c->GPR3) + i));
    //   }
    //   c->GPRx = c->GPR4;
    // }
    int fd = c->GPR2;
    char *buf = (char *)c->GPR3;
    int count = c->GPR4;
    if (fd == 1 || fd == 2)
    {
      for (int i = 0; i < count; i++)
      {
        putch(*buf++);
      }
    }
    else
    {
      count = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
    }
    c->GPRx = count;
    // char *x = (char *)a[2];
    // for (int i = 0; i < a[3]; i++)
    //   putch(*x++);
    break;
  case SYS_gettimeofday:
    c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
