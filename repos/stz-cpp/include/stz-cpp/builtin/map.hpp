#pragma once

#include "stz-cpp/builtin/hash.hpp"

// --------------- Definitions ---------------

// Generic map
template <typename K, typename V>
struct Map
{
    isize       len;
    isize       exp; // Exponent to power of 2
    Pair<K, V>* buf;

    Map(Buf* b, isize exp);

    V*  Lookup(K key);
    int Insert(K key, V val);
    int Delete(K key);
};

// --------------- Implementation ---------------

template <typename K, typename V>
Map<K, V>::Map(Buf* b, isize exponent)
{
    len = 0;
    exp = exponent;

    // ALLOC_ZERO needed to set .buf = 0 which marks empty slot
    buf = b->Make<Pair<K, V>>(CapFromExp(exponent), AllocFlags::ZERO);
}

template <typename K, typename V>
V* Map<K, V>::Lookup(K key)
{
    if (!len) { return nullptr; } // empty set

    u64 hash  = Hash64(key);
    i32 count = 0;
    for (i32 i = static_cast<i32>(hash);;)
    {
        i = hash64_msi_next(hash, exp, i);
        if (IsEmpty(buf[i].a)) { return nullptr; }             // found empty slot
        else if (IsTomb(buf[i].a)) { continue; }               // found gravestone
        else if (IsEqual(key, buf[i].a)) { return &buf[i].b; } // found matching key
        if ((count++) >= len) { return nullptr; }              // TODO: necessary? no slot found after full iteration
    }
}

template <typename K, typename V>
int Map<K, V>::Insert(K key, V val)
{
    if ((len + 1) > CapFromExp(exp)) { return -1; } // overflows capacity

    u64 hash = Hash64(key);
    for (i32 i = static_cast<i32>(hash);;)
    {
        i = hash64_msi_next(hash, exp, i);
        if (IsEmpty(buf[i].a) || IsTomb(buf[i].a)) // found empty slot or gravestone, insert
        {
            buf[i].a = key;
            buf[i].b = val;
            len++;
            return i;
        }
        else if (IsEqual(key, buf[i].a)) // found filled slot, overwrite
        {
            return i;
        }
    }
}

template <typename K, typename V>
int Map<K, V>::Delete(K key)
{
    if (!len) { return -1; } // empty set

    u64 hash  = Hash64(key);
    i32 count = 0;
    for (i32 i = static_cast<i32>(hash);;)
    {
        i = hash64_msi_next(hash, exp, i);
        if (IsEmpty(buf[i].a)) { return -1; }    // found empty slot
        else if (IsTomb(buf[i].a)) { continue; } // found tombstone
        else if (IsEqual(key, buf[i].a))         // found matching key
        {
            SetTomb(buf[i].a); // insert tombstone
            len--;
            return 0; // successful delete
        }
        if ((count++) >= len) { return -1; } // TODO: necessary? no slot found after full iteration
    }
}
