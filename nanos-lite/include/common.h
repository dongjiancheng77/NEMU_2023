#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
//#define HAS_VME
//#define MULTIPROGRAM
//#define TIME_SHARING

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <debug.h>
size_t ramdisk_read(void *buf, size_t offset, size_t len); 
size_t ramdisk_write(const void *buf, size_t offset, size_t len); 
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len) ;
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
int fs_open(const char *pathname, int flags, int mode);
#endif
