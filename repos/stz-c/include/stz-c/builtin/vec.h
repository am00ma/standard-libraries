#pragma once

#include "stz-c/builtin/types.h"

#include <assert.h>

// Vector
#define Vec(T) CONCAT(Vec_, T)

// clang-format off

#define DECLARE_VECTOR(T) \
    typedef struct        \
    {                     \
        isize len;        \
        isize cap;        \
        T*    buf;        \
    } Vec(T);

// clang-format on
