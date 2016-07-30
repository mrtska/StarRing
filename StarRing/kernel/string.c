/*
[Module string.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/04/03
*/

#include <string.h>
#include <system.h>

void *memcpy(void *s, const void *ct, size_t size) {

	void *tmp = s;
	char *dp = (char*) s;
	char *sp = (char*) ct;

	while(size--) {
		*dp++ = *sp++;
	}
	return tmp;
}

void *memmove(void *s1, const void *s2, size_t n) {

	char *tmp;
	const char *s;

	tmp = s1;
	if(s1 <= s2) {
		s = s2;
		while(n--) {
			*tmp++ = *s++;
		}
	} else {
		tmp += n;
		s = s2;
		s += n;
		while(n--) {
			*--tmp = *--s;
		}
	}
	return s1;
}

int memcmp(const void *s1, const void *s2, size_t n) {

	const unsigned char *su1;
	const unsigned char *su2;
	int res = 0;
	for(su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, n--) {
		if((res = *su1 - *su2) != 0) {
			break;
		}
	}
	return res;
}

void *memchr(const void *s, int c, size_t n) {

	const unsigned char *p = s;
	while(n-- != 0) {
		if((unsigned char) c == *p++) {
			return (void*) (p - 1);
		}
	}
	return NULL;
}

void *memset(void *s, int c, size_t n) {

	char *xs = s;
	while(n--) {
		*xs++ = c;
	}
	return s;
}

char *strcpy(char *s1, const char *s2) {

	char *tmp = s1;
	while((*s1++ = *s2++) != '\0')
		;
	return tmp;
}

char *strncpy(char *s1, const char *s2, size_t n) {

	char *tmp = s1;
	while(n) {
		if((*tmp = *s2) != 0) {
			s2++;
		}
		tmp++;
		n--;
	}
	return s1;
}

char *strcat(char *s1, const char *s2) {

	char *tmp = s1;
	while(*s1) {
		s1++;
	}
	while((*s1++ = *s2++) != '\0')
		;
	return tmp;
}

char *strncat(char *s1, const char *s2, size_t n) {

	char *tmp = s1;
	if(n) {
		while(*s1) {
			s1++;
		}
		while((*s1++ = *s2++) != 0) {
			if(--n == 0) {
				*s1 = '\0';
				break;
			}
		}
	}
	return tmp;
}

int strcmp(const char *s1, const char *s2) {

	unsigned char c1;
	unsigned char c2;
	while(1) {
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

int strncmp(const char *s1, const char *s2, size_t n) {

	unsigned char c1;
	unsigned char c2;
	while(n) {
		c1 = *s1++;
		c2 = *s2++;
		if(c1 != c2) {
			return c1 < c2 ? -1 : 1;
		}
		if(!c1) {
			break;
		}
		n--;
	}
	return 0;
}

char *strchr(const char *s, int c) {

	for(; *s != (char) c; ++s) {
		if(*s == '\0') {
			return NULL;
		}
	}
	return (char*) s;
}

size_t strcspn(const char *s1, const char *s2) {

	const char *p;
	const char *r;
	size_t count = 0;
	for(p = s1; *p != '\0'; ++p) {
		for(r = s2; *r != '\0'; ++r) {
			if(*p == *r) {
				return count;
			}
		}
		++count;
	}
	return count;
}

char *strpbrk(const char *s1, const char *s2) {

	const char *sc1;
	const char *sc2;
	for(sc1 = s1; *sc1 != '\0'; ++sc1) {
		for(sc2 = s2; *sc2 != '\0'; ++sc2) {
			if(*sc1 == *sc2) {
				return (char*) sc1;
			}
		}
	}
	return NULL;
}

char *strrchr(const char *s, int c) {

	const char *p = s + strlen(s);
	do {
		if(*p == (char) c) {
			return (char*) p;
		}
	} while(--p >= s);
	return NULL;
}

size_t strspn(const char *s1, const char *s2) {

	const char *p;
	const char *a;
	size_t count = 0;
	for(p = s1; *p != '\0'; ++p) {
		for(a = s2; *a != '\0'; ++a) {
			if(*p == *a) {
				break;
			}
		}
		if(*a == '\0') {
			return count;
		}
		++count;
	}
	return count;
}

char *strstr(const char *s1, const char *s2) {

	size_t l1;
	size_t l2;
	l2 = strlen(s2);
	if(!l2) {
		return (char*) s1;
	}
	l1 = strlen(s1);
	while(l1 >= l2) {
		l1--;
		if(!memcmp(s1, s2, l2)) {
			return (char*) s1;
		}
		s1++;
	}
	return NULL;
}

size_t strlen(const char *s) {

	const char *sc;
	for(sc = s; *sc != '\0'; ++sc)
		;
	return sc - s;
}

size_t strnlen(const char *s, size_t count) {

	const char *sc;
	for(sc = s; count-- && *sc != '\0'; ++sc)
		;
	return sc - s;
}

char *strtok(char *s1, const char s2) {

	static char *p = 0;
	static char *ret;

	if(s1 != 0) {
		p = s1;
	}
	if(p == 0) {
		return (0);
	}

	ret = p;

	if(*ret == '\0') {
		p = 0;
		return (0);
	}
	while(*ret == s2) {
		ret++;
	}

	p = ret + 1;
	while(*p != s2) {
		if(*p == '\0') {
			break;
		}
		p++;
	}
	if(*p != '\0') {
		*p++ = '\0';
	}

	return (ret);

}
