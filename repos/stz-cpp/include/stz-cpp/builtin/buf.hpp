#pragma once

#include "stz-cpp/builtin/types.hpp"

#include <cassert> // assert
#include <cstdlib> // malloc
#include <cstring> // memset

// --------------- Definitions ---------------

enum class AllocFlags : int
{
    NOZERO   = 0x0,
    ZERO     = 0x1,
    SOFTFAIL = 0x2,
};

struct Buf
{
    isize len = 0;
    isize cap = 0;
    char* buf = nullptr;

    // Default constructors that C already gives
    Buf() = default;
    Buf(isize length, isize capacity, char* buffer);

    // Constructors
    Buf(isize capacity);
    Buf(Buf* src, isize capacity, AllocFlags flags);

    // 'Destructors'
    void Shrink(Buf* src);
    void Reset();
    void Free();

    // Allocation
    char* Alloc(usize objsize, usize align, isize count, AllocFlags flags);

    template <typename T>
    T* Make(isize count, AllocFlags flags);

    // Measurements
    template <typename T>
    isize Avail();
    bool  OnTop(void* buf, isize len);
};

#define buf_stack(name, capacity)                                                                                      \
    char tempbuf__##name[(capacity)] = {};                                                                             \
    Buf  name{0, (capacity), (char*)tempbuf__##name};

// --------------- Implementation ---------------

inline char* Buf::Alloc(usize objsize, usize align, isize count, AllocFlags flags)
{
    if (count < 0) { __builtin_trap(); } // Got negative count

    char* beg = &buf[len];

    if (count == 0) return beg;

    isize pad = static_cast<isize>(-(uptr)beg & (align - 1)); // Works as align is power of 2
    if (count > (cap - len) / (isize)objsize)
    {
        if (static_cast<int>(flags) & static_cast<int>(AllocFlags::SOFTFAIL)) return 0;
        else __builtin_trap(); // Exceeded capacity
    }

    isize total  = static_cast<isize>(count) * static_cast<isize>(objsize);
    char* p      = beg + pad;
    len         += pad + total;

    if ((static_cast<int>(flags) & static_cast<int>(AllocFlags::ZERO)))
    {
        p = (char*)memset(p, 0, static_cast<usize>(total));
    }

    return p;
}

template <typename T>
T* Buf::Make(isize count, AllocFlags flags)
{ return reinterpret_cast<T*>(Alloc(sizeof(T), alignof(T), count, flags)); }

inline Buf::Buf(isize length, isize capacity, char* buffer)
    : len(length),
      cap(capacity),
      buf(buffer)
{
}

inline Buf::Buf(isize capacity)
    : len(0),
      cap(0),
      buf(nullptr)
{
    if (capacity < 0) __builtin_trap(); // Requested negative capacity
    buf = (char*)malloc(static_cast<usize>(capacity));
    len = 0;
    cap = buf ? capacity : 0;
}

inline Buf::Buf(Buf* src, isize capacity, AllocFlags flags)
    : len(0),
      cap(0),
      buf(nullptr)
{
    buf = src->Make<char>(capacity, flags);
    cap = buf ? capacity : 0;
}

inline void Buf::Shrink(Buf* src)
{
    src->len -= cap - len;
    cap       = len;
}

inline void Buf::Reset() { len = 0; }

inline void Buf::Free()
{
    if (buf) { free(buf); }
    len = 0;
    cap = 0;
    buf = nullptr;
}

template <typename T>
inline isize Buf::Avail()
{ return (cap - len) / sizeof(T); }

inline bool Buf::OnTop(void* buffer, isize length) { return (len - length) == ((char*)buffer - buf); }
