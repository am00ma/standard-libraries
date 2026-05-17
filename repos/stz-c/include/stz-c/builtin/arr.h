#pragma once

#include "stz-c/builtin/buf.h"
#include "stz-c/builtin/str.h"

#include <assert.h>

// Name mangling to get generics
#define Arr(T) CONCAT(Arr_, T)

// clang-format off

// Declaration of struct
#define DECLARE_ARRAY(T) \
    typedef struct       \
    {                    \
        isize len;       \
        T*    buf;       \
    } Arr(T);

// Straight out macros, so preserves type info
#define arr_new(type, buffer, n, flags) (Arr(type)) {.len = (n), .buf = Make((buffer), type, (n), (flags))}

// Shrink given buffer
#define arr_shrink(arr, buffer, newlen)                                                                            \
    assert((newlen <= (arr).len) && buf_ontop((buffer), (arr).buf, ((arr).len) * sizeof(__typeof__(*(arr).buf)))); \
    (buffer)->len -= ((arr).len - (newlen)) * sizeof(__typeof__(*(arr).buf));                                      \
    (arr).len      = (newlen);

// Easy element access, type preserved
#define arr_first(arr)     (arr).buf[0]
#define arr_last(arr)      (arr).buf[(arr).len - 1]
#define arr_sub(arr, i, j) (__typeof__(arr)) {.len = ((j) - (i)), .buf = &arr.buf[(i)]}

// clang-format on

// Forward declaring primitive types, available in types.h

DECLARE_ARRAY(bool);
DECLARE_ARRAY(u8);
DECLARE_ARRAY(u16);
DECLARE_ARRAY(u32);
DECLARE_ARRAY(u64);
DECLARE_ARRAY(i8);
DECLARE_ARRAY(i16);
DECLARE_ARRAY(i32);
DECLARE_ARRAY(i64);
DECLARE_ARRAY(f32);
DECLARE_ARRAY(f64);

DECLARE_ARRAY(Any);
DECLARE_ARRAY(Chars);
DECLARE_ARRAY(CChars);

DECLARE_ARRAY(Str);
DECLARE_ARRAY(Buf);
