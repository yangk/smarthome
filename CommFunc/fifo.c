#include "fifo.h"

void fifo_init(struct fifo *f, void *buf, int len)
{
    f->rx = f->tx = 0;
    f->buf = (uint8_t *)buf;
    f->len = len;
}

int fifo_put(struct fifo *f, const void *in, int len)
{
    int i = 0;

    const uint8_t *buf = (const uint8_t *)in;

    while (!fifo_full(f) && i < len)
    {
        f->buf[f->rx++] = buf[i++];
        f->rx &= f->len - 1;
    }
    return i;
}

int fifo_get(struct fifo * f, void *out, int len)
{
    int i = 0;

    uint8_t *buf = (uint8_t *)out;

    while (!fifo_empty(f) && i < len)
    {
        buf[i++] = f->buf[f->tx++];
        f->tx &= f->len - 1;
    }
    return i;
}
