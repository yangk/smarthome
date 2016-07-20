#ifndef COMMFUNC_H
#define COMMFUNC_H

#include <QtCore/qglobal.h>
#include <stdint.h>

#if defined(COMMFUNC_LIBRARY)
#  define COMMFUNCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define COMMFUNCSHARED_EXPORT Q_DECL_IMPORT
#endif


#define SIZE_1K 1024L
#define SIZE_1M (SIZE_1K*SIZE_1K)

#define set_bit(x, bit) ((x) |= 1 << (bit))
#define clr_bit(x, bit) ((x) &= ~(1 << (bit)))
#define tst_bit(x, bit) ((x) & (1 << (bit)))
#define get_bits(val,x1,x2) (((val)>>(x1))&((1<<((x2)-(x1)+1))-1))

#ifndef min
#define min(a, b) ((a)<(b) ? (a):(b))
#endif
#ifndef max
#define max(a, b) ((a)>(b) ? (a):(b))
#endif

#define array_size(array) (sizeof(array)/sizeof(*array))

#define time_after(a,b) ((short)((b) - (a)) < 0)
#define time_before(a,b) time_after(b,a)
#define time_after_eq(a,b)((short)((a) - (b)) >= 0)
#define time_before_eq(a,b)	time_after_eq(b,a)

#define _swap8(a, b) do {a ^= b; b ^= a; a ^= b;} while (0)

#define _swab16(x) ((uint16_t)(	\
    (((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |\
    (((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define _swab32(x) ((uint32_t)(	\
    (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |\
    (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |\
    (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |\
    (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#ifdef BIG_ENDIAN
#define cpu_to_le32(x) ((uint32_t)_swab32((x)))
#define le32_to_cpu(x) ((uint32_t)_swab32((uint32_t)(x)))
#define cpu_to_le16(x) ((uint16_t)_swab16((x)))
#define le16_to_cpu(x) ((uint16_t)_swab16((uint16_t)(x)))
#define cpu_to_be32(x) ((uint32_t)(x))
#define be32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_be16(x) ((uint16_t)(x))
#define be16_to_cpu(x) ((uint16_t)(x))
#else
#define cpu_to_le32(x) ((uint32_t)(x))
#define le32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_le16(x) ((uint16_t)(x))
#define le16_to_cpu(x) ((uint16_t)(x))
#define cpu_to_be32(x) ((uint32_t)_swab32((x)))
#define be32_to_cpu(x) ((uint32_t)_swab32((uint32_t)(x)))
#define cpu_to_be16(x) ((uint16_t)_swab16((x)))
#define be16_to_cpu(x) ((uint16_t)_swab16((uint16_t)(x)))
#endif

#define bcd2bin(val) (((val) & 0x0f) + ((val) >> 4) * 10)
#define bin2bcd(val) ((((val) / 10) << 4) + (val) % 10)

//extern "C"
//{
COMMFUNCSHARED_EXPORT uint8_t checksum(const void *data, int len);

/* byte order */
COMMFUNCSHARED_EXPORT uint64_t get_le_val(const uint8_t * p, size_t bytes);
COMMFUNCSHARED_EXPORT uint64_t get_be_val(const uint8_t * p, size_t bytes);
COMMFUNCSHARED_EXPORT void put_le_val(uint64_t val, uint8_t * p, size_t bytes);
COMMFUNCSHARED_EXPORT void put_be_val(uint64_t val, uint8_t * p, size_t bytes);

COMMFUNCSHARED_EXPORT int split(char *str, char **arr, const char *del);
COMMFUNCSHARED_EXPORT int is_all_xx(const void *s1, uint8_t val, int n);
COMMFUNCSHARED_EXPORT void reverse(void *data, int len);

COMMFUNCSHARED_EXPORT int arr2str_complex(const void *array, int len, char *destStr, int maxLen,
             int num_in_each_line, const char *split, const char *addon);
COMMFUNCSHARED_EXPORT const char *arr2str(const void *arr, int len, void *dest, int maxlen);
COMMFUNCSHARED_EXPORT int is_good_xdstr(const char *str);
COMMFUNCSHARED_EXPORT int str2arr(const char *str, void *out, int maxlen);
COMMFUNCSHARED_EXPORT char *i2str(uint8_t val, char *destStr);

COMMFUNCSHARED_EXPORT uint16_t calc_crc16(uint16_t crc, const void *buf, uint32_t size);
COMMFUNCSHARED_EXPORT const void *memmem(const void *start, size_t s_len, const void *find, size_t f_len);
//}
#endif // COMMFUNC_H
