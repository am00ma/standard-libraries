#pragma once

// --------------- Includes ---------------

#include <stddef.h> // ptrdiff_t
#include <stdint.h> // uint.., int..

// --------------- Essentials ---------------

#define SI          static inline
#define countof(a)  (isize)(sizeof(a) / sizeof(*(a)))
#define lengthof(s) (countof(s) - 1)

#define CONCAT_(a, b) a##b
#define CONCAT(a, b)  CONCAT_(a, b)

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

// clang-format off

// --------------- Result ---------------

#define Res(T) CONCAT(Res_, T)

#define DECLARE_RESULT(T) \
    typedef struct        \
    {                     \
        int err;          \
        T   data;         \
    } Res(T);

// --------------- Array ---------------

#define Arr(T) CONCAT(Arr_, T)

#define DECLARE_ARRAY(T) \
    typedef struct       \
    {                    \
        isize len;       \
        T*    buf;       \
    } Arr(T);

// --------------- Vector ---------------

#define Vec(T) CONCAT(Vec_, T)

#define DECLARE_VECTOR(T) \
    typedef struct        \
    {                     \
        isize len;        \
        isize cap;        \
        T*    buf;        \
    } Vec(T);

// clang-format on
