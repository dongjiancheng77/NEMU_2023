#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
// #include <types.h>
// libs
// #include <string.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define BUF_LEN 256
#define append(x) \
  {               \
    out[j++] = x; \
    if (j >= n)   \
    {             \
      break;      \
    }             \
  }

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  char buffer[128];
  char *txt, cha;
  int num, len;
  unsigned int unum;
  uint32_t pointer;
  bool state = true;
  int i = 0, j = 0;
  for (; fmt[i] != '\0'; ++i)
  {
    if (state)

    {
      if (fmt[i] != '%')
      {
        append(fmt[i]);
      }
      else
        state = false;
    }
    else
    {
      switch (fmt[i])
      {
      case 's':
        txt = va_arg(ap, char *);
        for (int k = 0; txt[k] != '\0'; ++k)
          append(txt[k]);
        break;

      case 'd':
        num = va_arg(ap, int);
        if (num == 0)
        {
          append('0');
          break;
        }
        if (num < 0)
        {
          append('-');
          num = -num;
        }
        for (len = 0; num; num /= 10, ++len)
          buffer[len] = num % 10 + '0';
        // buffer[len] = HEX_CHARACTERS[num % 10]; 
        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;

      case 'c':
        cha = (char)va_arg(ap, int);
        append(cha);
        break;

      case 'p':
        pointer = va_arg(ap, uint32_t);
        for (len = 0; pointer; pointer /= 16, ++len)
          buffer[len] = "0123456789ABCDEF"[pointer % 16]; 
        for (int k = 0; k < 8 - len; ++k)
          append('0');

        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;

      case 'x':
        unum = va_arg(ap, unsigned int);
        if (unum == 0)
        {
          append('0');
          break;
        }
        for (len = 0; unum; unum >>= 4, ++len)
          buffer[len] = "0123456789ABCDEF"[unum & 0xF]; //逆序的

        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;

      default:
        assert(0);
      }
      state = true;
    }
  }

  out[j] = '\0';
  return j;
}

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

/* NOT WORK TODO */
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