#include "stz-c/builtin/set.h"

#include "stz-c/builtin/arr.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/set.h");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Set(Str)), 24L); }

    TEST_CASE("new, lookup, insert, delete")
    {
        Buf b = buf_new(1024);

        Set(Str) m    = set_new(Str)(&b, 4);
        Arr(Str) keys = arr_new(Str, &b, CapFromExp(m.exp), ALLOC_NOZERO);
        RANGE(i, CapFromExp(m.exp)) { keys.buf[i] = str_fmt(&b, "key-%ld", i); }

        // Insert till capacity
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(set_insert(Str)(&m, keys.buf[i]) >= 0); }
        EXPECT_FALSE(set_insert(Str)(&m, _("a")) >= 0);

        // Check that all are found
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(set_lookup(Str)(&m, keys.buf[i]) >= 0); }
        EXPECT_FALSE(set_lookup(Str)(&m, _("a")) >= 0);

        // Check again, to make sure no mutation during lookup
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(set_lookup(Str)(&m, keys.buf[i]) >= 0); }
        EXPECT_FALSE(set_lookup(Str)(&m, _("a")) >= 0);

        // Delete all keys
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(set_delete(Str)(&m, keys.buf[i]) >= 0); }
        EXPECT_FALSE(set_lookup(Str)(&m, _("a")) >= 0);

        // Ensure all deleted
        RANGE(i, CapFromExp(m.exp)) { EXPECT_FALSE(set_lookup(Str)(&m, keys.buf[i]) >= 0); }
        EXPECT_FALSE(set_lookup(Str)(&m, _("a")) >= 0);

        buf_free(&b);
    }

    return TEST_RESULTS();
}
