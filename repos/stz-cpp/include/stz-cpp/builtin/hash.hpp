#pragma once

#include "stz-cpp/builtin/str.hpp" // IWYU pragma: keep

// --------------- Constants ---------------

#define FNV_64_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_64_PRIME        1099511628211

// --------------- Definitions ---------------

// Generic hash function
template <typename T>
u64 Hash64(T t);

// Specialization for string
template <typename Str>
u64 Hash64(Str s);

// Mask, step, index
SI int hash64_msi_next(u64 hash, int exp, int i);

// --------------- Implementation ---------------

template <typename Str>
inline u64 Hash64(Str s)
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
    unsigned step = static_cast<unsigned>(hash >> (64 - exp) | 1);
    return static_cast<int>((static_cast<unsigned>(i) + step) & mask);
}
