#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#if 0
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->(MEMBER))
#endif
#define container_of(ptr, type, member)	(type *)((char *)ptr - offsetof(type,member))

#endif // TYPE_H

