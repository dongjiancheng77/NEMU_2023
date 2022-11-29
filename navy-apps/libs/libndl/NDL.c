#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int dispinfo_dev = -1, screen_w = 0, screen_h = 0, canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks()
{
  // return 0;
  static struct timeval timeval;
  static struct timezone timezone;
  int ret = gettimeofday(&timeval, &timezone);
  return timeval.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len)
{
  buf[0] = '\0';
  return read(evtdev, buf, len);
}

void NDL_OpenCanvas(int *w, int *h)
{
  if (getenv("NWM_APP"))
  {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1)
    {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }
    close(fbctl);
  }
  if (*h == 0 && *w == 0)
  {
    canvas_h = screen_h;
    canvas_w = screen_w;
    *w = screen_w;
    *h = screen_h;
  }
  else if (*h <= screen_h && *w <= screen_w)
  {
    canvas_h = *h;
    canvas_w = *w;
  }
  // printf("%d,%d a/n", *w, *h);
  // printf("%d,%d b/n", canvas_w, canvas_h);
  // printf("%d,%d c/n", screen_w, screen_h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h)
{
  // printf("%d,%d d/n", w, h);
  y += (screen_h - canvas_h) / 2;
  size_t offset_0 = (y * screen_w + x + (screen_w - canvas_w) / 2) * sizeof(uint32_t);
  int ret_seek = 0, ret_write;
  // printf("%d \n", ret_seek);
  for (int j = 0; j < h; ++j)
  {
    // printf("%d \n ", ret_seek);
    ret_seek = lseek(fbdev, offset_0 + j * screen_w * sizeof(uint32_t), SEEK_SET);
    ret_write = write(fbdev, pixels + j * w, w * sizeof(uint32_t));
  }
}

void NDL_OpenAudio(int freq, int channels, int samples)
{
}

void NDL_CloseAudio()
{
}

int NDL_PlayAudio(void *buf, int len)
{
  return 0;
}

int NDL_QueryAudio()
{
  return 0;
}

int NDL_Init(uint32_t flags)
{
  if (getenv("NWM_APP"))
  {
    evtdev = 3;
  }
  evtdev = open("/dev/events", O_RDONLY);
  fbdev = open("/dev/fb", O_RDONLY);
  dispinfo_dev = open("/proc/dispinfo", O_RDONLY);
  char buf[128];
  read(dispinfo_dev, buf, 128);
  char *buf0 = strtok(buf, ":");
  char *buf1 = strtok(NULL, "\n");
  if (strcmp(buf0, "WIDTH") == 0)
  {
    screen_w = atoi(buf1);
  }
  else
  {
    screen_h = atoi(buf1);
  }
  char *buf2 = strtok(NULL, ":");
  char *buf3 = strtok(NULL, "\n");
  if (strcmp(buf2, "WIDTH") == 0)
  {
    screen_w = atoi(buf3);
  }
  else
  {
    screen_h = atoi(buf3);
  }
  return 0;
}

void NDL_Quit()
{
  close(dispinfo_dev);
}
