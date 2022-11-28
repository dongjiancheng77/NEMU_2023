#include <sys/time.h>
#include <stdio.h>
int main()
{
  int count = 0;
  struct timeval old_tv, cur_tv;
  struct timezone tz;
  unsigned long old_usec, cur_usec;
  gettimeofday(&old_tv, &tz);
  old_usec = old_tv.tv_sec * 1000000 + old_tv.tv_usec;
  while (count < 10)
  {

    gettimeofday(&cur_tv, &tz);
    unsigned long cur_usec = cur_tv.tv_sec * 1000000 + cur_tv.tv_usec;
    // printf("tick0 %ld\n", cur_usec);
    if (cur_usec - old_usec > 500000)
    {
      old_usec = cur_usec;
      printf("tick %d\n", count);
      count++;
      printf("tick1 %d\n", count);
    }
  }
  return 0;
}
