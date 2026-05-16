#pragma once

#include "stz-cpp/builtin/buf.hpp" // IWYU pragma: keep

// --------------- Definition ---------------

struct Str
{
    isize len = 0;
    char* buf = nullptr;

    Str() = default;
    Str(isize length, char* buffer);

    // Construct from literal
    template <usize N>
    Str(const char (&s)[N]);

    // Construct from char*
    Str(char* chars);

    // str_equal
    bool operator==(Str s);
};

// Shorthand for null string
#define StrNull Str()

// Using Str like an interface
#define Str_(s)                                                                                                        \
    (Str) { s.len, (char*)s.buf }

// Check null-terminated
#define IsNullTerm(s) ((s).buf[(s).len] == '\0')

// Printing strings
#define __(s) (int)(s.buf ? s.len : 0), (s.buf ? s.buf : "")

// --------------- Implementation ---------------

inline Str::Str(isize length, char* buffer)
    : len(length),
      buf(buffer)
{
}

template <usize N>
inline Str::Str(const char (&s)[N])
    : len(N - 1),
      buf((char*)s)
{
}

inline Str::Str(char* s)
    : len(static_cast<isize>(strlen(s))),
      buf(s)
{
}

// str_equal cases:
//   - Empty strings are equal: `"" == ""`
//   - Null strings are equal: `StrNull == StrNull`
//   - but empty != null: `"" != StrNull`
//   - Non-null and non-empty strings must match at all bytes
inline bool Str::operator==(Str s)
{
    if (s.len != len) return false;
    if (!len) { return (buf && s.buf) || (!buf && !s.buf); }
    return !strncmp(buf, s.buf, static_cast<usize>(len));
}
