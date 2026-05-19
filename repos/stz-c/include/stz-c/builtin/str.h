#pragma once

#include "stz-c/builtin/buf.h"

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vsnprintf TODO: Why stdio?

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

// Printing strings
#define __(s) (int)(s.buf ? s.len : 0), (s.buf ? s.buf : "")

// Check null-terminated: NOTE: Goes beyond buffer len
#define IsNullTerm(s) ((s).buf[(s).len] == '\0')

// clang-format on

// Essentials
SI Str  str_new(Buf* b, isize len);
SI bool str_equal(Str s1, Str s2);

// Memory management
SI Str str_copy(Buf* b, Str s, bool null_terminated);

// Format strings
SI Str str_fmtn(Buf* b, isize len, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
SI Str str_fmt(Buf* b, const char* fmt, ...) __attribute__((format(printf, 2, 3)));

// Substrings
SI Str  str_sub(Str s1, isize start, isize end);
SI bool str_startswith(Str s1, Str prefix);
SI bool str_endswith(Str s1, Str suffix);

// --------------- Implementation ---------------

SI Str str_new(Buf* b, isize len)
{
    return (Str){
        .buf = Make(b, char, len, ALLOC_NOZERO),
        .len = len,
    };
}

// str_equal cases:
//   - Empty strings are equal: `"" == ""`
//   - Null strings are equal: `StrNull == StrNull`
//   - but empty != null: `"" != StrNull`
//   - Non-null and non-empty strings must match at all bytes
SI bool str_equal(Str s1, Str s2)
{
    if (s1.len != s2.len) { return false; }
    if (s1.len == 0) { return (s1.buf && s2.buf) || (!s1.buf && !s2.buf); }
    return !memcmp(s1.buf, s2.buf, s1.len);
}

SI Str str_copy(Buf* a, Str s, bool null_term)
{
    if (!s.len) return StrNull;                                                         // Null case
    Str c = {.len = s.len, .buf = Make(a, char, s.len + (int)null_term, ALLOC_NOZERO)}; // Alloc
    memcpy(c.buf, s.buf, c.len);                                                        // Copy string
    if (null_term) c.buf[c.len] = '\0'; // Set null if needed since we init with ARENA_NOZERO
    return c;
}

SI Str str_fmtn(Buf* b, isize len, char const* fmt, ...)
{
    Str s = {.len = 0, .buf = Make(b, char, len, ALLOC_NOZERO)};

    va_list arg;
    va_start(arg, fmt);
    s.len = vsnprintf(s.buf, len, fmt, arg);
    va_end(arg);

    b->len -= (len - s.len);
    return s;
}

SI Str str_fmt(Buf* b, char const* fmt, ...)
{
    isize len = buf_avail(b, sizeof(char));
    Str   s   = {.len = 0, .buf = Make(b, char, len, ALLOC_NOZERO)};

    va_list arg;
    va_start(arg, fmt);
    s.len = vsnprintf(s.buf, len, fmt, arg);
    va_end(arg);

    b->len -= (len - s.len);
    return s;
}

// Only positive indices, start inclusive, end exclusive
SI Str str_sub(Str s, isize i, isize j)
{
    assert((i >= 0) && (i <= j) && (j <= s.len)); // Bounds check
    return (Str){j - i, &s.buf[i]};               // Finally, the normal case (includes empty string)
}

bool str_startswith(Str s1, Str prefix)
{
    if (s1.len < prefix.len) return false;
    return str_equal(str_sub(s1, 0, prefix.len), prefix);
}

bool str_endswith(Str s1, Str suffix)
{
    if (s1.len < suffix.len) return false;
    return str_equal(str_sub(s1, s1.len - suffix.len, s1.len), suffix);
}
