#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
// #include <types.h>
// libs
// #include <string.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define BUF_LEN 256
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

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  char buffer[128];
char *str;
  int val, len;
  unsigned int unum;
  uint32_t pointer;
  bool state = true;
  int i = 0, j = 0;
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
        int k = 0;
        while (str[k] != '\0')
        {
          out[j++] = str[k++];
        }
        break;

      case 'd':
        val = va_arg(ap, int);
        // if (val == 0)
        // {
        //   out[j++] = '0';
        //   break;
        // }
        // if (val < 0)
        // {
        //   val = (-1) * val;
        //   out[j++] = '-';
        // }
        // for (len = 0; val; val /= 10, ++len)
        //   buffer[len] = val % 10 + '0';
        // for (int k = len - 1; k >= 0; --k)
        //   out[j++] = buffer[k];

          itoa(val,str,10);
          for (; val; val /= 10)
          j++;
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
        if (unum == 0)
        {
          out[j++] = '0';
          break;
        }
        for (len = 0; unum; unum >>= 4, ++len)
          buffer[len] = "0123456789ABCDEF"[unum & 0xF]; //逆序的

        for (int k = len - 1; k >= 0; --k)
          out[j++] = buffer[k];
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

// itoa()函数把整数转换成字符串，并返回指向转换后的字符串的指针。


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

char *gcvt(double value, int ndigit, char *buf)
{
  char tmpbuf[72];
  int int_part = (int)value;

  double folat_part = value - int_part;

  if (folat_part < 0)
    folat_part = -folat_part;
  itoa(int_part, tmpbuf, 10);

  char *p = tmpbuf;

  while (*p != '\0')
    p++;

  *p++ = '.';

  while (ndigit > 0 && folat_part > 0.00000001)
  {
    *p++ = (int)(folat_part * 10) + '0';
    folat_part = (folat_part * 10) - (int)(folat_part * 10);
    ndigit--;
  }

  *p = '\0';
  strcpy(buf, tmpbuf);
  return buf;
}
int vsprintf(char *out, const char *fmt, va_list ap)
{
  return vsnprintf(out, -1, fmt, ap);

  // char *j;
  // // char cha;
  // va_list p_next_arg = ap;
  // for (j = out; *fmt; fmt++)
  // {
  //   if (*fmt != '%')
  //   {
  //     *j++ = *fmt;
  //     continue;
  //   }
  //   fmt++; // *fmt = '%'
  //   switch (*fmt)
  //   {
  //   case 'd':
  //     itoa(va_arg(p_next_arg, int), j, 10);
  //     j += strlen(j);
  //     break;
  //   case 'x':
  //     uitoa(va_arg(p_next_arg, unsigned int), j, 16);
  //     j += strlen(j);
  //     break;
  //   case 'c':
  //     *j++ = (char)va_arg(ap, int);
  //     break;
  //   case 's':
  //     *j = '\0';
  //     strcat(j, va_arg(p_next_arg, char *));
  //     j += strlen(j);
  //     break;
  //   case 'f':
  //     gcvt(va_arg(p_next_arg, double), 6, j);
  //     j += strlen(j);
  //     break;
  //   default:
  //     break;
  //   }
  // }
  // *j = '\0';
  // return strlen(j);
}
int printf(const char *fmt, ...)
{
  int i;
  char buf[BUF_LEN];

  va_list arg;
  va_start(arg, fmt);
  i = vsprintf(buf, fmt, arg);
  // write(i,buf,  BUF_LEN);
  // it can't be used
  putstr(buf);
  va_end(arg);
  return i;
}

// vsprintf return 要打印出来的字符串的长度

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
  va_list arg;
  va_start(arg, fmt);

  int done = vsnprintf(out, n, fmt, arg);

  va_end(arg);
  return done;
}

#endif