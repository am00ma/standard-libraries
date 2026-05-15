#pragma once

#include "stz-c/builtin/types.h"

#include <assert.h> // assert
#include <stdlib.h> // malloc
#include <string.h> // memset

// --------------- Definitions ---------------

typedef enum
{
    ALLOC_NOZERO   = 0x0,
    ALLOC_ZERO     = 0x1,
    ALLOC_SOFTFAIL = 0x2,

} AllocFlags;

typedef struct
{
    isize len;
    isize cap;
    char* buf;

} Buf;

// Constructors
SI Buf buf_new(isize cap);
SI Buf buf_new2(Buf* b, isize cap, AllocFlags flags);

// 'Destructors'
SI void buf_reset(Buf* b);
SI void buf_free(Buf* b);

// Allocation
SI char* buf_alloc(Buf* b, usize objsize, usize align, isize count, AllocFlags flags);

#define Make(b, type, n, flags) (type*)buf_alloc(b, sizeof(type), alignof(type), n, flags)

// Measurements
SI isize buf_avail(Buf* b, usize objsize);
SI bool  buf_ontop(Buf* b, void* buf, isize len);

#define buf_stack(name, capacity)                                                                                      \
    char tempbuf__##name[(capacity)] = {};                                                                             \
    Buf  name                        = {.len = 0, .cap = (capacity), .buf = tempbuf__##name};

// --------------- Implementation ---------------

// Allocation
SI char* buf_alloc(Buf* b, usize objsize, usize align, isize count, AllocFlags flags)
{
    if (count < 0) { __builtin_trap(); } // Got negative count

    char* beg = &b->buf[b->len];

    if (count == 0) return beg;

    isize pad = -(uptr)beg & (align - 1); // Works as align is power of 2
    if (count > (b->cap - b->len) / (isize)objsize)
    {
        if (flags & ALLOC_SOFTFAIL) return 0;
        else __builtin_trap(); // Exceeded capacity
    }

    isize total  = count * objsize;
    char* p      = beg + pad;
    b->len      += pad + total;
    if ((flags & ALLOC_ZERO)) p = (char*)memset(p, 0, total);

    return p;
}

// Lifetime
SI Buf buf_new(isize cap)
{
    Buf b = {};
    b.buf = malloc(cap);
    b.len = 0;
    b.cap = b.buf ? cap : 0;
    return b;
}

SI Buf buf_new2(Buf* b, isize cap, AllocFlags flags)
{
    Buf dst = {};
    dst.buf = buf_alloc(b, sizeof(char), alignof(char), cap, flags);
    dst.len = 0;
    dst.cap = dst.buf ? cap : 0;
    return dst;
}

SI void buf_reset(Buf* b) { b->len = 0; }

SI void buf_free(Buf* b)
{
    if (b->buf) free(b->buf);
    *b = (Buf){};
}

// Measurements
SI isize buf_avail(Buf* b, usize objsize) { return (b->cap - b->len) / objsize; }

SI bool buf_ontop(Buf* b, void* buf, isize len) { return (b->len - len) == ((char*)buf - b->buf); }
