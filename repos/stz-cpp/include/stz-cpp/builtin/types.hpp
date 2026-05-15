#pragma once

// --------------- Includes ---------------

#include <cstddef> // ptrdiff_t
#include <cstdint> // uint.., int..

// --------------- Essentials ---------------

#define SI          static inline
#define countof(a)  (isize)(sizeof(a) / sizeof(*(a)))
#define lengthof(s) (countof(s) - 1)

// --------------- Primitives ---------------

typedef uintptr_t uptr;
typedef ptrdiff_t isize;
typedef size_t    usize;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

typedef void*       Any;
typedef char*       Chars;
typedef const char* CChars;

// --------------- Result ---------------

template <typename T>
struct Res
{
    int err;
    T   data;
};

// --------------- Array ---------------

template <typename T>
struct Arr
{
    isize len;
    T*    buf;
};

// --------------- Vector ---------------

template <typename T>
struct Vec
{
    isize len;
    isize cap;
    T*    buf;
};
