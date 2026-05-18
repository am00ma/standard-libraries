#include "stz-c/builtin/map.h"

#include "stz-c/builtin/arr.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/map.h");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(MAP(Str, Str)), 24L); }

    TEST_CASE("new, lookup, insert, delete")
    {
        Buf b = buf_new(1024 * 1024); // MB

        MAP(Str, Str) m = map_new(Str, Str)(&b, 4);

        Arr(Str) keys = arr_new(Str, &b, CapFromExp(m.exp), ALLOC_NOZERO);
        Arr(Str) vals = arr_new(Str, &b, CapFromExp(m.exp), ALLOC_NOZERO);
        RANGE(i, CapFromExp(m.exp))
        {
            keys.buf[i] = str_fmt(&b, "key-%ld", i);
            vals.buf[i] = str_fmt(&b, "val-%ld", i);
        }

        // Insert till capacity
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(map_insert(Str, Str)(&m, keys.buf[i], vals.buf[i]) >= 0); }
        EXPECT_FALSE(map_insert(Str, Str)(&m, _("a"), _("b")) >= 0);

        // Check that all are found
        RANGE(i, CapFromExp(m.exp))
        {
            Str* val;
            val = map_lookup(Str, Str)(&m, keys.buf[i]);
            EXPECT_NEQ_NULL(val);
            EXPECT_EQ_STR(*val, vals.buf[i]);
        }
        EXPECT_FALSE(map_insert(Str, Str)(&m, _("a"), _("b")) >= 0);

        // Check again, to make sure no mutation during lookup
        RANGE(i, CapFromExp(m.exp))
        {
            Str* val;
            val = map_lookup(Str, Str)(&m, keys.buf[i]);
            EXPECT_NEQ_NULL(val);
            EXPECT_EQ_STR(*val, vals.buf[i]);
        }
        EXPECT_FALSE(map_insert(Str, Str)(&m, _("a"), _("b")) >= 0);

        // Delete all keys
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(map_delete(Str, Str)(&m, keys.buf[i]) == 0); }

        // Ensure all deleted
        RANGE(i, CapFromExp(m.exp)) { EXPECT_EQ_NULL(map_lookup(Str, Str)(&m, keys.buf[i])); }

        // Final check
        EXPECT_TRUE(map_insert(Str, Str)(&m, _("a"), _("b")) >= 0);
        Str* val;
        val = map_lookup(Str, Str)(&m, _("a"));
        EXPECT_NEQ_NULL(val);
        EXPECT_EQ_STR(*val, _("b"));
        EXPECT_EQ_LONG(m.len, 1L);

        buf_free(&b);
    }

    return TEST_RESULTS();
}
