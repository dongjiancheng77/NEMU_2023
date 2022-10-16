#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
// #include <types.h>
// libs
// #include <string.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define BUF_LEN 256

// itoa()函数把整数转换成字符串，并返回指向转换后的字符串的指针。

char *itoa(int value, char *str, int radix)
{
  char reverse[36];
  char *p = reverse;
  bool sign = (value >= 0) ? true : false;

  value = (value >= 0) ? value : -value;
  *p++ = '\0';
  while (value >= 0)
  {
    *p++ = "0123456789abcdef"[value % radix];
    value /= radix;
    if (value == 0)
      break;
  }

  if (!sign)
  {
    *p = '-';
  }
  else
  {
    p--;
  }

  while (p >= reverse)
  {
    *str++ = *p--;
  }

  return str;
}
char *uitoa(uint32_t value, char *str, int radix)
{
  char reverse[36];
  char *p = reverse;

  *p++ = '\0';
  while (value != 0)
  {
    *p++ = "0123456789abcdef"[value % radix];
    value /= radix;
    if (value == 0)
      break;
  }
  p--;

  while (p >= reverse)
  {
    *str++ = *p--;
  }

  return str;
}
int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  char buffer[128];
  char *str;
  int val, len;
  unsigned int unum;
  uint32_t pointer;
  bool state = true;
  int i = 0, j = 0, k = 0;
  while (fmt[i] != '\0' && j + 1 < n)
  {
    if (state)

    {
      if (fmt[i] != '%')
      {
        out[j++] = fmt[i];
      }
      else
        state = false;
    }
    else
    {

      switch (fmt[i])
      {

      case 'c':
        out[j++] = va_arg(ap, int);
        break;
      case 's':
        str = va_arg(ap, char *);
        k = 0;
        while (str[k] != '\0')
        {
          out[j++] = str[k++];
        }
        break;

      case 'd':
        val = va_arg(ap, int);
        if (val == 0)
        {
          out[j++] = '0';
          break;
        }
        if (val < 0)
        {
          val = (-1) * val;
          out[j++] = '-';
        }
        for (len = 0; val; val /= 10, ++len)
          buffer[len] = val % 10 + '0';
        for (int k = len - 1; k >= 0; --k)
          out[j++] = buffer[k];

        // itoa(val,str,10);
        // for (; val; val /= 10)
        // j++;
        break;

      case 'p':
        pointer = va_arg(ap, uint32_t);
        for (len = 0; pointer; pointer /= 16, ++len)
          buffer[len] = "0123456789ABCDEF"[pointer % 16];
        for (int k = 0; k < 8 - len; ++k)
          out[j++] = '0';

        for (int k = len - 1; k >= 0; --k)
          out[j++] = buffer[k];
        break;

      case 'x':
        unum = va_arg(ap, unsigned int);
        // if (unum == 0)
        // {
        //   out[j++] = '0';
        //   break;
        // }
        // for (len = 0; unum; unum >>= 4, ++len)
        //   buffer[len] = "0123456789ABCDEF"[unum & 0xF]; //逆序的

        // for (int k = len - 1; k >= 0; --k)
        //   out[j++] = buffer[k];
        uitoa(unum, str, 10);
        k = 0;
        while (str[k] != '\0')
        {
          out[j++] = str[k++];
        }
        break;

      default:
        assert(0);
      }
      state = true;
    }
    i++;
  }

  out[j] = '\0';
  return j;
}

// vsprintf return 要打印出来的字符串的长度
int vsprintf(char *out, const char *fmt, va_list ap)
{
  return vsnprintf(out, -1, fmt, ap);
}
int printf(const char *fmt, ...)
{
  char buffer[256];
  va_list arg;
  va_start(arg, fmt);

  int i = vsprintf(buffer, fmt, arg);

  putstr(buffer);

  va_end(arg);
  return i;
}
int sprintf(char *out, const char *fmt, ...)
{
  va_list valist;
  va_start(valist, fmt);

  int res = vsprintf(out, fmt, valist);
  va_end(valist);
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  return 0;
}

#endif