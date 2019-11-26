#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; i++) {
    _putc(((char*)buf)[i]);
  }

  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  _DEV_INPUT_KBD_t kbd = {0};
  _DEV_TIMER_UPTIME_t uptime = {0};

  assert(buf);

  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &kbd, sizeof(kbd));
  if (kbd.keycode != _KEY_NONE) {
    if (kbd.keydown) {
      sprintf(buf, "kd %s\n", keyname[kbd.keycode]);
    } else {
      sprintf(buf, "ku %s\n", keyname[kbd.keycode]);
    }
  } else {
    _io_read(_DEV_TIMER, _DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
    sprintf(buf, "t %d\n", uptime.lo);
  }

  assert(strlen(buf) < len);
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  assert(buf && offset <= strlen(dispinfo) && strlen(dispinfo + offset) < len);
  strcpy(buf, dispinfo + offset);

  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  assert(offset % 4 == 0 && len % 4 == 0);
  size_t row_len = screen_width() * 4;
  size_t row_offset = offset % row_len;
  assert(row_offset + len <= row_len); // TODO: now only support draw one row

  int x = row_offset / 4;
  int y = offset / row_len;
  int w = len / 4;
  int h = 1;
  draw_rect((uint32_t*)buf, x, y, w, h);

  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
