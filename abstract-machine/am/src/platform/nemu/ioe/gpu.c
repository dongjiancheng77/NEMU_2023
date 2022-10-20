#include <am.h>
#include <nemu.h>
#include <stdio.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)
static int w;
static int h;
void __am_gpu_init()
{
  // int i;
  const uint32_t vgainfo = inl(VGACTL_ADDR);
  w = vgainfo >> 16;
  h = (vgainfo << 16) >> 16;
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i++)
  //   fb[i] = i;
  outl(SYNC_ADDR, 1);

}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{

  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = w, .height = h, .vmemsz = h * w};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{
const uint32_t* pixel = (uint32_t*)ctl->pixels;
	uint32_t* fb = (uint32_t*)(uintptr_t)FB_ADDR;

	for (int i = 0; i < ctl->h; ++i)
	{
		for (int j = 0; j < ctl->w; ++j)
		{
			const uint32_t pOffset = i * ctl->w + j;
			const uint32_t baseOffset = ((ctl->y + i) * w + (ctl->x + j));
			fb[baseOffset] = pixel[pOffset];
		}
	}
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
