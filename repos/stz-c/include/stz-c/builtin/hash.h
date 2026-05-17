#pragma once

#include "stz-c/builtin/str.h"

// --------------- Constants ---------------

#define FNV_64_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_64_PRIME        1099511628211

// --------------- Definitions ---------------

// Generic hash function
#define HASH64(T) CONCAT(hash64__, T)

// Specialization for string
SI u64 HASH64(Str)(Str s);

// Mask, step, index
SI int hash64_msi_next(u64 hash, int exp, int i);

// --------------- Implementation ---------------

SI u64 HASH64(Str)(Str s)
{
    u64 h = FNV_64_OFFSET_BASIS;
    RANGE(i, s.len)
    {
        h ^= s.buf[i] & 255;
        h *= FNV_64_PRIME;
    }
    return h;
}

SI int hash64_msi_next(u64 hash, int exp, int i)
{
    unsigned mask = (1 << exp) - 1;
    unsigned step = hash >> (64 - exp) | 1;
    return (i + step) & mask;
}
