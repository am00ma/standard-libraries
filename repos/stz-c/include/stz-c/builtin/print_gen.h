#pragma once

#include "stz-c/builtin/print.h"

#include "stz-c/builtin/buf.h"
#include "stz-c/builtin/str.h"

/* ---------------------------------------------------------------------------
 * Printing generics
 * ------------------------------------------------------------------------- */

// --------------- definitions ---------------

#define PrintVarT(type) CONCAT(PrintVar__, type)

// ... continued after macro declarations as we need generic ...

// --------------- Implementation ---------------

#define Buf__(b) x->len, x->cap, x->buf

#define X_TABLE_PRIMITIVES(type, fmt, args)                                                                            \
    X(Any, "%p", *x)                                                                                                   \
    X(Chars, "%s", *x)                                                                                                 \
    X(CChars, "%s", *x)                                                                                                \
    X(bool, "%s", *x ? "true" : "false")                                                                               \
    X(u8, "%u", *x)                                                                                                    \
    X(u16, "%u", *x)                                                                                                   \
    X(u32, "%u", *x)                                                                                                   \
    X(u64, "%lu", *x)                                                                                                  \
    X(i8, "%d", *x)                                                                                                    \
    X(i16, "%d", *x)                                                                                                   \
    X(i32, "%d", *x)                                                                                                   \
    X(i64, "%ld", *x)                                                                                                  \
    X(f32, "%f", *x)                                                                                                   \
    X(f64, "%f", *x)                                                                                                   \
    X(Str, "%.*s", __((*x)))                                                                                           \
    X(Buf, "%ld, %ld @ %p", Buf__((*x)))

#define X_MACRO_PRINT(type, fmt, args)                                                                                 \
    SI void PrintVarT(type)(type * x) { PrintInline(fmt, args); };

#define X X_MACRO_PRINT
X_TABLE_PRIMITIVES(type, fmt, args)
#undef X

#undef Buf__

#define PrintVar(x)                                                                                                    \
    _Generic((x),                                                                                                      \
        Any: PrintVar__Any,                                                                                            \
        Chars: PrintVar__Chars,                                                                                        \
        CChars: PrintVar__CChars,                                                                                      \
        bool: PrintVar__bool,                                                                                          \
        u8: PrintVar__u8,                                                                                              \
        u16: PrintVar__u16,                                                                                            \
        u32: PrintVar__u32,                                                                                            \
        u64: PrintVar__u64,                                                                                            \
        i8: PrintVar__i8,                                                                                              \
        i16: PrintVar__i16,                                                                                            \
        i32: PrintVar__i32,                                                                                            \
        i64: PrintVar__i64,                                                                                            \
        f32: PrintVar__f32,                                                                                            \
        f64: PrintVar__f64,                                                                                            \
        Str: PrintVar__Str)(&x)

// --------------- Definitions, continued ---------------

#define PrintVarLn(x) (PrintInline("%s = ", #x), PrintVar(x), PrintNewline())
