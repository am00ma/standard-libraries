#pragma once

#include "stz-c/builtin/arr.h"
#include "stz-c/builtin/buf.h"

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vsnprintf TODO: Why stdio?

// --------------- Definition ---------------

typedef struct
{
    isize len;
    char* buf;

} Str;

// Key Value
DECLARE_PAIR(Str, Str);

// Array of strings
DECLARE_ARRAY(Str);

// clang-format off

// Shorthand for null string
#define StrNull (Str){}

// Construct from literal
#define _(s) (Str){sizeof(s) - 1, s}

// Construct from char*
#define _c(s) (Str){strlen(s), s}

// Using Str like an interface
#define Str_(s) (Str){(s).len, (char*)(s).buf}

// Printing strings
#define __(s) (int)((s).buf ? (s).len : 0), ((s).buf ? (s).buf : "")

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

// Trim
typedef enum
{
    TRIM_NONE          = 0,
    TRIM_LEFT          = 1U << 0,
    TRIM_RIGHT         = 1U << 1,
    TRIM_SPACES        = 1U << 2, // ' '
    TRIM_TABS          = 1U << 3, // '\t'
    TRIM_NEWLINES      = 1U << 4, // '\n'
    TRIM_CRETURNS      = 1U << 5, // '\r'
    TRIM_LEFTRIGHT     = TRIM_LEFT | TRIM_RIGHT,
    TRIM_WHITESPACE    = TRIM_SPACES | TRIM_TABS | TRIM_NEWLINES | TRIM_CRETURNS,
    TRIM_DEFAULT       = TRIM_LEFTRIGHT | TRIM_WHITESPACE,
    TRIM_DEFAULT_LEFT  = TRIM_LEFT | TRIM_WHITESPACE,
    TRIM_DEFAULT_RIGHT = TRIM_RIGHT | TRIM_WHITESPACE,

} StrTrimFlags;

SI Str str_trim(Str src, StrTrimFlags flags);

// Split
typedef enum
{
    SPLIT_DEFAULT         = 0,
    SPLIT_IGNORE_EMPTY    = 1U << 0,
    SPLIT_SUBSTITUTE_NULL = 1U << 1,

} StrSplitFlags;

SI PAIR(Str, Str) str_split_pair(Str src, char sep, StrTrimFlags flags);
SI Arr(Str) str_splitc(Buf* b, Str src, char sep, isize maxlen, StrSplitFlags flags);
SI Arr(Str) str_split_lines(Buf* b, Str src, isize maxlen, bool ignore_empty);

// Iterators
SI Str str_till_next(Str* src, char c);
SI Str str_till_next2(Str* src, Str c);

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

// --------------- Format ---------------

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

// --------------- Substrings ---------------

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

isize str_find(Str s1, Str sub)
{
    if (!s1.buf) return (sub.buf ? -1 : 0); // If s1 is StrNull, only true if sub is also StrNull
    if (!sub.len) return 0;                 // Always finds StrNull, Empty string
    if (sub.len > s1.len) return -1;
    RANGE(i, s1.len)
    {
        if (!(s1.buf[i] == sub.buf[0])) continue;
        if (str_equal((Str){.buf = &s1.buf[i], .len = sub.len}, sub)) { return i; }
    }
    return -1;
}

// --------------- Trim ---------------

SI Str str_trim(Str src, StrTrimFlags flags)
{
    if (!src.len) return src;

    isize start = 0;
    if (flags & TRIM_LEFT)
    {
        RANGE(i, src.len)
        {
            if ((src.buf[i] == ' ') && (flags & TRIM_SPACES)) continue;
            else if ((src.buf[i] == '\t') && (flags & TRIM_TABS)) continue;
            else if ((src.buf[i] == '\n') && (flags & TRIM_NEWLINES)) continue;
            else if ((src.buf[i] == '\r') && (flags & TRIM_CRETURNS)) continue;
            else
            {
                start = i;
                break;
            }
        }
    }

    isize stop = src.len;
    if (flags & TRIM_RIGHT)
    {
        for (isize i = src.len - 1; i >= 0; i--)
        {
            if ((src.buf[i] == ' ') && (flags & TRIM_SPACES)) continue;
            else if ((src.buf[i] == '\t') && (flags & TRIM_TABS)) continue;
            else if ((src.buf[i] == '\n') && (flags & TRIM_NEWLINES)) continue;
            else if ((src.buf[i] == '\r') && (flags & TRIM_CRETURNS)) continue;
            else
            {
                stop = i + 1;
                break;
            }
        }
    }

    return (Str){
        .len = stop - start,
        .buf = &src.buf[start],
    };
}

