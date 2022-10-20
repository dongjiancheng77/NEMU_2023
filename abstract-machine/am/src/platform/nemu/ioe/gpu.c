#include <am.h>
#include <nemu.h>
#include <stdio.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)
static int W;
static int H;
void __am_gpu_init()
{
  int i;
  int w = 0; // TODO: get the correct width
  int h = 0; // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i++)
    fb[i] = i;
  outl(SYNC_ADDR, 1);
  const uint32_t vgainfo = inl(VGACTL_ADDR);
  W = vgainfo >> 16;
  H = (vgainfo << 16) >> 16;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{

  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = W, .height = H, .vmemsz = H * W};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{
const uint32_t* pixel = (uint32_t*)ctl->pixels;
	uint32_t* base = (uint32_t*)(uintptr_t)FB_ADDR;

	for (int i = 0; i < ctl->h; ++i)
	{
		for (int j = 0; j < ctl->w; ++j)
		{
			const uint32_t pOffset = i * ctl->w + j;
			const uint32_t baseOffset = ((ctl->y + i) * W + (ctl->x + j));
			base[baseOffset] = pixel[pOffset];
		}
	}
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
