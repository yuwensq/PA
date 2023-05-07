#include "fs.h"

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset; // 这里如果定义到里面，多个程序同时读写文件会咋样？
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
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

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    {"stdin", 0, 0, invalid_read, invalid_write},
    {"stdout", 0, 0, invalid_read, invalid_write},
    {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs()
{
  // TODO: initialize the size of /dev/fb
}

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 3; i < NR_FILES; i++)
  {
    if (strcmp(file_table[i].name, pathname) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(false);
}

size_t fs_read(int fd, void *buf, size_t len)
{
  int paddr = file_table[fd].disk_offset + file_table[fd].open_offset;
  int end = file_table[fd].disk_offset + file_table[fd].size;
  size_t real_len = ((paddr + len <= end) ? len : (end - paddr));
  assert(real_len >= 0);
  ramdisk_read(buf, paddr, real_len);
  file_table[fd].open_offset += real_len;
  return real_len;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  if (fd == 1 || fd == 2)
  {
    for (int i = 0; i < len; i++)
      _putc(((char *)buf)[i]);
    return len;
  }
  int paddr = file_table[fd].disk_offset + file_table[fd].open_offset;
  int end = file_table[fd].disk_offset + file_table[fd].size;
  size_t real_len = ((paddr + len <= end) ? len : (end - paddr));
  assert(real_len >= 0);
  ramdisk_write(buf, paddr, real_len);
  file_table[fd].open_offset += real_len;
  return real_len;
}

static size_t update_open_offset(int fd, size_t new_offset)
{
  int end = file_table[fd].size;
  if (new_offset < 0 || new_offset > end)
    return -1;
  return new_offset;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  size_t res_offset = 0;
  switch (whence)
  {
  case SEEK_SET:
    res_offset = update_open_offset(fd, offset);
    break;
  case SEEK_CUR:
    res_offset = update_open_offset(fd, file_table[fd].open_offset + offset);
    break;
  case SEEK_END:
    res_offset = update_open_offset(fd, file_table[fd].size + offset);
    break;
  default:
    res_offset = -1;
    break;
  }
  if (res_offset == -1)
    return -1;
  return (file_table[fd].open_offset = res_offset);
}

int fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}