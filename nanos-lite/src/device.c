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

  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
