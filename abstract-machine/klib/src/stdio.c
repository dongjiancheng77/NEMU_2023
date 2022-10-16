#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <fcntl.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int vsprintf(char *out, const char *fmt, va_list ap);
int vsnprintf(char *out, size_t n, const char *fmt, va_list ap);
int printf(const char *fmt, ...) 
{ 
    int i; 
    char buf[256]; 
 
    va_list arg ; 
    va_start (arg, fmt);
    i = vsprintf(buf, fmt, arg); 
  putstr(buf);
   va_end(arg);
    return i; 
} 

static char HEX_CHARACTERS[] = "0123456789ABCDEF";
#define BIT_WIDE_HEX 8

//vsprintf return 要打印出来的字符串的长度
int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, -1, fmt, ap);
}
// char* itoa(int value, char *str, int radix){
//     char reverse[36];   
//     char *p = reverse;
//     bool sign = (value >= 0)?true:false;

//     value = (value >= 0)?value:-value;
//     *p++ = '\0';
//     while (value >= 0){
//         *p++ = "0123456789abcdef"[value%radix];
//         value /= radix;
//         if (value == 0) break;
//     }

//     if (!sign) {
//         *p = '-';
//     }
//     else {
//         p--;
//     }

//     while (p >= reverse){
//         *str++ = *p--;
//     }

//     return str;
// }
// int vsprintf(char *buf, const char *fmt, va_list args)
// {
//     char* p;
//     char tmp[256];
//     va_list p_next_arg = args;
 
//     for (p = buf; *fmt; fmt++)
//     {
//         if (*fmt != '%')
//         {
//             *p++ = *fmt;
//             continue;
//         }
 
//         fmt++;
 
//         switch (*fmt)
//         {
//             case 'x':
//                 itoa(*((int*)p_next_arg),tmp, 10);
//                 strcpy(p, tmp);
//                 p_next_arg += 4;
//                 p += strlen(tmp);
//                 break;
//             case 's':
//                 break;
//             default:
//                 break;
//         }
//     }
   
//     return (p - buf);
// } 
int sprintf(char *out, const char *fmt, ...) {
  va_list valist;
  va_start(valist, fmt);

  int res = vsprintf(out ,fmt, valist);
  va_end(valist);
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list arg;
  va_start (arg, fmt);

  int done = vsnprintf(out, n, fmt, arg);

  va_end(arg);
  return done;
}

#define append(x) {out[j++]=x; if (j >= n) {break;}}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char buffer[128];
  char *txt, cha;
  int num, len;
  unsigned int unum;
  uint32_t pointer;
  
  
  int state = 0, i, j;//模仿一个状态机
  for (i = 0, j = 0; fmt[i] != '\0'; ++i){
    switch (state)
    {
    case 0:
      if (fmt[i] != '%'){
        append(fmt[i]);
      } else
        state = 1;
      break;
    
    case 1:
      switch (fmt[i])
      {
      case 's':
        txt = va_arg(ap, char*);
        for (int k = 0; txt[k] !='\0'; ++k)
          append(txt[k]);
        break;
      
      case 'd':
        num = va_arg(ap, int);
        if(num == 0){
          append('0');
          break;
        }
        if (num < 0){
          append('-');
          num = 0 - num;
        }
        for (len = 0; num ; num /= 10, ++len)
          //buffer[len] = num % 10 + '0';//逆序的
          buffer[len] = HEX_CHARACTERS[num % 10];//逆序的
        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;
      
      case 'c':
        cha = (char)va_arg(ap, int);
        append(cha);
        break;

      case 'p':
        pointer = va_arg(ap, uint32_t);
        for (len = 0; pointer ; pointer /= 16, ++len)
          buffer[len] = HEX_CHARACTERS[pointer % 16];//逆序的
        for (int k = 0; k < BIT_WIDE_HEX - len; ++k)
          append('0');

        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;

      case 'x':
        unum = va_arg(ap, unsigned int);
        if(unum == 0){
          append('0');
          break;
        }
        for (len = 0; unum ; unum >>= 4, ++len)
          buffer[len] = HEX_CHARACTERS[unum & 0xF];//逆序的

        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;  

      default:
        assert(0);
      }
      state = 0;
      break;
      
    }
  }

  out[j] = '\0';
  return j;
}

#endif
