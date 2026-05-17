#pragma once

#include "stz-c/builtin/hash.h"

// --------------- Definitions ---------------

#define CapFromExp(exp) (1 << exp)

// Generic set
#define SET(T) CONCAT(Set_, T)

// Generic tomb
#define TOMB(T) CONCAT(Tomb_, T)

// Other generics needed
// HASH(T): defined in hash.h

#define Tomb_Str (Str){-1, 0}

// Specialization for string
typedef struct
{
    Str*  buf;
    isize len;
    isize exp; // Exponent to power of 2

} SET(Str);

SI     SET(Str) strset_new(Buf* b, isize exp);
SI int strset_lookup(SET(Str) * m, Str key);
SI int strset_insert(SET(Str) * m, Str key);
SI int strset_delete(SET(Str) * m, Str key);

// --------------- Implementation ---------------

SI SET(Str) strset_new(Buf* b, isize exp)
{
    return (SET(Str)){
        // ALLOC_ZERO needed to set .buf = 0 which marks empty slot
        .buf = Make(b, Str, CapFromExp(exp), ALLOC_ZERO),
        .len = 0,
        .exp = exp,
    };
}

SI int strset_lookup(SET(Str) * m, Str key)
{
    if (!m->len) { return -1; } // empty set

    u64 hash  = HASH64(Str)(key);
    i32 count = 0;
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (!m->buf[i].buf && !m->buf[i].len) { return -1; }            // found empty slot
        else if (!m->buf[i].buf && (m->buf[i].len == -1)) { continue; } // found gravestone
        else if (str_equal(key, m->buf[i])) { return i; }               // found filled slot
        if ((count++) >= m->len) { return -1; } // BUG: necessary? no slot found after full iteration
    }
}

SI int strset_insert(SET(Str) * m, Str key)
{
    if ((m->len + 1) > (1 << m->exp)) { return -1; } // overflows capacity

    u64 hash = HASH64(Str)(key);
    for (i32 i = hash;;)
    {
        i = hash64_msi_next(hash, m->exp, i);
        if (!m->buf[i].buf) // found empty slot or gravestone, insert
        {
            m->buf[i] = key;
            m->len++;
            return i;
        }
        else if (str_equal(key, m->buf[i])) // found filled slot, overwrite
        {
            return i;
        }
    }
}

SI int strset_delete(SET(Str) * m, Str key)
{
    int idx = strset_lookup(m, key);
    if (idx < 0) { return -1; } // not found
    m->buf[idx] = TOMB(Str);    // insert gravestone
    m->len--;
    return idx;
}
