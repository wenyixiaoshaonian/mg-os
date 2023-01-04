#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */

#define is_digit(c)	((c) >= '0' && (c) <= '9')

void puts(char *s) {
  for (; *s; s++) putch(*s);
}

static int skip_atoi(const char **s) // 将连续的字符串转成整数
{
	int i=0;

	while (is_digit(**s)) // 判断是否为数字
		i = i*10 + *((*s)++) - '0';// 转数字
	return i;
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

#define do_div(n,base) ({ \
int __res; \
__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })

static char * number(char * str, int num, int base, int size, int precision
	,int type) // 转换字符串
{
	char c,sign,tmp[36];
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type&SMALL) digits="0123456789abcdefghijklmnopqrstuvwxyz"; // 用小写字母
	if (type&LEFT) type &= ~ZEROPAD;//按位取反
	if (base<2 || base>36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ' ;//补零
	if (type&SIGN && num<0) {//判断负数
		sign='-';
		num = -num;
	} else
		sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);//是否有‘+’ 或 ‘ ’
	if (sign) size--;

	if (type&SPECIAL) // 判断特殊字符
		if (base==16) size -= 2;
		else if (base==8) size--;
	    i=0;
	if (num==0)
		tmp[i++]='0';
	else while (num!=0)
		tmp[i++]=digits[do_div(num,base)];//把数存到数组中
	if (i>precision) precision=i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT))) // 是否补零或左对齐
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type&SPECIAL)//判断特殊字符
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type&LEFT)) // 左对齐
		while(size-->0)
			*str++ = c;
	while(i<precision--) // 补零
		*str++ = '0';
	while(i-->0)
		*str++ = tmp[i];
	while(size-->0)
		*str++ = ' ';
	return str;//返回字符串
}
//https://github.com/TAOYUCODE/vsprintf/blob/master/vsprintf.c
int vsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	int i;
	char * str;
	char *s;
	int *ip;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	for (str=buf ; *fmt ; ++fmt) {//遍历字符串
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}
			
		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}
		
		/* get field width */
		field_width = -1;
		if (is_digit(*fmt)) // 判断数字字符
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			/* it's the next argument */
			field_width = va_arg(args, int);//获取参数值
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;//左对齐
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (is_digit(*fmt))//判断数字字符
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		switch (*fmt) { //判断数据类型
		case 'c': // 字符型
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			break;

		case 's': // 字符串
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";
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

		case 'o': // 八进制输出
			str = number(str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			break;

		case 'p': //指针
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			break;

		case 'x': //十六进制
			flags |= SMALL;
		case 'X': //字母大写的十六进制 
			str = number(str, va_arg(args, unsigned long), 16,
				field_width, precision, flags);
			break;

		case 'd': //十进制整形
		case 'i':
			flags |= SIGN;
		case 'u': //无符号整形
			str = number(str, va_arg(args, unsigned long), 10,
		    field_width, precision, flags);
			break;

		case 'n': //字符个数
			ip = va_arg(args, int *);
			*ip = (str - buf);
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
	*str = '\0';//结束
	return str-buf;//返回字符个数
}

//https://github.com/TAOYUCODE/vsprintf/blob/master/vsprintf.c
int sprintf(char * buf, const char *fmt, ...) // 第一个参数是写入一个字符串， 第二个是写入格式， 后面的...是参数占位符
{
	va_list args;
	int i; //记录函数的返回值

	va_start(args, fmt); // 初始化va_list, 得到第一个参数的地址
	i=vsprintf(buf,fmt,args); // 调用vsprintf()  
	va_end(args); // 将args置空
	return i;//返回值
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  const int ret = vsnprintf(out, n, fmt, va);
  va_end(va);
  return ret;
}

//https://github.com/MrBad/vsnprintf/blob/master/vsnprintf.c
#define is_digit(c) (c >= '0' && c <= '9')

static int get_atoi(const char **str)
{
    int n;
    for (n = 0; is_digit(**str); (*str)++)
        n = n * 10 + **str - '0';
    return n;
}

static void bputc(char *buf, size_t *pos, size_t max, char c)
{
    if (*pos < max)
        buf[(*pos)] = c;
    (*pos)++;
}

#define F_ALTERNATE 0001    // put 0x infront 16, 0 on octals, b on binary
#define F_ZEROPAD   0002    // value should be zero padded
#define F_LEFT      0004    // left justified if set, otherwise right justified
#define F_SPACE     0010    // place a space before positive number
#define F_PLUS      0020    // show +/- on signed numbers, default only for -
#define F_SIGNED    0040    // is an unsigned number?
#define F_SMALL     0100    // use lowercase for hex?

/**
 * Formats an integer number
 *  buf - buffer to print into
 *  len - current position in buffer
 *  maxlen - last valid position in buf
 *  num - number to print
 *  base - it's base
 *  width - how many spaces this should have; padding
 *  flags - above F flags
 */
static void fmt_int(char *buf, size_t *len, size_t maxlen,
        long long num, int base, int width, int flags)
{
    char nbuf[64], sign = 0;
    char altb[8]; // small buf for sign and #
    unsigned long n = num;
    int npad;           // number of pads
    char pchar = ' ';   // padding character
    char *digits = "0123456789ABCDEF";
    char *ldigits = "0123456789abcdef";
    int i, j;

    if (base < 2 || base > 16)
        return;
    if (flags & F_SMALL) digits = ldigits;
    if (flags & F_LEFT) flags &= ~F_ZEROPAD;

    if ((flags & F_SIGNED) && num < 0) {
        n = -num;
        sign = '-';
    } else if (flags & F_PLUS) {
        sign = '+';
    } else if (flags & F_SPACE)
        sign = ' ';

    i = 0;
    do {
        nbuf[i++] = digits[n % base];
        n = n / base;
    } while (n > 0);
   
    j = 0;
    if (sign) altb[j++] = sign;
    if (flags & F_ALTERNATE) {
        if (base == 8 || base == 16) {
            altb[j++] = '0';
            if (base == 16)
                altb[j++] = (flags & F_SMALL) ? 'x' : 'X';
        }
    }
    altb[j] = 0;

    npad = width > i + j ? width - i - j : 0;

    if (width > i + j)
        npad = width - i - j;

    if (npad > 0 && ((flags & F_LEFT) == 0)) {
        if (flags & F_ZEROPAD) {
            for (j = 0; altb[j]; j++)
                bputc(buf, len, maxlen, altb[j]);
            altb[0] = 0;
        }
        while (npad-- > 0)
            bputc(buf, len, maxlen, (flags & F_ZEROPAD) ? '0' : ' ');
    }
    for (j = 0; altb[j]; j++)
        bputc(buf, len, maxlen, altb[j]);

    while (i-- > 0)
        bputc(buf, len, maxlen, nbuf[i]);

    if (npad > 0 && (flags & F_LEFT))
        while(npad-- > 0)
            bputc(buf, len, maxlen, pchar);
}

static void fmt_chr(char *buf, size_t *pos, size_t max, char c,
        int width, int flags)
{
    int npad = 0;
    if (width > 0) npad = width - 1;
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            bputc(buf, pos, max, ' ');

    bputc(buf, pos, max, c);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            bputc(buf, pos, max, ' ');
}

/**
 * strlen()
 */
static size_t slen(char *s)
{
    size_t i;
    for (i = 0; *s; i++, s++)
        ;
    return i;
}

static void fmt_str(char *buf, size_t *pos, size_t max, char *s,
        int width, int flags)
{
    int npad = 0;
    if (width > 0) npad = width - slen(s);
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            bputc(buf, pos, max, ' ');

    while (*s)
        bputc(buf, pos, max, *s++);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            bputc(buf, pos, max, ' ');
}

/* Format states */
#define S_DEFAULT   0
#define S_FLAGS     1
#define S_WIDTH     2
#define S_PRECIS    3
#define S_LENGTH    4
#define S_CONV      5

/* Lenght flags */
#define L_CHAR      1
#define L_SHORT     2
#define L_LONG      3
#define L_LLONG     4
#define L_DOUBLE    5

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    size_t n = 0;
    char c, *s;
    char state = 0;
    long long num;
    int base;
    int flags, width, precision, lflags;

    if (!buf) size = 0;

    for (;;) {
        c = *fmt++;
        if (state == S_DEFAULT) {
            if (c == '%') {
                state = S_FLAGS;
                flags = 0;
            } else {
                bputc(buf, &n, size, c);
            }
        } else if (state == S_FLAGS) {
            switch (c) {
                case '#': flags |= F_ALTERNATE; break;
                case '0': flags |= F_ZEROPAD; break;
                case '-': flags |= F_LEFT; break;
                case ' ': flags |= F_SPACE; break;
                case '+': flags |= F_PLUS; break;
                case '\'':
                case 'I' : break; // not yet used
                default: fmt--; width = 0; state = S_WIDTH;
            }
        } else if (state == S_WIDTH) {
            if (c == '*') {
                width = va_arg(ap, int);
                if (width < 0) {
                    width = -width;
                    flags |= F_LEFT;
                }
            } else if (is_digit(c) && c > '0') {
                fmt--;
                width = get_atoi(&fmt);
            } else {
                fmt--;
                precision = -1;
                state = S_PRECIS;
            }
        } else if (state == S_PRECIS) {
            // Ignored for now, but skip it
            if (c == '.') {
                if (is_digit(*fmt))
                    precision = get_atoi(&fmt);
                else if (*fmt == '*')
                    precision = va_arg(ap, int);
                precision = precision < 0 ? 0 : precision;
            } else
                fmt--;
            lflags = 0;
            state = S_LENGTH;
        } else if (state == S_LENGTH) {
            switch(c) {
                case 'h': lflags = lflags == L_CHAR ? L_SHORT : L_CHAR; break;
                case 'l': lflags = lflags == L_LONG ? L_LLONG : L_LONG; break;
                case 'L': lflags = L_DOUBLE; break;
                default: fmt--; state = S_CONV;
            }
        } else if (state == S_CONV) {
            if (c == 'd' || c == 'i' || c == 'o' || c == 'b' || c == 'u' 
                    || c == 'x' || c == 'X') {
                if (lflags == L_LONG)
                    num = va_arg(ap, long);
                else if (lflags & (L_LLONG | L_DOUBLE))
                    num = va_arg(ap, long long);
                else if (c == 'd' || c == 'i')
                    num = va_arg(ap, int);
                else
                    num = (unsigned int) va_arg(ap, int);

                base = 10;
                if (c == 'd' || c == 'i') {
                    flags |= F_SIGNED;
                } else if (c == 'x' || c == 'X') {
                    flags |= c == 'x' ? F_SMALL : 0;
                    base = 16;
                } else if (c == 'o') {
                    base = 8;
                } else if (c == 'b') {
                    base = 2;
                }
                fmt_int(buf, &n, size, num, base, width, flags);
            } else if (c == 'p') {
                num = (long) va_arg(ap, void *);
                base = 16;
                flags |= F_SMALL | F_ALTERNATE;
                fmt_int(buf, &n, size, num, base, width, flags);
            } else if (c == 's') {
                s = va_arg(ap, char *);
                if (!s)
                    s = "(null)";
                fmt_str(buf, &n, size, s, width, flags);
            } else if (c == 'c') {
                c = va_arg(ap, int);
                fmt_chr(buf, &n, size, c, width, flags);
            } else if (c == '%') {
                bputc(buf, &n, size, c);
            } else {
                bputc(buf, &n, size, '%');
                bputc(buf, &n, size, c);
            }
            state = S_DEFAULT;
        }
        if (c == 0)
            break;
    }
    n--;
    if (n < size)
        buf[n] = 0;
    else if (size > 0)
        buf[size - 1] = 0;

    return n;
}

#endif
