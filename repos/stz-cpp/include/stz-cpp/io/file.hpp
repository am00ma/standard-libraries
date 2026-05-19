#pragma once

#include "stz-cpp/stz-cpp.hpp" // IWYU pragma: keep

#include <cerrno>     // IWYU pragma: keep
#include <libgen.h>   // basename, dirname
#include <sys/stat.h> // stat
#include <unistd.h>   // access, F_OK

// --------------- Definitions ---------------

bool     file_exists(Str path);
Res<i64> file_size(Str path);
Res<Str> file_read(Buf* b, Str path);
Res<u64> file_write(Str path, Str text, const char* mode);

// --------------- Implementations ---------------

inline bool file_exists(Str path)
{
    if (!path.buf || !path.len) return false;
    if (!IsNullTerm(path)) return false;

    struct stat info = {};

    int err = stat(path.buf, &info);
    if (err != 0) return false; // TODO: stat error != file does not exist

    return S_ISDIR(info.st_mode)    //
           || S_ISREG(info.st_mode) //
           || S_ISLNK(info.st_mode);
}

inline Res<i64> file_size(Str path)
{
    if (!path.buf || !path.len) return {EBADF, 0};
    if (!IsNullTerm(path)) return {EBADF, 0};

    struct stat info = {};

    int err = stat(path.buf, &info);
    if (err) return {ENOENT, 0}; // TODO: stat error != file does not exist

    return {0, info.st_size};
}

inline Res<Str> file_read(Buf* b, Str path)
{
    if (!path.buf || !path.len) return {EBADF, StrNull};
    if (!IsNullTerm(path)) return {EBADF, StrNull};

    int err = 0;

    struct stat info = {};

    err = stat(path.buf, &info);
    if (err) return {ENOENT, StrNull}; // TODO: stat error != file does not exist

    if (S_ISDIR(info.st_mode)) return {EBADF, StrNull}; // Expected file

    FILE* f = fopen(path.buf, "rb");
    if (f == NULL) return {errno, StrNull};

    i64 size  = info.st_size;
    u64 nread = 0;
    Str data  = {};

    data  = Str(b, size);
    nread = fread(data.buf, sizeof(char), static_cast<usize>(data.len), f);
    err   = nread != static_cast<usize>(data.len) ? EIO : 0;

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return {err, StrNull};

    // On success return success and data
    return {0, data};
}

inline Res<u64> file_write(Str path, Str src, const char* mode)
{
    if (!path.buf || !path.len) return {EBADF, 0};
    if (!IsNullTerm(path)) return {EBADF, 0};

    int err = 0;

    FILE* f = fopen(path.buf, mode);
    if (f == NULL) return {errno, 0};

    u64 nwritten = 0;

    nwritten = fwrite(src.buf, sizeof(char), static_cast<usize>(src.len), f);
    err      = nwritten != (u64)src.len ? EIO : 0;

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return {err, 0};

    // On success return success and data
    return {0, nwritten};
}
