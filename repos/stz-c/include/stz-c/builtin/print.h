#pragma once

#include "stz-c/builtin/types.h"

#include <stdio.h> // IWYU pragma: keep

// --------------- Styles ---------------

// Terminal ANSI styles
#define COLOR_BOLD   "\033[1m"
#define COLOR_ITALIC "\033[3m"
#define COLOR_ULINE  "\033[4m"

// Terminal ANSI colors
#define COLOR_RESET   "\033[0m"
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

// Terminal hyperlink
#define STYLE_URL_START COLOR_BLUE COLOR_ULINE "\033]8;;"
#define STYLE_URL_MID   "\033\\"
#define STYLE_URL_END   "\033]8;;\033\\" COLOR_RESET

// --------------- Functions ---------------

#define PrintNewline()        PrintFunc(fprintf, stdout, "\n")
#define PrintInline(fmt, ...) PrintFunc(fprintf, stdout, fmt, ##__VA_ARGS__)
#define PrintLn(fmt, ...)     PrintFunc(fprintf, stdout, fmt "\n", ##__VA_ARGS__)

#define PrintStyleFunc(fn, dst, style, fmt, ...) fn(dst, style fmt COLOR_RESET, ##__VA_ARGS__)

#define PrintTitle(fmt, ...) PrintStyleFunc(fprintf, stderr, STYLE_TITLE, fmt "\n", ##__VA_ARGS__)
#define PrintInfo(fmt, ...)  PrintStyleFunc(fprintf, stderr, STYLE_INFO, fmt "\n", ##__VA_ARGS__)
#define PrintWarn(fmt, ...)  PrintStyleFunc(fprintf, stderr, STYLE_WARN, fmt "\n", ##__VA_ARGS__)
#define PrintError(fmt, ...) PrintStyleFunc(fprintf, stderr, STYLE_ERROR, fmt "\n", ##__VA_ARGS__)

#define PrintUrl(link, text) PrintFunc(fprintf, stdout, STYLE_URL_START link STYLE_URL_MID text STYLE_URL_END)
