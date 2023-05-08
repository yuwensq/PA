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
    {"/dev/events", 0, 0, 0, events_read, invalid_write},
    {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
    {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
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
  assert(false);
}

size_t fs_read(int fd, void *buf, size_t len)
{
  if (file_table[fd].read != NULL)
    return file_table[fd].read(buf, 0, len);
  int paddr = file_table[fd].disk_offset + file_table[fd].open_offset;
  int end = file_table[fd].disk_offset + file_table[fd].size;
  size_t real_len = ((paddr + len <= end) ? len : (end - paddr));
  assert(real_len >= 0);
  ramdisk_read(buf, paddr, real_len);
  file_table[fd].open_offset += real_len;
  return real_len;
}

size_t fs_write(int fd,const void *buf,size_t len){
  assert(fd<NR_FILES);
  //printf("fd is %d\n",fd);
  size_t lens=len;
  if(file_table[fd].size&&file_table[fd].open_offset+len>file_table[fd].size){
      lens=file_table[fd].size-file_table[fd].open_offset;
  }
  if(file_table[fd].write==NULL){
    lens=ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,lens); 
    file_table[fd].open_offset+=lens;
  }
  else{
    lens=file_table[fd].write(buf,file_table[fd].open_offset,lens);
    file_table[fd].open_offset+=lens;
  }
  return lens;
}
// size_t fs_write(int fd, const void *buf, size_t len)
// {
//   if (file_table[fd].write != NULL)
//     return file_table[fd].write(buf, 0, len);
//   int paddr = file_table[fd].disk_offset + file_table[fd].open_offset;
//   int end = file_table[fd].disk_offset + file_table[fd].size;
//   size_t real_len = ((paddr + len <= end) ? len : (end - paddr));
//   assert(real_len >= 0);
//   ramdisk_write(buf, paddr, real_len);
//   file_table[fd].open_offset += real_len;
//   return real_len;
// }

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