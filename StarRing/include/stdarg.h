#pragma once


#ifdef __GNUC__

typedef __builtin_va_list va_list;

#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_start(ap, type) __builtin_va_start(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#define va_copy(dest, src) __builtin_va_copy(dest, src)

#else

#define va_start(ap,v) ap  = (va_list)&v + sizeof(v)
#define va_end(list)
#define va_arg(list, mode) ((mode *)(list =\
 (char *) ((((int)list + (__alignof__(mode)<=4?3:7)) &\
 (__alignof__(mode)<=4?-4:-8))+sizeof(mode))))[-1]
typedef char *  va_list;
#endif

