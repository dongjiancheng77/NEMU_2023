#include <common.h>

static Context *do_event(Event e, Context *c)
{
  switch (e.event)
  {
  case EVENT_YIELD:
    printf("EVENT_YIELD\n");
    // Log("EVENT_YIELD\n");
    // return schedule(c);
    // case EVENT_SYSCALL:
    //   do_syscall(c);
    //   break;

    // case EVENT_IRQ_TIMER:
    //   Log("EVENT_IRQ_TIMER happening...");
    //   // break;
    //   return schedule(c);

  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
