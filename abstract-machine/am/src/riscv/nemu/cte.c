#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *c)
{
  // TODO:
  // printf("页表项:%p\t上下文地址%p\t栈帧:%p\n", c->pdir, c, &c);
  if (user_handler)
  {
    Event ev = {0};
    printf("c->GPR1 = %d \n", c->GPR1);
    printf("c->mcause = %d \n", c->mcause);
    switch (c->mcause)
    {

    case -1:

      ev.event = EVENT_YIELD;
      c->mepc += 4;
      break;
    case 0 ... 19:
      ev.event = EVENT_SYSCALL;

      c->mepc += 4;
      break;

    default:
      ev.event = EVENT_ERROR;
      break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
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
