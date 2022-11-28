#include <sys/time.h>
#include <stdio.h>
#include <NDL.h>
int main()
{
  int count = 0;
  // struct timeval old_tv, cur_tv;
  // struct timezone tz;
  // unsigned long old_usec, cur_usec;
  // gettimeofday(&old_tv, &tz);
  // old_usec = old_tv.tv_sec * 1000000 + old_tv.tv_usec;
  // uint32_t sec = 0;
  // uint32_t usec = 0;
  NDL_Init(0);
  unsigned int old_ms = NDL_GetTicks();
  unsigned int cur_ms = 0;
  while (count < 32)
  {

    // gettimeofday(&cur_tv, &tz);
    // unsigned long cur_usec = cur_tv.tv_sec * 1000000 + cur_tv.tv_usec;
    // // printf("tick0 %ld\n", cur_usec);
    // if (cur_usec - old_usec > 500000)
    // {
    //   old_usec = cur_usec;
    //   printf("tick %d\n", count);
    //   count++;
    //   printf("tick1 %d\n", count);
    // }
    cur_ms = NDL_GetTicks();
    if (cur_ms - old_ms > 500)
    {
      old_ms = cur_ms;
      printf("ndl tick %d\n", count);
      count++;
    }
  }
  return 0;
}
