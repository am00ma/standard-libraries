#pragma once

#include "stz-c/builtin/hash.h"

// Prerequisites: defined in hash.h
// HASH(T)
// ISEQUAL(T)
// ISEMPTY(T)
// ISTOMB(T)
// SETTOMB(T)

// --------------- Definitions ---------------

// Generic Map
#define MAP(A, B) CONCAT(CONCAT(Map_, A), B)

#define map_new(A, B)    CONCAT(CONCAT(CONCAT(Map_, A), B), _new)
#define map_lookup(A, B) CONCAT(CONCAT(CONCAT(Map_, A), B), _lookup)
#define map_insert(A, B) CONCAT(CONCAT(CONCAT(Map_, A), B), _insert)
#define map_delete(A, B) CONCAT(CONCAT(CONCAT(Map_, A), B), _delete)

// --------------- Example of specialization ---------------

// Specialization for string, string
typedef struct
{
    isize len;
    isize exp; // Exponent to power of 2
    PAIR(Str, Str) * buf;

} MAP(Str, Str);

// clang-format off

SI MAP(Str, Str) map_new(Str, Str)(Buf* b, isize exp);
SI Str*          map_lookup(Str, Str)(MAP(Str, Str)* m, Str key);
SI int           map_insert(Str, Str)(MAP(Str, Str)* m, Str key, Str val);
SI int           map_delete(Str, Str)(MAP(Str, Str)* m, Str key);

// clang-format on

// --------------- Implementation ---------------

SI MAP(Str, Str) map_new(Str, Str)(Buf* b, isize exp)
{
    return (MAP(Str, Str)){
        .len = 0,
        .exp = exp,
        // ALLOC_ZERO needed to set .buf = 0 which marks empty slot
        .buf = Make(b, PAIR(Str, Str), CapFromExp(exp), ALLOC_ZERO),
    };
}

SI Str* map_lookup(Str, Str)(MAP(Str, Str) * m, Str key)
{
    if (!m->len) { return NULL; } // empty set

    u64 hash  = HASH64(Str)(key);
    i32 count = 0;
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (ISEMPTY(Str)(m->buf[i].a)) { return NULL; }                   // found empty slot
        else if (ISTOMB(Str)(m->buf[i].a)) { continue; }                  // found gravestone
        else if (ISEQUAL(Str)(key, m->buf[i].a)) { return &m->buf[i].b; } // found matching key
        if ((count++) >= m->len) { return NULL; } // TODO: necessary? no slot found after full iteration
    }
}

SI int map_insert(Str, Str)(MAP(Str, Str) * m, Str key, Str val)
{
    if ((m->len + 1) > CapFromExp(m->exp)) { return -1; } // overflows capacity

    u64 hash = HASH64(Str)(key);
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (ISEMPTY(Str)(m->buf[i].a) || ISTOMB(Str)(m->buf[i].a)) // found empty slot or gravestone, insert
        {
            m->buf[i].a = key;
            m->buf[i].b = val;
            m->len++;
            return i;
        }
        else if (ISEQUAL(Str)(key, m->buf[i].a)) // found filled slot, overwrite
        {
            return i;
        }
    }
}

SI int map_delete(Str, Str)(MAP(Str, Str) * m, Str key)
{
    if (!m->len) { return -1; } // empty set

    u64 hash  = HASH64(Str)(key);
    i32 count = 0;
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (ISEMPTY(Str)(m->buf[i].a)) { return -1; }    // found empty slot
        else if (ISTOMB(Str)(m->buf[i].a)) { continue; } // found tombstone
        else if (ISEQUAL(Str)(key, m->buf[i].a))         // found matching key
        {
            SETTOMB(Str)(m->buf[i].a); // insert tombstone
            m->len--;
            return 0; // successful deletion
        }
        if ((count++) >= m->len) { return -1; } // TODO: necessary? no slot found after full iteration
    }
}
