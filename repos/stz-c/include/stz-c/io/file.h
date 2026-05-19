#pragma once

#include "stz-c/stz-c.h" // IWYU pragma: keep

#include <errno.h>    // IWYU pragma: keep
#include <libgen.h>   // basename, dirname
#include <sys/stat.h> // stat
#include <unistd.h>   // access, F_OK

// --------------- Definitions ---------------

bool file_exists(Str path);
Res(i64) file_size(Str path);
Res(Str) file_read(Buf* b, Str path);
Res(u64) file_write(Str path, Str text, const char* mode);

// --------------- Implementations ---------------

inline bool file_exists(Str path)
{
    if (!path.buf || !path.len) return false;
    if (!IsNullTerm(path)) return false;

    struct stat info = {};

    int err = stat(path.buf, &info);
    if (err) return false; // TODO: stat error != file does not exist

    return S_ISDIR(info.st_mode)    //
           || S_ISREG(info.st_mode) //
           || S_ISLNK(info.st_mode);
}

inline Res(i64) file_size(Str path)
{
    if (!path.buf || !path.len) return (Res(i64)){EBADF, 0};
    if (!IsNullTerm(path)) return (Res(i64)){EBADF, 0};

    struct stat info = {};

    int err = stat(path.buf, &info);
    if (err) return (Res(i64)){ENOENT, 0}; // TODO: stat error != file does not exist

    if (S_ISDIR(info.st_mode)) return (Res(i64)){EBADF, 0};

    return (Res(i64)){0, info.st_size};
}

inline Res(Str) file_read(Buf* b, Str path)
{
    if (!path.buf || !path.len) return (Res(Str)){EBADF, StrNull};
    if (!IsNullTerm(path)) return (Res(Str)){EBADF, StrNull};

    int err = 0;

    struct stat info = {};

    err = stat(path.buf, &info);
    if (err) return (Res(Str)){ENOENT, StrNull}; // TODO: stat error != file does not exist

    if (S_ISDIR(info.st_mode)) return (Res(Str)){EBADF, StrNull}; // Expected file

    FILE* f = fopen(path.buf, "rb");
    if (f == NULL) return (Res(Str)){errno, StrNull};

    i64 size  = info.st_size;
    u64 nread = 0;
    Str data  = {};

    data  = str_new(b, size);
    nread = fread(data.buf, sizeof(char), data.len, f);
    err   = nread != (u64)data.len ? EIO : 0;

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return (Res(Str)){err, StrNull};

    // On success return success and data
    return (Res(Str)){0, data};
}

inline Res(u64) file_write(Str path, Str src, const char* mode)
{
    if (!path.buf || !path.len) return (Res(u64)){EBADF, 0};
    if (!IsNullTerm(path)) return (Res(u64)){EBADF, 0};

    int err = 0;

    FILE* f = fopen(path.buf, mode);
    if (f == NULL) return (Res(u64)){errno, 0};

    u64 nwritten = 0;

    nwritten = fwrite(src.buf, sizeof(char), src.len, f);
    err      = nwritten != (u64)src.len ? EIO : 0;

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return (Res(u64)){err, 0};

    // On success return success and data
    return (Res(u64)){0, nwritten};
}
