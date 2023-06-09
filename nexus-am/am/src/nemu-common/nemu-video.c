#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

#define W 400
#define H 300

size_t __am_video_read(uintptr_t reg, void *buf, size_t size)
{
  switch (reg)
  {
  case _DEVREG_VIDEO_INFO:
  {
    _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
    info->width = 400;
    info->height = 300;
    return sizeof(_DEV_VIDEO_INFO_t);
  }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size)
{
  switch (reg)
  {
  case _DEVREG_VIDEO_FBCTL:
  {
    _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      
    uint32_t *vga_buffer = (uint32_t *)(uintptr_t)FB_ADDR;
    int bytes_per_row  = sizeof(uint32_t) * (ctl->x + ctl->w > W ? W - ctl->x : ctl->w);

    for (int j = 0; j < ctl->h && ctl->y + j < H; j++)
    {
      memcpy(vga_buffer + (ctl->y + j) * W + ctl->x, ctl->pixels + ctl->w * j, bytes_per_row);
    }

    if (ctl->sync)
    {
      outl(SYNC_ADDR, 0);
    }
    return size;
  }
  }
  return 0;
}

void __am_vga_init()
{
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i++)
    fb[i] = i;
  draw_sync();
}
