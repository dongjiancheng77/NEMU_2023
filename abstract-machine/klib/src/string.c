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
  return dst;
}
// C 库函数 char *strcat(char *dest, const char *src) 把 src 所指向的字符串追加到 dest 所指向的字符串的结尾。
char *strcat(char *dst, const char *src)
{
  char *addr = dst;

  while (*dst)
    dst++;
  do
    *dst++ = *src++;
  while (*src); // end by zero
  return addr;
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
  uint8_t *temp = (uint8_t *)s;

  for (; n != 0; n--)
    *temp++ = c;
  return s;
}
void *memcpy(void *out, const void *in, size_t n)
{
  const uint8_t *sp = (const uint8_t *)in;
  uint8_t *dp = (uint8_t *)out;

  for (; n != 0; n--)
    *dp++ = *sp++;
  return out;
  panic("memcpy!!Not implemented");
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

int memcmp(const void *s1, const void *s2, size_t n)
{
  size_t i = 0;
  while(((char*)s1)[i] == ((char *)s2)[i] && i < n)
  {
    i++;
  }
  return i == n? 0:((char*)s1)[i] - ((char *)s2)[i];
  
}

#endif
