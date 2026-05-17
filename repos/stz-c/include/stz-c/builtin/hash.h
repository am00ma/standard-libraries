#pragma once

#include "stz-c/builtin/str.h"

// --------------- Constants ---------------

#define FNV_64_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_64_PRIME        1099511628211

// --------------- Definitions ---------------

// Since we usually store only exponent
#define CapFromExp(exp) (1L << exp)

// Generic hash function
#define HASH64(T) CONCAT(Hash64_, T)

// Generic equality, empty and tombstone, expected to be def as macro
#define ISEQUAL(T) CONCAT(IsEqual_, T)
#define ISEMPTY(T) CONCAT(IsEmpty_, T)
#define ISTOMB(T)  CONCAT(IsTomb_, T)
#define SETTOMB(T) CONCAT(SetTomb_, T)

// Mask, step, index
SI int hash64_msi_next(u64 hash, int exp, int i);

// Specialization for string

// --------------- Specialization ---------------

// String - hash function
SI u64 HASH64(Str)(Str s);

// String - hash table necessities
#define IsEqual_Str    str_equal
#define IsEmpty_Str(s) (!((s).buf) && !((s).len))
#define IsTomb_Str(s)  (!((s).buf) && ((s).len == -1))
#define SetTomb_Str(s) (s) = (Str){-1, 0}

// --------------- Implementations ---------------

SI int hash64_msi_next(u64 hash, int exp, int i)
{
    unsigned mask = (1 << exp) - 1;
    unsigned step = hash >> (64 - exp) | 1;
    return (i + step) & mask;
}

// Specialization for string
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
