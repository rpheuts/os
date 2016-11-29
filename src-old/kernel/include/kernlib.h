#ifndef __KERNLIB_H
#define __KERNLIB_H

#include "types.h"

extern char* strncpy(char * __restrict dst, const char * __restrict src, size_t n);
extern char* strcat(char * __restrict s, const char * __restrict append);
extern u_long random();
extern int strcmp(char* s1, char* s2);
extern int strncmp(char* s1, char* s2, size_t n);
extern char* strstr(char* s, char* find);
extern size_t strlen(const char *str);
extern char * strsep(char **stringp, const char *delim);
extern void int2str(int a, char *s);
extern void int2hex(int a, char *s);
extern char hex2Nibble(char n);
extern int hex2int (char *s);
extern int str2int(char* digit, int* result);
extern void kprintf(const char* fmt, ...);
extern int ffs(int mask);
extern int ffsl(long mask);
extern int fls(int mask);
extern int flsl(long mask);

extern uint32_t crc32_tab[];

static __inline uint32_t crc32_raw(const void *buf, size_t size, uint32_t crc)
{
	const uint8_t *p = (const uint8_t *)buf;

	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
	return (crc);
}

static __inline uint32_t crc32(const void *buf, size_t size)
{
	uint32_t crc;

	crc = crc32_raw(buf, size, ~0U);
	return (crc ^ ~0U);
}

static __inline void *memset(void *dest, char val, size_t count)
{
	char *temp = (char *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

static __inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	unsigned short *temp = (unsigned short *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

static __inline void *memcpy(void *dest, const void *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for(; count != 0; count--) *dp++ = *sp++;
	return dest;
}

static __inline int bcd(int dec)
{
	return ((dec/10)<<4)+(dec%10);
}

static __inline int unbcd(int bcd)
{
	return ((bcd>>4)*10)+bcd%16;
} 

#endif
