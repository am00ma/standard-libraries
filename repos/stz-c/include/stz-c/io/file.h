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
    if (err != 0) return false; // TODO: stat error != file does not exist

    return S_ISDIR(info.st_mode)    //
           || S_ISREG(info.st_mode) //
           || S_ISLNK(info.st_mode);
}

inline Res(i64) file_size(Str path)
{
    if (!file_exists(path)) return (Res(i64)){ENOENT, 0};

    int err = 0;

    FILE* f = fopen(path.buf, "rb");
    if (f == NULL) return (Res(i64)){errno, 0};

    // Get size
    i64 size = 0;

    err = fseek(f, 0, SEEK_END);
    if (err) goto lbl_close;
    size = ftell(f);
    err  = (size < 0) ? EINVAL : 0;
    if (err) goto lbl_close;

lbl_close:

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return (Res(i64)){err, 0};

    // On success return success and size
    return (Res(i64)){0, size};
}

inline Res(Str) file_read(Buf* b, Str path)
{
    if (!file_exists(path)) return (Res(Str)){ENOENT, StrNull};

    int err = 0;

    FILE* f = fopen(path.buf, "rb");
    if (f == NULL) return (Res(Str)){errno, StrNull};

    i64 size  = 0;
    u64 nread = 0;
    Str data  = {};

    err = fseek(f, 0, SEEK_END);
    if (err) goto lbl_close;
    size = ftell(f);
    err  = (size < 0) ? EINVAL : 0;
    if (err) goto lbl_close;
    err = fseek(f, 0, SEEK_SET);
    if (err) goto lbl_close;

    data = str_new(b, size);

    nread = fread(data.buf, sizeof(char), data.len, f);
    err   = nread != (u64)data.len ? EIO : 0;
    if (err) goto lbl_close;

lbl_close:

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return (Res(Str)){err, StrNull};

    // On success return success and data
    return (Res(Str)){0, data};
}

inline Res(u64) file_write(Str path, Str src, const char* mode)
{
    if (!file_exists(path)) return (Res(u64)){ENOENT, 0};

    int err = 0;

    FILE* f = fopen(path.buf, mode);
    if (f == NULL) return (Res(u64)){errno, 0};

    u64 nwritten = 0;

    nwritten = fwrite(src.buf, sizeof(char), src.len, f);
    err      = nwritten != (u64)src.len ? EIO : 0;
    if (err) goto lbl_close;

lbl_close:

    // Close irrespective of error, ignore error from close
    fclose(f);

    // On error, return error and null
    if (err) return (Res(u64)){err, 0};

    // On success return success and data
    return (Res(u64)){0, nwritten};
}
