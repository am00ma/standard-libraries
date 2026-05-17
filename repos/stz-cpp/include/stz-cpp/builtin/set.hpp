#pragma once

#include "stz-cpp/builtin/hash.hpp"

// --------------- Definitions ---------------

// Generic set
template <typename T>
struct Set
{
    isize len;
    isize exp; // Exponent to power of 2
    T*    buf;

    Set(Buf* b, isize exp);

    int Lookup(T key);
    int Insert(T key);
    int Delete(T key);
};

// --------------- Implementation ---------------

template <typename T>
Set<T>::Set(Buf* b, isize exponent)
{
    len = 0;
    exp = exponent;

    // ALLOC_ZERO needed to set .buf = 0 which marks empty slot
    buf = b->Make<Str>(CapFromExp(exponent), AllocFlags::ZERO);
}

template <typename T>
int Set<T>::Lookup(T key)
{
    if (!len) { return -1; } // empty set

    u64 hash  = Hash64(key);
    i32 count = 0;
    for (i32 i = static_cast<i32>(hash);;)
    {
        i = hash64_msi_next(hash, exp, i);
        if (IsEmpty(buf[i])) { return -1; }          // found empty slot
        else if (IsTomb(buf[i])) { continue; }       // found gravestone
        else if (IsEqual(key, buf[i])) { return i; } // found matching key
        if ((count++) >= len) { return -1; }         // TODO: necessary? no slot found after full iteration
    }
}

template <typename T>
int Set<T>::Insert(T key)
{
    if ((len + 1) > CapFromExp(exp)) { return -1; } // overflows capacity

    u64 hash = Hash64(key);
    for (i32 i = static_cast<i32>(hash);;)
    {
        i = hash64_msi_next(hash, exp, i);
        if (IsEmpty(buf[i]) || IsTomb(buf[i])) // found empty slot or gravestone, insert
        {
            buf[i] = key;
            len++;
            return i;
        }
        else if (IsEqual(key, buf[i])) // found filled slot, overwrite
        {
            return i;
        }
    }
}

template <typename T>
int Set<T>::Delete(T key)
{
    if (!len) { return -1; } // empty set

    u64 hash  = Hash64(key);
    i32 count = 0;
    for (i32 i = static_cast<i32>(hash);;)
    {
        i = hash64_msi_next(hash, exp, i);
        if (IsEmpty(buf[i])) { return -1; }    // found empty slot
        else if (IsTomb(buf[i])) { continue; } // found tombstone
        else if (IsEqual(key, buf[i]))         // found matching key
        {
            SetTomb(buf[i]); // insert tombstone
            len--;
            return i;
        } // found matching key
        if ((count++) >= len) { return -1; } // TODO: necessary? no slot found after full iteration
    }
}
