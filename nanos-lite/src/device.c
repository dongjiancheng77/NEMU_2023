#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static AM_GPU_FBDRAW_T gpu_fbdraw;
static AM_GPU_CONFIG_T gpu_config;

static const char *keyname[256] __attribute__((used)) = {
    [AM_KEY_NONE] = "NONE",
    AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t events_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  // return 0;
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
