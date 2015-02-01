/*
[Module printf.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/03
*/

#include <stdarg.h>
#include <internal/stdio.h>
#include <stdio.h>
#include <system.h>
#include <string.h>
#include <spinlock.h>

unsigned short* textmemptr = (unsigned short*) 0xFFFFFFFF800B8000;
static volatile unsigned char printf_lock;

static void scroll(void) {

	textmemptr = (unsigned short*) 0xFFFFFFFF800B8000;
	unsigned short *newaddr = textmemptr + 80;
	memcpy(textmemptr, newaddr, 80 * 25 * 2);

}

static void print(unsigned char c, int x, int y, int attr) {

	unsigned short *where;
	unsigned att = attr << 8;
	where = textmemptr + (y * 80 + x);
	*where = c | att;
}

int x = 0;
int y = 0;
void AcpiOsVprintf(const char *format, va_list args) {

	//vprintf(format, args);

}

int vprintf(const char *format, va_list args) {

	char buf[256];

	int ret = vsprintf(buf, format, args);


	unsigned char *c = (unsigned char*) buf;
	while(*c) {
		if(*c == '\n') {
			x = 0;
			y++;
			c++;
			if(y >= 25) {
				y = 24;
				scroll();
			}
			continue;
		}
		print(*c, x++, y, (0x07 & 0x0F));
		if(x == 80) {
			x = 0;
			y++;
		}
		if(y >= 25) {
			y = 24;
			scroll();
		}
		c++;
	}
	int cur_pos = x + y * 80;
	outb(0x3D4, 15);
	outb(0x3D5, cur_pos);
	outb(0x3D4, 14);
	outb(0x3D5, cur_pos >> 8);

	return ret;
}


int kkprintf(const char *format, ...) {



	char buf[1024];
	va_list args;
	va_start(args, format);

	int ret = vsprintf(buf, format, args);

	va_end(args);

	unsigned char *c = (unsigned char*) buf;
	while(*c) {
		if(*c == '\n') {
			x = 0;
			y++;
			c++;
			if(y >= 25) {
				y = 24;
				scroll();
			}
			continue;
		}
		print(*c, x++, y, (0x07 & 0x0F));
		if(x == 80) {
			x = 0;
			y++;
		}
		if(y >= 25) {
			y = 24;
			scroll();
		}
		c++;
	}
	int cur_pos = x + y * 80;

	outb(0x3D4, 15);
	outb(0x3D5, cur_pos);
	outb(0x3D4, 14);
	outb(0x3D5, cur_pos >> 8);

	writes_serial(buf);

	return ret;
}

int kprintf(const char *format, ...) {


	spin_lock(&printf_lock);

	char buf[1024];
	va_list args;
	va_start(args, format);

	int ret = vsprintf(buf, format, args);

	va_end(args);

	unsigned char *c = (unsigned char*) buf;
	while(*c) {
		if(*c == '\n') {
			x = 0;
			y++;
			c++;
			if(y >= 25) {
				y = 24;
				scroll();
			}
			continue;
		}
		print(*c, x++, y, (0x07 & 0x0F));
		if(x == 80) {
			x = 0;
			y++;
		}
		if(y >= 25) {
			y = 24;
			scroll();
		}
		c++;
	}
	int cur_pos = x + y * 80;

	outb(0x3D4, 15);
	outb(0x3D5, cur_pos);
	outb(0x3D4, 14);
	outb(0x3D5, cur_pos >> 8);

	writes_serial(buf);
	spin_unlock(&printf_lock);

	return ret;
}

int sprintf(char *data, const char *format, ...) {

	va_list args;
	va_start(args, format);

	int ret = vsprintf(data, format, args);

	va_end(args);


	return ret;
}

void resetPos(void) {

	x = 0;
	y = 0;
	outb(0x3D4, 15);
	outb(0x3D5, 0);
	outb(0x3D4, 14);
	outb(0x3D5, 0 >> 8);
}

