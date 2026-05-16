#pragma once

// --------------- Includes ---------------

#include <stddef.h> // ptrdiff_t
#include <stdint.h> // uint.., int..

// --------------- Essentials ---------------

#define PrintFunc(fn, dst, fmt, ...) fn(dst, fmt, ##__VA_ARGS__)

#define SI          static inline
#define countof(a)  (isize)(sizeof(a) / sizeof(*(a)))
#define lengthof(s) (countof(s) - 1)

#define RANGE(...)                 RANGEx(__VA_ARGS__, RANGE4, RANGE3, RANGE2, RANGE1)(__VA_ARGS__)
#define RANGEx(a, b, c, d, e, ...) e
#define RANGE1(i)                  for (isize i = 0; i < 1; i++)
#define RANGE2(i, b)               for (isize i = 0; i < (b); i++)
#define RANGE3(i, a, b)            for (isize i = (a); i < (b); i++)
#define RANGE4(i, a, b, c)         for (isize i = (a); i < (b); i += (c))

#define CONCAT_(a, b) a##b
#define CONCAT(a, b)  CONCAT_(a, b)

// Array primitives that may let us change `buf`, `len` names later
#define AT(s, i) s.buf[i]
#define LEN(s)   s.len

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

#define Res(T) CONCAT(Res_, T)

// clang-format off

#define DECLARE_RESULT(T) \
    typedef struct        \
    {                     \
        int err;          \
        T   data;         \
    } Res(T);

// clang-format on
