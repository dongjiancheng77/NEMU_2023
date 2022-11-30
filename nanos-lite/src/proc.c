#include <proc.h>
#include <fs.h>
#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}
int program_index = 1;
void switch_program_index(int new_index)
{
  if (new_index == program_index)
    return;

  switch_boot_pcb();

  program_index = new_index;
  pcb[0].cp->pdir = NULL;
  // TODO: 这是一种trade-off
  // set_satp(pcb[1].cp->pdir);
  // printf("Switch to PCB[%d]\n", new_index);

  yield();
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

  naive_uload(NULL, "/bin/nterm");
  // load program here
}

Context *schedule(Context *prev)
{
  return NULL;
}
void context_uload(PCB *ptr_pcb, const char *filename, char *const argv[], char *const envp[]);

int execve(const char *pathname, char *const argv[], char *const envp[])
{
  if (fs_open(pathname, 0, 0) == -1)
  { // 文件不存在
    return -1;
  }
  printf("Loading from %s ...\n", pathname);
  context_uload(&pcb[program_index], pathname, argv, envp);
  switch_boot_pcb();

  pcb[0].cp->pdir = NULL;
  // TODO: 这是一种trade-off
  // set_satp(pcb[1].cp->pdir);
  printf("PCB[0] pdir: %p cp: %p\n", pcb[0].cp->pdir, pcb[0].cp);

  yield();
  return 0;
}
void exit(int status)
{
  if (status == 0)
  {
    execve("/bin/nterm", NULL, NULL);
  }
  else
    halt(status);
}