#pragma once

#include "stz-c/builtin/buf.h" // IWYU pragma: keep

// --------------- Definition ---------------

typedef struct
{
    isize len;
    char* buf;

} Str;

// clang-format off

// Shorthand for null string
#define StrNull (Str){}

// Construct from literal
#define _(s) (Str){sizeof(s) - 1, s}

// Construct from char*
#define _c(s) (Str){strlen(s), s}

// Using Str like an interface
#define Str_(s) (Str){s.len, (char*)s.buf}

// Check null-terminated
#define IsNullTerm(s) ((s).buf[(s).len] == '\0')

// Printing strings
#define __(s) (int)(s.buf ? s.len : 0), (s.buf ? s.buf : "")

// clang-format on

SI Str  str_new(Buf* b, isize len);
SI bool str_equal(Str s1, Str s2);

// --------------- Implementation ---------------

SI Str str_new(Buf* b, isize len)
{
    return (Str){
        .buf = Make(b, char, len, ALLOC_NOZERO),
        .len = len,
    };
}

SI bool str_equal(Str s1, Str s2)
{
    if (s1.len != s2.len) { return false; }
    if (s1.len == 0) { return true; }
    return !memcmp(s1.buf, s2.buf, s1.len);
}