// --------------- Split ---------------

SI PAIR(Str, Str) str_split_pair(Str src, char sep, StrTrimFlags flags)
{
    if (!src.len) return (PAIR(Str, Str)){src, StrNull};

    isize found = -1;
    RANGE(i, src.len)
    {
        if (!(src.buf[i] == sep)) continue;
        found = i;
        break;
    }

    // If not found: Probably means key with no val
    // e.g. with sep ':'
    //      src is `hello ` instead of `hello: hi`
    //      so return `hello`,``
    if (found < 0) return (PAIR(Str, Str)){str_trim(src, flags), {}};

    PAIR(Str, Str)
    pair = {
        .a = {.len = found, .buf = src.buf},
        .b = {.len = src.len - found - 1, .buf = &src.buf[found + 1]},
    };

    pair.a = str_trim(pair.a, flags);
    pair.b = str_trim(pair.b, flags);

    return pair;
}

SI Arr(Str) str_splitc(Buf* b, Str src, char sep, isize maxlen, StrSplitFlags flags)
{
    if (maxlen == -1) maxlen = buf_avail(b, sizeof(Str));
    if (!maxlen) return (Arr(Str)){};

    char* start    = &src.buf[0];
    Arr(Str) parts = {.len = maxlen, .buf = Make(b, Str, maxlen, ALLOC_NOZERO)};
    isize count    = 0;
    RANGE(i, src.len)
    {
        if (sep == src.buf[i])
        {
            isize len = &src.buf[i] - start;
            if (len || !(flags & SPLIT_IGNORE_EMPTY))
            {
                parts.buf[count] = (Str){.buf = start, .len = len};
                count++;
                if (count >= maxlen) goto __done;
            }
            if (flags & SPLIT_SUBSTITUTE_NULL) src.buf[i] = '\0';
            start = &src.buf[i] + 1; // Skip delimiter
        }
    }

    if ((isize)(start - src.buf) <= src.len)
    {
        isize len = src.len - (start - src.buf);
        if (len || !(flags & SPLIT_IGNORE_EMPTY))
        {
            parts.buf[count] = (Str){.buf = start, .len = len};
            count++;
        }
    }

__done:
    parts.len  = count;
    b->len    -= sizeof(Str) * (maxlen - count);

    return parts;
}

SI Arr(Str) str_split_lines(Buf* b, Str src, isize maxlen, bool ignore_empty)
{
    if (maxlen == -1) maxlen = buf_avail(b, sizeof(Str));
    StrSplitFlags flags = ignore_empty ? SPLIT_IGNORE_EMPTY : SPLIT_DEFAULT;
    return str_splitc(b, src, '\n', maxlen, flags);
}

// --------------- Iterator ---------------

SI Str str_till_next(Str* src, char c)
{
    char* start = src->buf;
    while ((src->buf[0] != c) && (src->len > 0))
    {
        src->buf++;
        src->len--;
    }
    Str dst   = {src->buf - start, start};
    src->buf += src->len > 0;
    src->len -= src->len > 0;
    return dst;
}

// BUG: Currently too complicated. Why? Could use auxiliary str_find func instead?
SI Str str_till_next2(Str* src, Str s)
{
    isize pos = str_find(*src, s);

    if (pos < 0)
    {
        Str dst   = *src;     // Return full string
        src->buf += src->len; // Advance to end
        src->len  = 0;
        return dst;
    }

    Str dst   = {pos, src->buf};
    src->buf += (pos + s.len);
    src->len -= (pos + s.len);

    return dst;
}
