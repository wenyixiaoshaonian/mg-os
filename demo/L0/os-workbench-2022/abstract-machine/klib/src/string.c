#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  char* start = s;
	char* end = NULL;
	while (*s != '\0')
	{
		s++;
		end = s;
	}
	return end - start;

}

char *strcpy(char *dst, const char *src) {
  assert(dst && src);
	char* tmp = dst;
	while ((*dst++ = *src++)!= '\0')
	{
		;
	}
	return tmp;
}

char *strncpy(char *dst, const char *src, size_t n) {
  assert(dst && src);
	char* tmp = dst;
	while (n-- && (*dst++ = *src++)!= '\0')
	{
		;
	}
	return tmp;
}

char *strcat(char *dst, const char *src) {
  char* ret = dst;
	assert(dst && src);
	//找到目的字符串的'\0'
	while (*dst != '\0') {
		dst++;
	}
	//追加
    while (*dst++ = *src++) {
        ;
    }
	return ret;
}

int strcmp(const char *s1, const char *s2) {
	int res;
 
	while (1) {
		if ((res = *s1 - *s2++) != 0 || !*s1++)
			break;
	}
	return res;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	int res;
  	while (n--) {
		if ((res = *s1 - *s2++) != 0 || !*s1++)
			break;
	}
	return res;
}

void *memset(void *s, int c, size_t n) {
  while(n--) {
	*(int *)s++ = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  
	assert(dst && src);
	void* ret = dst;
	if (src > dst)
	{
		while (n--)
		{
			*(char*)dst = *(char*)src;
			(char*)dst++;
			(char*)src++;
		}
	}
	else
	{
		while (n--)
		{
			*((char*)dst + n) = *((char*)src + n);
		}
	}
	return ret;
}

void *memcpy(void *out, const void *in, size_t n) {
    void * ret = out;
	while (n-- > 0) 
		*(char *)out++ = *(char *)in++;
	return ret;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1 != NULL && s2 != NULL && n > 0);

    const char *buf1 = (const char*)s1;
    const char *buf2 = (const char*)s2;

    while (n--) 
    {
        if (*buf1++ != *buf2++)
        {
            return buf1[-1] < buf2[-1] ? -1 : 1;
        }
            
    }
    return 0;
}

#endif
