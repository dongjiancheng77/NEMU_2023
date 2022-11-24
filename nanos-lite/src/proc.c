#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j++;
    yield();
  }
}

void init_proc()
{
  switch_boot_pcb();

  Log("Initializing processes...");

  naive_uload(NULL, NULL);
  // load program here
}

Context *schedule(Context *prev)
{
  return NULL;
}
int execve(const char *pathname, char *const argv[], char *const envp[])
{
  // // char *envp1[] = {NULL};
  // // printf("execve pathname is %s\n", pathname);
  // int fd = fs_open(pathname, 0, 0);
  // if (fd == -1)
  // {
  //   return -1;
  // }
  // else
  //   fs_close(fd);

  // // char* pathname2 = "/bin/pal";
  // // printf("brk is %p\n",heap_brk);
  // // naive_uload(NULL, pathname);
  // // if (argv != NULL)
  // //   for (int i = 0; argv[i] != NULL; ++i)
  // //     printf("argv%d is %s\n", i, argv[i]);
  // // if (envp != NULL)
  // //   for (int i = 0; envp[i] != NULL; ++i)
  // //     printf("envp%d is %s\n", i, envp[i]);

  // // printf("envp %s\n", envp[0]);
  // current->cp = context_uload(current, (char *)pathname, argv, envp);
  // NR_PROC--;
  // assert(NR_PROC >= 0 && NR_PROC <= MAX_NR_PROC);
  // switch_boot_pcb();
  // // printf("yield for execve\n");
  // // printf("return ctx address is %p\n", pcb[1].cp);
  // // pre_process();
  // yield();
  // // printf("return address is %p\n", pcb[1].cp);
  // // schedule(NULL);

  return 0;
}
void exit(int status)
{
  // if (status == 0)
  // {
  //   execve("/bin/nterm", NULL, NULL);
  // }
  // else
    halt(status);
}