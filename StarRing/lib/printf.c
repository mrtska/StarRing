/*
 * printf.c
 *
 *  Created on: 2014/04/03
 *      Author: StarRing
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

#define __do_div(n, base) ({ 						\
	int __res;										\
	__res = ((unsigned long) n) % (unsigned) base; 	\
	n = ((unsigned long) n) / (unsigned) base; 		\
	__res;											\
})



static int skip_atoi(const char **s) {

	int i = 0;
	while(isdigit(**s)) {
		i = i * 10 + *((*s)++) - '0';
	}
	return i;
}

int printf(const char *format, ...) {
	return 0;
}

static char *number(char *str, long num, int base, int size, int precision, int type) {

	static const char digits[16] = "0123456789ABCDEF";

	char tmp[66];
	char c, sign, locase;
	int i;
	int count = 0;

	locase = (type & SMALL);
	if(type & LEFT) {
		type &= ~ZEROPAD;
	}
	if(base < 2 || base > 16) {
		return NULL;
	}
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if(type & SIGN) {
		if(num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if(type & PLUS) {
			sign = '+';
			size--;
		} else if(type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if(type & SPECIAL) {
		if(base == 16)
			size -= 2;
		else if(base == 8)
			size--;
	}
	i = 0;
	if(num == 0) {
		tmp[i++] = '0';
	} else {
		while(num != 0) {
			tmp[i++] = (digits[__do_div(num, base)] | locase);
		}
	}
	if(i > precision) {
		precision = i;
	}
	size -= precision;
	if(!(type & (ZEROPAD + LEFT))) {
		while(size-- > 0) {
			*str++ = ' ';
		}
	}
	if(sign) {
		*str++ = sign;
	}
	if(type & SPECIAL) {
		if(base == 8) {
			*str++ = '0';
		} else if(base == 16) {
			*str++ = '0';
			*str++ = ('X' | locase);
		}
	}
	if(!(type & LEFT)) {
		while(size-- > 0) {
			*str++ = c;
		}
	}
	while(i < precision--) {
		*str++ = '0';
	}
	count = i % 8 == 0 ? 0 : 8 - (i % 8);
	while(i-- > 0) {

		if(count % 8 == 0 && count != 0) {
			*str++ = ',';
		}
		*str++ = tmp[i];

		if(base == 2) {
			count++;
		}
	}
	while(size-- > 0) {
		*str++ = ' ';
	}
	return str;
}

int vsprintf(char *buf, const char *format, va_list args) {

	int len;
	unsigned long num;
	int i;
	int base;
	char *str;
	const char *s;

	int flags;

	int field_width;
	int precision;

	int qualifier;

	//フォーマット指定子パース処理
	for(str = buf; *format; ++format) {
		if(*format != '%') {
			*str++ = *format;
			continue;
		}
		flags = 0;
		reqeat: ++format;
		switch(*format) {
		case '-':
			flags |= LEFT;
			goto reqeat;
		case '+':
			flags |= PLUS;
			goto reqeat;
		case ' ':
			flags |= SPACE;
			goto reqeat;
		case '#':
			flags |= SPECIAL;
			goto reqeat;
		case '0':
			flags |= ZEROPAD;
			goto reqeat;
		}

		field_width = -1;
		if(isdigit(*format)) {
			field_width = skip_atoi(&format);
		} else if(*format == '*') {
			++format;
			field_width = va_arg(args, int);
			if(field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}
		precision = -1;
		if(*format == '.') {
			++format;
			if(isdigit(*format)) {
				precision = skip_atoi(&format);
			} else if(*format == '*') {
				++format;
				//次のフォーマット指定子に移る
				precision = va_arg(args, int);
			}
			if(precision < 0) {
				precision = 0;
			}
		}

		qualifier = -1;
		if(*format == 'h' || *format == 'l' || *format == 'L') {
			qualifier = *format;
			++format;
		}
		base = 10;
		//%xのxの部分の処理
		switch(*format) {
		case 'b':

			str = number(str, (unsigned long) va_arg(args, void*), 2, field_width, 0, flags);
			continue;
		case 'c':
			if(!(flags & LEFT)) {
				while(--field_width > 0) {
					*str++ = ' ';
				}
			}
			*str++ = (unsigned char) va_arg(args, int);
			while(--field_width > 0) {
				*str++ = ' ';
			}
			continue;
		case 's':
			s = va_arg(args, char*);
			len = strnlen(s, precision);
			if(!(flags & LEFT)) {
				while(len < field_width--) {
					*str++ = ' ';
				}
			}
			for(i = 0; i < len; ++i) {
				*str++ = *s++;
			}
			while(len < field_width--) {
				*str++ = ' ';
			}
			continue;
		case 'p':
			if(field_width == -1) {
				field_width = 2 * sizeof(void*);
				flags |= ZEROPAD;
			}
			str = number(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
			continue;
		case 'n':
			if(qualifier == 'l') {
				long *ip = va_arg(args, long*);
				*ip = (str - buf);
			} else {
				int *ip = va_arg(args, int*);
				*ip = (str - buf);
			}
			continue;
		case '%':
			*str++ = '%';
			continue;
		case 'o':
			base = 8;
			break;
		case 'x':
			flags |= SMALL;
		case 'X':
			base = 16;
			break;
		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;
		default:
			*str++ = '%';
			if(*format) {
				*str++ = *format;
			} else {
				--format;
			}
			continue;
		}
		if(qualifier == 'l') {
			num = va_arg(args, unsigned long);
		} else if(qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if(flags & SIGN)
				num = (short) num;
		} else if(flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str - buf;
}
