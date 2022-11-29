#include <am.h>
#include <nemu.h>
#include <stdio.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init()
{
  // // int i;
  // const uint32_t vgainfo = inl(VGACTL_ADDR);
  // w = vgainfo >> 16;
  // h = (vgainfo << 16) >> 16;
  // // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // // for (i = 0; i < w * h; i++)
  // //   fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{
  uint32_t vgainfo = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = (vgainfo >> 16), .height = (vgainfo << 16) >> 16, .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{
  if (ctl->w != 0 && ctl->h != 0)
  {
    int win_weight = io_read(AM_GPU_CONFIG).width;
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    uint32_t *p = (uint32_t *)(uintptr_t)ctl->pixels;
    for (int i = 0; i < ctl->h; ++i)
    {
      for (int j = 0; j < ctl->w; ++j)
      {
        fb[(ctl->y) * win_weight + i * win_weight + ctl->x + j] = p[i * (ctl->w) + j];
      }
    }
  }
  if (ctl->sync)
  {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
void __am_gpu_memcpy(AM_GPU_MEMCPY_T *gpu_memcpy) {
  int i;
  const char* src = (char *)gpu_memcpy->src;
  for (i = 0; i < gpu_memcpy->size; ++ i) {
    outb(FB_ADDR + i + gpu_memcpy->dest, *(src + i));
  }
  outl(SYNC_ADDR, 1);
}