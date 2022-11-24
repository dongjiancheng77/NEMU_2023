#include <common.h>
#include "syscall.h"
#include <fs.h>
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

void do_syscall(Context *c)
{
  // printf("dd");
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  printf("STRACE: [#%d]( %d, %d, %d )\n", a[0], a[1], a[2], a[3]);
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
  // case SYS_brk
  //   sys_brk(c);
  //   break;
  // case SYS_execve:
  //   sys_execve(c);
  //   break;
  case SYS_write:
    // TODO();
    printf("11");
    // c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
    char *x = (char *)a[2];
    for (int i = 0; i < a[3]; i++)
      putch(*x++);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
