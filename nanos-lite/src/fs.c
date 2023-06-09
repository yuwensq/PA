#include "fs.h"

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset; // 这里如果定义到里面，多个程序同时读写文件会咋样？
  ReadFn read;
  WriteFn write;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB,
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t fbsync_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    {"stdin", 0, 0, 0, invalid_read, invalid_write},
    {"stdout", 0, 0, 0, invalid_read, serial_write},
    {"stderr", 0, 0, 0, invalid_read, serial_write},
    {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
    {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
    {"/dev/events", 0, 0, 0, events_read, invalid_write},
    {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
    {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs()
{
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = screen_height() * screen_width() * 4;
}

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 0; i < NR_FILES; i++)
  {
    if (strcmp(file_table[i].name, pathname) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  Log("%s", pathname);
  assert(false);
}

size_t fs_read(int fd, void *buf, size_t len)
{
  size_t read_len = len;
  size_t real_len = 0;
  if (file_table[fd].size && file_table[fd].open_offset + len > file_table[fd].size)
    read_len = file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].read != NULL)
    real_len = file_table[fd].read(buf, file_table[fd].open_offset, read_len);
  else
    real_len = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_len);
  file_table[fd].open_offset += real_len;
  return real_len;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  size_t write_len = len;
  size_t real_len = 0;
  if (file_table[fd].size && file_table[fd].open_offset + len > file_table[fd].size)
    write_len = file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].write != NULL)
    real_len = file_table[fd].write(buf, file_table[fd].open_offset, write_len);
  else
    real_len = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, write_len);
  file_table[fd].open_offset += real_len;
  return real_len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  default:
    return -1;
  }
  if (file_table[fd].open_offset > file_table[fd].size)
    file_table[fd].open_offset = file_table[fd].size;
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}