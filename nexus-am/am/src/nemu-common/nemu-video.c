#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

int screen_width();

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t vinfo = inl(SCREEN_ADDR);
      info->width = ((vinfo & 0xFFFF0000) >> 16);
      info->height = (vinfo & 0xFFFF);
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      } else if (ctl->w * ctl->h > 0) {
        for (int y = ctl->y; y < ctl->y + ctl->h; y++) {
          for (int x = ctl->x; x < ctl->x + ctl->w; x++) {
            *((uint32_t*)(FB_ADDR + sizeof(uint32_t) * (y * screen_width() + x))) 
              = ctl->pixels[(y - ctl->y) * ctl->w + (x - ctl->x)];
          }
        }
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
