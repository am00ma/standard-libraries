#pragma once

#include "stz-cpp/builtin/buf.hpp"
#include "stz-cpp/builtin/str.hpp"

#include <cstdio> // IWYU pragma: keep

/* ---------------------------------------------------------------------------
 * Pretty printing
 * ------------------------------------------------------------------------- */
#define COLOR_RESET "\033[0m"

// Prototypes
#define PrintFunc(fn, dst, fmt, ...)             fn(dst, fmt, ##__VA_ARGS__)
#define PrintStyleFunc(fn, dst, style, fmt, ...) fn(dst, style fmt COLOR_RESET, ##__VA_ARGS__)

// Print to stdout
#define PrintNewline()        PrintFunc(fprintf, stdout, "\n")
#define PrintInline(fmt, ...) PrintFunc(fprintf, stdout, fmt, ##__VA_ARGS__)
#define PrintLn(fmt, ...)     PrintFunc(fprintf, stdout, fmt "\n", ##__VA_ARGS__)

// Terminal ANSI styles
#define COLOR_BOLD   "\033[1m"
#define COLOR_ITALIC "\033[3m"
#define COLOR_ULINE  "\033[4m"

// Terminal ANSI colors
#define COLOR_BLACK   "\033[0;30m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_WHITE   "\033[0;37m"

// Title, Info, Warn, Error
#define STYLE_TITLE COLOR_YELLOW COLOR_BOLD COLOR_ITALIC COLOR_ULINE
#define STYLE_INFO  COLOR_WHITE
#define STYLE_WARN  COLOR_YELLOW
#define STYLE_ERROR COLOR_RED

#define PrintTitle(fmt, ...) PrintStyleFunc(fprintf, stderr, STYLE_TITLE, fmt "\n", ##__VA_ARGS__)
#define PrintInfo(fmt, ...)  PrintStyleFunc(fprintf, stderr, STYLE_INFO, fmt "\n", ##__VA_ARGS__)
#define PrintWarn(fmt, ...)  PrintStyleFunc(fprintf, stderr, STYLE_WARN, fmt "\n", ##__VA_ARGS__)
#define PrintError(fmt, ...) PrintStyleFunc(fprintf, stderr, STYLE_ERROR, fmt "\n", ##__VA_ARGS__)

// Terminal hyperlink
#define STYLE_URL_START      COLOR_BLUE COLOR_ULINE "\033]8;;"
#define STYLE_URL_MID        "\033\\"
#define STYLE_URL_END        "\033]8;;\033\\" COLOR_RESET
#define PrintUrl(link, text) PrintFunc(fprintf, stdout, STYLE_URL_START link STYLE_URL_MID text STYLE_URL_END)

/* ---------------------------------------------------------------------------
 * Printing generics
 * ------------------------------------------------------------------------- */

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
