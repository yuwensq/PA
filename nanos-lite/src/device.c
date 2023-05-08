#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len)
{
  for (int i = 0; i < len; i++)
    _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [_KEY_NONE] = "NONE",
    _KEYS(NAME)};

size_t events_read(void *buf, size_t offset, size_t len)
{
  _DEV_INPUT_KBD_t kbd_event;
  int kb_res = _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &kbd_event, 0);
  if (kb_res != 0 && kbd_event.keycode != _KEY_NONE)
  {
    ((char *)buf)[0] = 'k';
    if (kbd_event.keydown)
      ((char *)buf)[1] = 'd';
    else
      ((char *)buf)[1] = 'u';
    ((char *)buf)[2] = ' ';
    printf("%d", kbd_event.keycode);
    memcpy(buf + 3, keyname[kbd_event.keycode], strlen(keyname[kbd_event.keycode]));
    int read_size = 3 + strlen(keyname[kbd_event.keycode]);
    ((char *)buf)[read_size++] = '\n';
    return read_size;
  }
  // _DEV_TIMER_UPTIME_t time_event;
  // int ti_res = _io_read(_DEV_TIMER, _DEVREG_TIMER_UPTIME, &time_event, 0);
  // if (ti_res != 0)
  // {
  //   ((char *)buf)[0] = 't';
  //   ((char *)buf)[1] = ' ';
  //   char up_time[16];
  //   sprintf(up_time, "%u", time_event.lo);
  //   memcpy(buf + 2, up_time, strlen(up_time));
  //   int read_size = 2 + strlen(up_time);
  //   ((char *)buf)[read_size++] = '\n';
  //   return read_size;
  // }
  ((char *)buf)[0] = '\n';
  return 1;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len)
{
  return 0;
}

void init_device()
{
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
