#pragma once

#include "stz-cpp/builtin/buf.hpp"

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

    // Construct from buffer
    Str(Buf* b, isize length);

    // str_equal
    bool operator==(Str s);

    // indexing
    char& operator[](isize i);
    Str   operator[](isize i, isize j);
};

// Shorthand for null string
#define StrNull Str()

// Using Str like an interface
#define Str_(s) Str(s.len, (char*)s.buf)

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

inline Str::Str(Buf* b, isize length)
    : len(length),
      buf(b->Make<char>(length, AllocFlags::NOZERO))
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

inline char& Str::operator[](isize i) { return buf[i]; }

// Only positive indices, start inclusive, end exclusive
inline Str Str::operator[](isize i, isize j)
{
    if ((i < 0) || (j > len) || (i > j)) { return StrNull; } // Bounds check
    return (Str){j - i, &buf[i]};                            // Finally, the normal case (includes empty string)
}
