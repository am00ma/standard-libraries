#pragma once

#include "stz-c/builtin/hash.h"

// Prerequisites: defined in hash.h
// HASH(T)
// ISEQUAL(T)
// ISEMPTY(T)
// ISTOMB(T)

// --------------- Definitions ---------------

// Generic set
#define SET(T) CONCAT(Set_, T)

#define set_new(T)    CONCAT(CONCAT(Set_, T), _new)
#define set_lookup(T) CONCAT(CONCAT(Set_, T), _lookup)
#define set_insert(T) CONCAT(CONCAT(Set_, T), _insert)
#define set_delete(T) CONCAT(CONCAT(Set_, T), _delete)

// Specialization for string
typedef struct
{
    Str*  buf;
    isize len;
    isize exp; // Exponent to power of 2

} SET(Str);

// clang-format off

SI SET(Str) set_new(Str)(Buf* b, isize exp);
SI int      set_lookup(Str)(SET(Str)* m, Str key);
SI int      set_insert(Str)(SET(Str)* m, Str key);
SI int      set_delete(Str)(SET(Str)* m, Str key);

// clang-format on

// --------------- Implementation ---------------

SI SET(Str) set_new(Str)(Buf* b, isize exp)
{
    return (SET(Str)){
        // ALLOC_ZERO needed to set .buf = 0 which marks empty slot
        .buf = Make(b, Str, CapFromExp(exp), ALLOC_ZERO),
        .len = 0,
        .exp = exp,
    };
}

SI int set_lookup(Str)(SET(Str) * m, Str key)
{
    if (!m->len) { return -1; } // empty set

    u64 hash  = HASH64(Str)(key);
    i32 count = 0;
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (ISEMPTY(Str)(m->buf[i])) { return -1; }          // found empty slot
        else if (ISTOMB(Str)(m->buf[i])) { continue; }       // found gravestone
        else if (ISEQUAL(Str)(key, m->buf[i])) { return i; } // found matching key
        if ((count++) >= m->len) { return -1; }              // TODO: necessary? no slot found after full iteration
    }
}

SI int set_insert(Str)(SET(Str) * m, Str key)
{
    if ((m->len + 1) > CapFromExp(m->exp)) { return -1; } // overflows capacity

    u64 hash = HASH64(Str)(key);
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (ISEMPTY(Str)(m->buf[i]) || ISTOMB(Str)(m->buf[i])) // found empty slot or gravestone, insert
        {
            m->buf[i] = key;
            m->len++;
            return i;
        }
        else if (ISEQUAL(Str)(key, m->buf[i])) // found filled slot, overwrite
        {
            return i;
        }
    }
}

SI int set_delete(Str)(SET(Str) * m, Str key)
{
    if (!m->len) { return -1; } // empty set

    u64 hash  = HASH64(Str)(key);
    i32 count = 0;
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (ISEMPTY(Str)(m->buf[i])) { return -1; }     // found empty slot
        else if (ISTOMB(Str)(m->buf[i])) { return -1; } // found gravestone
        else if (ISEQUAL(Str)(key, m->buf[i]))          // found matching key
        {
            SETTOMB(Str)(m->buf[i]); // insert tombstone
            m->len--;
            return i;
        }
        if ((count++) >= m->len) { return -1; } // TODO: necessary? no slot found after full iteration
    }
}
