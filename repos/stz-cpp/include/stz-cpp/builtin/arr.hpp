#pragma once

#include "stz-cpp/builtin/buf.hpp"

#include <cassert>

template <typename T>
struct Arr
{
    isize len;
    T*    buf;

    // Alloc from buffer
    Arr(Buf* b, isize count, AllocFlags flags);

    // Shrink given buffer
    void Shrink(Buf* b, isize newlen);

    // Easy element access
    T&     First();
    T&     Last();
    T&     operator[](isize i);
    Arr<T> operator[](isize i, isize j);
};

template <typename T>
inline Arr<T>::Arr(Buf* b, isize count, AllocFlags flags)
    : len(count),
      buf(b->Make<T>(count, flags))
{
}

template <typename T>
inline void Arr<T>::Shrink(Buf* b, isize newlen)
{
    assert((newlen <= len) && b->OnTop(buf, len * sizeof(T)));
    b->len -= (len - newlen) * sizeof(T);
    len     = newlen;
}

// clang-format off

template <typename T> inline T&     Arr<T>::First() { return &buf[0]; }
template <typename T> inline T&     Arr<T>::Last()  { return &buf[len - 1]; }
template <typename T> inline T&     Arr<T>::operator[](isize i) { return &buf[i]; }
template <typename T> inline Arr<T> Arr<T>::operator[](isize i, isize j)
{
    if ((i < 0) || (j > len) || (i > j)) { return {}; } // Bounds check
    return {j - i, &buf[i]};                            // Finally, the normal case (includes empty string)
}

// clang-format on
