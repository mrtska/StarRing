#include <string>

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

extern "C" int isupper(int c) {
	return (unsigned) c - 'A' < 26;
}

extern "C" int isdigit(int c) {

	return (unsigned) c - '0' < 10;
}


extern "C" int isxdigit(int c) {
	return isdigit(c) || ((unsigned)c | 32) - 'a' < 6;
}

int vsnprintf(char *s, unsigned int n, const char *fmt, va_list args) {

    int ret = std::vsprintf(s, fmt ,args);

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

