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
  int kb_code = read_key();
  int key_down = (kb_code & 0x8000);
  kb_code &= (~0x8000);
  Log("%d", kb_code);
  if (kb_code != _KEY_NONE)
    sprintf(buf, "k%s %s\n", (key_down ? "d" : "u"), keyname[kb_code]);
  else
    sprintf(buf, "t %d\n", uptime());
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  strncpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  int width = screen_width();
  int x = offset / 4 % width;
  int y = offset / 4 / width;
  draw_rect((uint32_t *)buf, x, y, len / 4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len)
{
  draw_sync();
  return len;
}

void init_device()
{
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
