#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Commfunc.h"

//---------------------------------------------------------------------------------------
uint8_t checksum(const void *data, int len)
{
    uint8_t cs = 0;

    while (len-- > 0)
        cs += *((uint8_t *) data + len);
    return cs;
}
//---------------------------------------------------------------------------------------
uint64_t get_le_val(const uint8_t * p, size_t bytes)
{
    uint64_t ret = 0;

    while (bytes-- > 0)
    {
        ret <<= 8;
        ret |= *(p + bytes);
    }
    return ret;
}
uint64_t get_be_val(const uint8_t * p, size_t bytes)
{
    uint64_t ret = 0;
    while (bytes-- > 0)
    {
        ret <<= 8;
        ret |= *p++;
    }

    return ret;
}
void put_le_val(uint64_t val, uint8_t * p, size_t bytes)
{
    while (bytes-- > 0)
    {
        *p++ = val & 0xFF;
        val >>= 8;
    }
}
void put_be_val(uint64_t val, uint8_t * p, size_t bytes)
{
    while (bytes-- > 0)
    {
        *(p + bytes) = val & 0xFF;
        val >>= 8;
    }
}
//---------------------------------------------------------------------------------------
int is_all_xx(const void *s1, uint8_t val, int n)
{
    while (n && *(uint8_t *) s1 == val)
    {
        s1 = (uint8_t *) s1 + 1;
        n--;
    }
    return !n;
}
void reverse(void *data, int len)
{
    uint8_t *rearp = (uint8_t *) data + len - 1;
    uint8_t *headp = (uint8_t *) data;

    if (len < 1) return;
    while (headp < rearp)
    {
        _swap8(*headp, *rearp);
        headp++;
        rearp--;
    }
}
//---------------------------------------------------------------------------------------
int arr2str_complex(const void *array, int len, char *destStr, int maxLen,
             int num_in_each_line, const char *split, const char *addon)
{
    int i, idx = 0;

    maxLen -= 3;
    const unsigned char *charArray = (const unsigned char *)array;
    for (i = 0; i < len && idx < maxLen; i++)
    {
        if (i > 0 && i % num_in_each_line == 0)
        {
            idx += snprintf(destStr + idx, maxLen - idx, "\n");
        }
        idx += snprintf(destStr + idx, maxLen - idx + 2, "%02X%s", charArray[i], split);
    }
    idx += snprintf(destStr + idx, maxLen - idx + 2, "%s", addon);
    return i;
}

char *i2str(uint8_t val, char *dest)
{
    const char *charmap = "0123456789ABCDEF";

    *dest++ = charmap[get_bits(val, 4, 7)];
    *dest++ = charmap[get_bits(val, 0, 3)];
    *dest++ = '\0';
    return dest-3;
}
const char *arr2str(const void *arr, int len, void *dest, int maxlen)
{
    const uint8_t *_arr = (const uint8_t *)arr;
    char *_dest = (char *)dest;

    if (len == 0) goto out;

    while (len-- && maxlen > 0)
    {
        i2str(*_arr++, _dest);
        _dest += 2;
        *_dest++ = ' ';
        maxlen -= 3;
    }
    if (_dest - (char*)dest > 0) _dest--;
out:
    *_dest = '\0';
    return (const char *)dest;
}

//---------------------------------------------------------------------------------------
int split(char *str, char **arr, const char *del)
{
    if (!str) return 0;

    char **src = arr;

    for (*arr++ = str; *str; str++)
    {
        if (*str != *del) continue;

        *str = '\0';
        *arr++ = str+1;
    }

    return arr - src;
}
//---------------------------------------------------------------------------------------
static int get_num(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}
int is_good_xdstr(const char *str)
{
    while (*str)
    {
        if (!isxdigit(*str) && *str != ' ')
            return 0;
        str++;
    }
    return 1;
}
int str2arr(const char *str, void *out, int maxlen)
{
    int i, n = 0;

    while (*str)
    {
        while (*str && !isxdigit(*str)) str++;

        uint8_t val = 0;
        for (i = 0; i < 2 && isxdigit(*str); i++)
        {
            val <<= 4;
            val |= get_num(*str++);
        }
        if (i) ((uint8_t*)out)[n++] = val;
        if (n >= maxlen) break;
    }
    return n;
}
//---------------------------------------------------------------------------------------
uint16_t calc_crc16(uint16_t crc, const void *buf, uint32_t size)
{
    uint32_t i;
    uint8_t *_buf = (uint8_t *) buf;

    while (size-- != 0)
    {
        for (i = 0x80; i != 0; i >>= 1)
        {
            if ((crc & 0x8000) != 0)
            {
                crc <<= 1;
                crc ^= 0x1021;
            }
            else
            {
                crc <<= 1;
            }
            if (((*_buf) & i) != 0)
            {
                crc ^= 0x1021;
            }
        }
        _buf++;
    }
    return crc;
}
//---------------------------------------------------------------------------------------
const void *memmem(const void *start, size_t s_len, const void *find, size_t f_len)
{
    size_t len = 0;
    const char *p = (char *)start;
    char *q = (char *)find;

    while((p - (char *)start + f_len) <= s_len)
    {
        while (*p++ == *q++)
        {
            len++;
            if (len == f_len)
                return(p - f_len);
        };
        q = (char *)find;
        len = 0;
    };

    return NULL;
}
//---------------------------------------------------------------------------------------
