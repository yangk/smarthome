#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>
#include "CommFunc.h"

struct fifo
{
    int tx, rx;
    int len;
    uint8_t *buf;
};

#define fifo_full(f) ((((f)->rx + 1) & ((f)->len-1)) == (f)->tx)
#define fifo_empty(f) (((f)->tx & ((f)->len-1)) == (f)->rx)
#define fifo_bytes(f) (((f)->rx + (f)->len - (f)->tx) & ((f)->len - 1))
#define fifo_empty_bytes(f) ((f)->tx + (f)->len - (f)->rx - 1) & ((f)->len - 1)

//extern "C"
//{
COMMFUNCSHARED_EXPORT void fifo_init(struct fifo *f, void *buf, int len);
COMMFUNCSHARED_EXPORT int fifo_put(struct fifo *f, const void *in, int len);
COMMFUNCSHARED_EXPORT int fifo_get(struct fifo *f, void *out, int len);
//}
#endif
