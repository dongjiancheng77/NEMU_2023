#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB,
  FD_EVENTS,
  FD_DISPINFO
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
size_t fb_write(const void *buf, size_t offset, size_t len);
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
    [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
    [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
    [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs()
{
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T fbctl = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].disk_offset = 0;
  file_table[FD_FB].size = fbctl.width * fbctl.height * sizeof(uint32_t);
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++)
  {
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  Finfo *info = &file_table[fd];
  size_t real_len;

  // assert(info->open_offset + len <= info->size);
  if (info->read)
  {
    real_len = info->read(buf, info->open_offset, len);
    info->open_offset += real_len;
  }
  else
  {
    real_len = info->open_offset + len <= info->size ? len : info->size - info->open_offset;
    ramdisk_read(buf, info->disk_offset + info->open_offset, real_len);
    info->open_offset += real_len;
  }

  return real_len;
  // if (file_table[fd].read)
  // {
  //   len = file_table[fd].read(buf, file_table[fd].open_offset, len);
  //   file_table[fd].open_offset += len;
  //   return len;
  // }
  // // fallback to ramdisk read
  // if (file_table[fd].open_offset - file_table[fd].disk_offset + len > file_table[fd].size)
  // {
  //   len = file_table[fd].size + file_table[fd].disk_offset - file_table[fd].open_offset;
  // }
  // Log("file_table[fd].open_offset: %d, disk_off: %d, size: %d, len: %d", file_table[fd].open_offset, file_table[fd].disk_offset,file_table[fd].size, len);
  // ramdisk_read(buf, file_table[fd].open_offset, len);
  // file_table[fd].open_offset += len;
  // return len;
}

size_t fs_write(int fd, void *buf, size_t len)
{
  if (file_table[fd].write)
  {
    len = file_table[fd].write(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
  // if (file_table[fd].open_offset - file_table[fd].disk_offset + len > file_table[fd].size)
  // {
  //   len = file_table[fd].size + file_table[fd].disk_offset - file_table[fd].open_offset;
  // }
  ramdisk_write(buf, file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  Finfo *temp = &file_table[fd];
  switch (whence)
  {
  case SEEK_SET:
    temp->open_offset = offset;
    break;
  case SEEK_CUR:
    temp->open_offset += offset;
    break;
  case SEEK_END:
    temp->open_offset = temp->size + offset;
    break;
  default:
    assert(0);
    break;
  }
  return temp->open_offset;
}

int fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}