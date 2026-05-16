#pragma once

#include "stz-cpp/builtin/print.hpp"

#include "stz-cpp/builtin/buf.hpp"
#include "stz-cpp/builtin/str.hpp"

// --------------- definitions ---------------

template <typename X>
inline void PrintVar(X* x);

// ... continued after macro declarations to correspond to c version ...

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
    template <>                                                                                                        \
    inline void PrintVar<type>(type * x)                                                                               \
    { PrintInline(fmt, args); }

#define X X_MACRO_PRINT
X_TABLE_PRIMITIVES(type, fmt, args)
#undef X

#undef Buf__

// --------------- Definitions, continued ---------------

#define PrintVarLn(x) (PrintInline("%s = ", #x), PrintVar((&x)), PrintNewline())
