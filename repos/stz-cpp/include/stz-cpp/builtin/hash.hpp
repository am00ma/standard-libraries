#pragma once

#include "stz-cpp/builtin/str.hpp" // IWYU pragma: keep

// --------------- Constants ---------------

#define FNV_64_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_64_PRIME        1099511628211

// --------------- Definitions ---------------

// clang-format off

// Since we usually store only exponent
#define CapFromExp(exp) (1L << exp)

// Generic hash function
template <typename T> u64 Hash64(T t);

// Generic equality, empty, tombstone
template <typename T> bool IsEqual(T t1, T t2);
template <typename T> bool IsEmpty(T t);
template <typename T> bool IsTomb(T t);
template <typename T> void SetTomb(T& t);

// Mask, step, index
SI int hash64_msi_next(u64 hash, int exp, int i);

// clang-format on

// --------------- Specialization ---------------

// clang-format off

// String - hash function
template <typename Str> u64  Hash64(Str s);
template <typename Str> bool IsEqual(Str t1, Str t2);
template <typename Str> bool IsEmpty(Str t);
template <typename Str> bool IsTomb(Str t);
template <typename Str> void SetTomb(Str& t);

// clang-format on

// --------------- Implementation ---------------

SI int hash64_msi_next(u64 hash, int exp, int i)
{
    unsigned mask = (1 << exp) - 1;
    unsigned step = static_cast<unsigned>(hash >> (64 - exp) | 1);
    return static_cast<int>((static_cast<unsigned>(i) + step) & mask);
}

// --------------- Specialization for string ---------------

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

// clang-format off

template <typename Str> bool IsEqual(Str t1, Str t2) { return t1 == t2; }
template <typename Str> bool IsEmpty(Str t)  { return (!t.buf && !t.len); }
template <typename Str> bool IsTomb(Str t)   { return (!t.buf && (t.len == -1)); }
template <typename Str> void SetTomb(Str& t) { t = {-1, 0}; }

// clang-format on
