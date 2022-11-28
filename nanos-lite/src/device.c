#include <common.h>
// #include <fs.h>
#include <proc.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [AM_KEY_NONE] = "NONE",
    AM_KEYS(NAME)};
int fg_pcb;
size_t serial_write(const void *buf, size_t offset, size_t len)
{
  // return 0;
  char *str = (char *)buf;
  for (size_t i = 0; i < len; ++i)
  {
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len)
{
  // printf("!");
  AM_INPUT_KEYBRD_T kbd_in = io_read(AM_INPUT_KEYBRD);
  if (kbd_in.keycode == AM_KEY_NONE)
    return 0;
  switch (kbd_in.keycode)
  {
  case AM_KEY_F1:
    switch_program_index(1);
    return 0;
  case AM_KEY_F2:
    switch_program_index(2);
    return 0;
  case AM_KEY_F3:
    switch_program_index(3);
    return 0;
  }
  snprintf(buf, len, "%s %s\n", kbd_in.keydown ? "kd" : "ku", keyname[kbd_in.keycode]);
  return 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  // return 0;
  AM_GPU_CONFIG_T fbctl = io_read(AM_GPU_CONFIG);
  int _len = snprintf(buf, len, "WIDTH : %d\n HEIGHT : %d\n", fbctl.width, fbctl.height);
  return _len;
}
static AM_GPU_CONFIG_T gpu_config;
static AM_GPU_FBDRAW_T gpu_fbdraw;
size_t fb_write(const void *buf, size_t offset, size_t len)
{
  if (len == 0)
  {
    // printf("end!\n");
    gpu_fbdraw.sync = 1;
    gpu_fbdraw.w = 0;
    gpu_fbdraw.h = 0;
    ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
    return 0;
  }

  // ioe_read(AM_GPU_CONFIG, &gpu_config);
  int width = gpu_config.width;
  // printf("screen is %d\n", width);

  gpu_fbdraw.pixels = (void *)buf;
  gpu_fbdraw.w = len;
  gpu_fbdraw.h = 1;
  gpu_fbdraw.x = offset % width;
  gpu_fbdraw.y = offset / width;
  // printf("w,h is %d,%d\n",gpu_fbdraw.w,gpu_fbdraw.h);
  gpu_fbdraw.sync = 0;
  ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
  return len;
}

void init_device()
{
  Log("Initializing devices...");
  ioe_init();
}
