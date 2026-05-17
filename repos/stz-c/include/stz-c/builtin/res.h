#pragma once

#include "stz-c/builtin/buf.h"
#include "stz-c/builtin/str.h"

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

DECLARE_RESULT(u8);
DECLARE_RESULT(u16);
DECLARE_RESULT(u32);
DECLARE_RESULT(u64);
DECLARE_RESULT(i8);
DECLARE_RESULT(i16);
DECLARE_RESULT(i32);
DECLARE_RESULT(i64);
DECLARE_RESULT(f32);
DECLARE_RESULT(f64);
DECLARE_RESULT(Any);
DECLARE_RESULT(Chars);
DECLARE_RESULT(CChars);

DECLARE_RESULT(Str);
DECLARE_RESULT(Buf);
