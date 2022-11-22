#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *c)
{
  printf("__am_irq_handle 1 c->pdir内容地址修改前 页表项:%p\t上下文地址%p\t所在栈帧:%p\n", c->pdir, c, &c);
  printf("设置c->np为%d\n", c->np);
  if (user_handler)
  {
    Event ev = {0};
    switch (c->mcause)
    {
    case (8):
      // printf("c->GPR1 = %d \n", c->GPR1);
      if (c->GPR1 == -1)
      { // 特指-1
        ev.event = EVENT_YIELD;
      }
      else
      {
        ev.event = EVENT_SYSCALL;
      }
      c->mepc += 4;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  printf("__am_irq_handle 2 c->pdir内容地址修改后 页表项:%p\t上下文地址%p\t所在栈帧:%p\n", c->pdir, c, &c);

  printf("c->np为%d\n", c->np);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *))
{
  // initialize exception entry
  asm volatile("csrw mtvec, %0"
               :
               : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  return NULL;
}

void yield()
{
  asm volatile("li a7, -1; ecall");
}

bool ienabled()
{
  return false;
}

void iset(bool enable)
{
}
