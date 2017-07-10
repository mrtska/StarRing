

extern "C" void *memcpy(void *s, const void *ct, unsigned int size) {

	void *tmp = s;
	char *dp = (char*) s;
	char *sp = (char*) ct;

	while(size--) {
		*dp++ = *sp++;
	}
	return tmp;
}
extern "C" void *memmove(void *s1, const void *s2, unsigned int n) {

	char *tmp;
	const char *s;

	tmp = static_cast<char*>(s1);
	if(s1 <= s2) {
		s = static_cast<const char*>(s2);
		while(n--) {
			*tmp++ = *s++;
		}
	} else {
		tmp += n;
		s = static_cast<const char*>(s2);
		s += n;
		while(n--) {
			*--tmp = *--s;
		}
	}
	return s1;
}

extern "C" void *memset(void *s, int c, unsigned int n) {

	char *xs = static_cast<char*>(s);
	while(n--) {
		*xs++ = c;
	}
	return s;
}

extern "C" int memcmp(const void *s1, const void *s2, unsigned int n) {

	const unsigned char *su1;
	const unsigned char *su2;
	int res = 0;
	for(su1 = static_cast<const unsigned char*>(s1), su2 = static_cast<const unsigned char*>(s2); 0 < n; ++su1, ++su2, n--) {
		if((res = *su1 - *su2) != 0) {
			break;
		}
	}
	return res;
}

extern "C" unsigned int strlen(const char *str) {

        
    char *start;
    char *s = const_cast<char*>(str);
    start = s;
    while( *s != 0 ) {
        ++s;
    }
    return s - start;
}

unsigned int strnlen(const char *s, unsigned int count) {

	const char *sc;
	for(sc = s; count-- && *sc != '\0'; ++sc)
		;
	return sc - s;
}



extern "C" int strcmp(const char *s1, const char *s2) {

	unsigned char c1;
	unsigned char c2;
	while(true) {
		c1 = *s1++;
		c2 = *s2++;
		if(c1 != c2) {
			return c1 < c2 ? -1 : 1;
		}
		if(!c1) {
			break;
		}
	}
	return 0;
}

const char *strchr(const char *s, int c) {

	for(; *s != (char) c; ++s) {
		if(*s == '\0') {
			return nullptr;
		}
	}
	return (const char*) s;
}



extern "C" int isdigit(int c) {

	return (unsigned)c-'0' < 10;
}
extern "C" int isxdigit(int c) {

	return isdigit(c) || ((unsigned)c|32)-'a' < 6;
}

extern "C" int isupper(int c) {

	return (unsigned)c-'A' < 26;
}

typedef __builtin_va_list va_list;

#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_start(ap, type) __builtin_va_start(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#define va_copy(dest, src) __builtin_va_copy(dest, src)


#define ZEROPAD	1		// pad with zero
#define SIGN	2		// unsigned/signed long
#define PLUS	4		// show plus
#define SPACE	8		// space if plus
#define LEFT	16		// left justified
#define SMALL	32		// Must be 32 == 0x20
#define SPECIAL	64		// 0x

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


static char *number(char *str, long num, int base, int size, int precision, int type) {

	static const char digits[17] = "0123456789ABCDEF";

	char tmp[66];
	char c, sign, locase;
	int i;
	int count = 0;

	locase = (type & SMALL);
	if(type & LEFT) {
		type &= ~ZEROPAD;
	}
	if(base < 2 || base > 16) {
		return nullptr;
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

int vsnprintf(char *s, unsigned int n, const char *fmt, va_list args) {

    int ret = vsprintf(s, fmt ,args);

    s[n] = '\0';

    return ret;
}

extern "C" int snprintf(char *s, unsigned int n, const char *fmt, ...) {

	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsnprintf(s, n, fmt, ap);
	va_end(ap);
	return ret;
}

