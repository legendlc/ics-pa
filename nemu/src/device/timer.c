#include "device/map.h"
#include "monitor/monitor.h"
#include <sys/time.h>

#define RTC_PORT 0x48   // Note that this is not the standard
#define RTC_MMIO 0xa1000048

void timer_intr() {
  if (nemu_state.state == NEMU_RUNNING) {
    extern void dev_raise_intr(void);
    dev_raise_intr();
  }
}

static uint32_t *rtc_port_base = NULL;
static struct timeval boot_time = {0};

void rtc_io_handler(uint32_t offset, int len, bool is_write) {
  assert(offset == 0);
  if (!is_write) {
    struct timeval now;
    gettimeofday(&now, NULL);
    uint32_t seconds = now.tv_sec;
    uint32_t useconds = now.tv_usec;
    rtc_port_base[0] = (seconds - boot_time.tv_sec) * 1000 + (useconds - boot_time.tv_usec) / 1000;
  }
}

void init_timer() {
  rtc_port_base = (void*)new_space(4);
  gettimeofday(&boot_time, NULL);
  add_pio_map("rtc", RTC_PORT, (void *)rtc_port_base, 4, rtc_io_handler);
  add_mmio_map("rtc", RTC_MMIO, (void *)rtc_port_base, 4, rtc_io_handler);
}
