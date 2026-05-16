#pragma once

#include "stz-c/builtin/str.h"

bool path_exists(Str path);
Str  path_basename(Buf* b, Str path);
Str  path_dirname(Buf* b, Str path);
Str  path_extension(Buf* b, Str path);
Str  path_join(Buf* b, Str parent, Str path);

int path_mkdir(Str path, int mode);
int path_delete(Str path);
Res(Str) path_read_text(Buf* b, Str path);
int path_write_text(Str path, Str src, const char* mode);
