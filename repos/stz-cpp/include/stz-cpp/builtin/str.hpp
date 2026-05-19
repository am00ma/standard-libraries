#pragma once

#include "stz-cpp/builtin/buf.hpp"

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf TODO: Why stdio?

// For some reason, clangd complains
#define __builtin_c23_va_start(a, b)

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

    // Memory management
    Str Copy(Buf* b, bool null_terminated);

    // Substrings
    char& operator[](isize i);
    Str   operator[](isize i, isize j);
    bool  StartsWith(Str prefix);
    bool  EndsWith(Str suffix);
    isize Find(Str sub);
};

// Format strings
SI Str str_fmtn(Buf* b, isize len, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
SI Str str_fmt(Buf* b, const char* fmt, ...) __attribute__((format(printf, 2, 3)));

// Shorthand for null string
#define StrNull Str()

// Using Str like an interface
#define Str_(s) Str(s.len, (char*)s.buf)

// Printing strings
#define __(s) (int)(s.buf ? s.len : 0), (s.buf ? s.buf : "")

// Check null-terminated: NOTE: Goes beyond buffer len
#define IsNullTerm(s) ((s).buf[(s).len] == '\0')

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

inline Str Str::Copy(Buf* a, bool null_term)
{
    if (!len) return StrNull;                                            // Null case
    Str c(len, a->Make<char>(len + (int)null_term, AllocFlags::NOZERO)); // Alloc
    memcpy(c.buf, buf, static_cast<usize>(c.len));                       // Copy string
    if (null_term) c.buf[c.len] = '\0'; // Set null if needed since we init with ARENA_NOZERO
    return c;
}

// --------------- Format ---------------

SI Str str_fmtn(Buf* b, isize len, char const* fmt, ...)
{
    Str s(0, b->Make<char>(len, AllocFlags::NOZERO));

    va_list arg;
    va_start(arg, fmt);
    s.len = vsnprintf(s.buf, static_cast<usize>(len), fmt, arg);
    va_end(arg);

    b->len -= (len - s.len);
    return s;
}

SI Str str_fmt(Buf* b, char const* fmt, ...)
{
    isize len = b->Avail<char>();
    Str   s(0, b->Make<char>(len, AllocFlags::NOZERO));

    va_list arg;
    va_start(arg, fmt);
    s.len = vsnprintf(s.buf, static_cast<usize>(len), fmt, arg);
    va_end(arg);

    b->len -= (len - s.len);
    return s;
}

// --------------- Substrings ---------------

inline char& Str::operator[](isize i) { return buf[i]; }

inline Str Str::operator[](isize i, isize j)
{
    assert((i >= 0) && (i <= j) && (j <= len)); // Bounds check
    return Str(j - i, &buf[i]);                 // Finally, the normal case (includes empty string)
}

inline bool Str::StartsWith(Str prefix)
{
    if (len < prefix.len) return false;
    return (*this)[0, prefix.len] == prefix;
}

inline bool Str::EndsWith(Str suffix)
{
    if (len < suffix.len) return false;
    return (*this)[len - suffix.len, len] == suffix;
}

inline isize Str::Find(Str sub)
{
    if (!buf) return (sub.buf ? -1 : 0); // If src is StrNull, only true if sub is also StrNull
    if (!sub.len) return 0;              // Always finds StrNull, Empty string
    if (sub.len > len) return -1;
    RANGE(i, len)
    {
        if (!(buf[i] == sub.buf[0])) continue;
        if (Str(sub.len, &buf[i]) == sub) { return i; }
    }
    return -1;
}
