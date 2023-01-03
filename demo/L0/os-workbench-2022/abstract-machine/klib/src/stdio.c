#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void puts(char *s) {
  for (; *s; s++) putch(*s);
}

//https://blog.csdn.net/u012252959/article/details/53761360
int printf(const char *fmt, ...) {
  char printf_buf[1024];
  va_list args;                                        /* args为定义的一个指向可变参数的变量，va_list以及下边要用到的va_start,va_end都是是在定义可变参数函数中必须要用到宏， 在stdarg.h头文件中定义 */
  int printed;
  va_start(args, fmt);                               //初始化args的函数，使其指向可变参数的第一个参数，fmt是可变参数的前一个参数

  printed = vsprintf(printf_buf, fmt, args);
  va_end(args);                                                             //结束可变参数的获取
  puts(printf_buf);
  return printed;
  // return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  int len;
  int i;
  char * str;
  char *s;
  int *ip;

  int flags;

  int field_width;
  int precision;
  int qualifier;

  for (str=out ; *fmt ; ++fmt) {
    if (*fmt != '%') {
      *str++ = *fmt;
      continue;
    }


    flags = 0;
    repeat:
    ++fmt;
    switch (*fmt) {
      case '-': flags |= LEFT; goto repeat;
      case '+': flags |= PLUS; goto repeat;
      case ' ': flags |= SPACE; goto repeat;
      case '#': flags |= SPECIAL; goto repeat;
      case '0': flags |= ZEROPAD; goto repeat;
    }


    field_width = -1;
    if (is_digit(*fmt))
      field_width = skip_atoi(&fmt);
    else if (*fmt == '*') {
      field_width = va_arg(args, int);
      if (field_width < 0) {
        field_width = -field_width;
        flags |= LEFT;
      }
    }

    precision = -1;
    if (*fmt == '.') {
      ++fmt;
    if (is_digit(*fmt))
      precision = skip_atoi(&fmt);
    else if (*fmt == '*') {
      precision = va_arg(args, int);
    }
    if (precision < 0)
      precision = 0;
    }

    qualifier = -1;
    if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
      qualifier = *fmt;
      ++fmt;
    }

    switch (*fmt) {
      case 'c':
        if (!(flags & LEFT))
        while (--field_width > 0)
        *str++ = ' ';
        *str++ = (unsigned char) va_arg(args, int);
        while (--field_width > 0)
        *str++ = ' ';
        break;
      case 's':
        s = va_arg(args, char *);
        len = strlen(s);
        if (precision < 0)
        precision = len;
        else if (len > precision)
        len = precision;

        if (!(flags & LEFT))
        while (len < field_width--)
        *str++ = ' ';
        for (i = 0; i < len; ++i)
        *str++ = *s++;
        while (len < field_width--)
        *str++ = ' ';
        break;
      case 'o':
        str = number(str, va_arg(args, unsigned long), 8,field_width, precision, flags);
        break;
      case 'p':
        if (field_width == -1) {
          field_width = 8;
          flags |= ZEROPAD;
        }
        str = number(str,(unsigned long) va_arg(args, void *), 16,field_width, precision, flags);
        break;
      case 'x':
        flags |= SMALL;
      case 'X':
        str = number(str, va_arg(args, unsigned long), 16,field_width, precision, flags);
        break;
      case 'd':
      case 'i':
        flags |= SIGN;
      case 'u':
        str = number(str, va_arg(args, unsigned long), 10,field_width, precision, flags);
      break;
      case 'n':
        ip = va_arg(args, int *);
        *ip = (str - out);
        break;
      default:
        if (*fmt != '%')
          *str++ = '%';
        if (*fmt)
          *str++ = *fmt;
        else
          --fmt;
        break;
    }
  }
  *str = '\0';
  return str-out;
}

int sprintf(char *out, const char *fmt, ...) {
  // puts("sprintf Not implemented");
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  // puts("snprintf Not implemented");
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  // puts("vsnprintf Not implemented");
  return 0;
}

#endif
