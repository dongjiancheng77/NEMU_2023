#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  size_t len = 0;
  while (s[len] != '\0')
  {
    len++;
  }

  return len;
}

char *strcpy(char *dst, const char *src)
{
  int i = 0;
  for (; src[i] != '\0'; ++i)
  {
    dst[i] = src[i];
  }
  dst[i] = '\0';

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
  int i = 0;
  for (; i < n && src[i] != '\0'; ++i)
  {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  // for (; i < n; ++i)
  // {
  //   dst[i] = '\0';
  // }

  return dst;
}
// C 库函数 char *strcat(char *dest, const char *src) 把 src 所指向的字符串追加到 dest 所指向的字符串的结尾。
char *strcat(char *dst, const char *src)
{
  int i, j;
  for (i = 0; dst[i] != '\0'; ++i)
  {
  }
  for (j = 0; src[j] != '\0'; ++j)
  {
    dst[i + j] = src[j];
  }
  dst[i + j] = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2)
{
  int i = 0;
  for (;; ++i)
  {
    if (s1[i] < s2[i])
      return -1;
    if (s1[i] > s2[i])
      return 1;
    if (s1[i] == '\0' && s2[i] != '\0')
      return -1;
    if (s1[i] != '\0' && s2[i] == '\0')
      return 1;
    if (s1[i] == '\0' && s2[i] == '\0')
      return 0;
  }
  panic("strcmp!!");
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  {
    if (!n)
      return 0;
    while (--n && *s1 && *s1 == *s2)
    {
      s1++;
      s2++;
    }
    return *s1 - *s2;
    panic("strncmp!!Not implemented");
  }
}
void *memset(void *s, int c, size_t n)
{
  unsigned char *target = (unsigned char *)s;
  for (int i = 0; i < n; ++i)
  {
    target[i] = (unsigned char)c;
  }
  return s;
  panic("memset!!Not implemented");
}
// C 库函数 void *memmove(void *str1, const void *str2, size_t n) 从 str2 复制 n 个字符到 str1，
// 但是在重叠内存块这方面，memmove() 是比 memcpy() 更安全的方法。如果目标区域和源区域有重叠的话，
// memmove() 能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中，复制后源区域的内容会被更改。
// 如果目标区域与源区域没有重叠，则和 memcpy() 函数功能相同。
void *memmove(void *dst, const void *src, size_t n)
{
  char *char_dst = (char *)dst;
  char *char_src = (char *)src;

  if (dst < src)
  {
    for (int i = 0; i < n; ++i)
      char_dst[i] = char_src[i];
  }
  else if (dst > src)
  {
    for (int i = n - 1; i >= 0; --i)
      char_dst[i] = char_src[i];
  } 

  return dst;
  panic("memmove!!Not implemented");
}

void *memcpy(void *out, const void *in, size_t n)
{
  char *char_out = (char *)out;
  const char *char_in = (const char *)in;
  for (int i = 0; i < n; ++i)
  {
    char_out[i] = char_in[i];
  }

  return out;
  panic("memcpy!!Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  char *char_s1 = (char *)s1;
  char *char_s2 = (char *)s2;

  int i = 0;
  for (; i < n; ++i)
  {
    if (char_s1[i] != char_s2[i])
      return (int)(char_s1[i]) - (int)(char_s2[i]);
    if (i == n - 1)
      return 0;
  }

  return (int)(char_s1[i]) - (int)(char_s2[i]);
  panic("memcmp!!Not implemented");
}

#endif
