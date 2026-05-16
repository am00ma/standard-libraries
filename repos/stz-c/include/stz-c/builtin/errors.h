#pragma once

#include "stz-c/builtin/print.h"

/* Error behaviours:
 *      1. Assert  : Drop to debugger
 *      2. Warn    : Do nothing
 *      3. Continue: In case of loops
 *      4. Break   : In case of loops
 *      5. Goto    : Needs label
 *      6. Return  : Needs return val, in case of function
 *      7. Fatal   : exit(1)
 */

// clang-format off

#define OnError(log_fn, cond, behaviour, fmt, ...)                              \
    if (cond)                                                                   \
    {                                                                           \
        log_fn("%s:%-4d : %s()", __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__); \
        log_fn("  " fmt, ##__VA_ARGS__);                                        \
        behaviour;                                                              \
    }

#define Assert(cond, fmt, ...)              OnError(PrintError, !(cond), __builtin_trap(), fmt, ##__VA_ARGS__)
#define OnError_Warn(cond, fmt, ...)        OnError(PrintWarn ,  (cond), (void)0,          fmt, ##__VA_ARGS__)
#define OnError_Continue(cond, fmt, ...)    OnError(PrintWarn ,  (cond), continue,         fmt, ##__VA_ARGS__)
#define OnError_Break(cond, fmt, ...)       OnError(PrintWarn ,  (cond), break,            fmt, ##__VA_ARGS__)
#define OnError_Goto(cond, label, fmt, ...) OnError(PrintError,  (cond), goto label,       fmt, ##__VA_ARGS__)
#define OnError_Return(cond, val, fmt, ...) OnError(PrintError,  (cond), return (val),     fmt, ##__VA_ARGS__)
#define OnError_Fatal(cond, fmt, ...)       OnError(PrintError,  (cond), exit(1),          fmt, ##__VA_ARGS__)

// clang-format on
