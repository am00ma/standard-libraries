#pragma once

#include "stz-cpp/builtin/buf.hpp" // IWYU pragma: keep

#include <assert.h>

template <typename T>
struct Vec
{
    isize len;
    isize cap;
    T*    buf;
};
