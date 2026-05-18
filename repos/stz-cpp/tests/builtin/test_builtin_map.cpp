#include "stz-cpp/builtin/map.hpp"

#include "stz-cpp/builtin/arr.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/map.hpp");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Map<Str, Str>), 24L); }

    TEST_CASE("new, lookup, insert, delete")
    {
        Buf b(1024 * 1024); // MB

        Map<Str, Str> m(&b, 4);

        Arr<Str> keys(&b, CapFromExp(m.exp), AllocFlags::NOZERO);
        Arr<Str> vals(&b, CapFromExp(m.exp), AllocFlags::NOZERO);
        RANGE(i, CapFromExp(m.exp))
        {
            keys.buf[i] = str_fmt(&b, "key-%ld", i);
            vals.buf[i] = str_fmt(&b, "val-%ld", i);
        }

        // Insert till capacity
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(m.Insert(keys.buf[i], vals.buf[i]) >= 0); }
        EXPECT_FALSE(m.Insert("a", "b") >= 0);

        // Check that all are found
        RANGE(i, CapFromExp(m.exp))
        {
            Str* val;
            val = m.Lookup(keys.buf[i]);
            EXPECT_NEQ_NULL(val);
            EXPECT_EQ_STR(*val, vals.buf[i]);
        }
        EXPECT_FALSE(m.Insert("a", "b") >= 0);

        // Check again, to make sure no mutation during lookup
        RANGE(i, CapFromExp(m.exp))
        {
            Str* val;
            val = m.Lookup(keys.buf[i]);
            EXPECT_NEQ_NULL(val);
            EXPECT_EQ_STR(*val, vals.buf[i]);
        }
        EXPECT_FALSE(m.Insert("a", "b") >= 0);

        // Delete all keys
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(m.Delete(keys.buf[i]) == 0); }

        // Ensure all deleted
        RANGE(i, CapFromExp(m.exp)) { EXPECT_EQ_NULL(m.Lookup(keys.buf[i])); }

        // Final check
        EXPECT_TRUE(m.Insert("a", "b") >= 0);
        Str* val;
        val = m.Lookup("a");
        EXPECT_NEQ_NULL(val);
        EXPECT_EQ_STR(*val, Str("b"));
        EXPECT_EQ_LONG(m.len, 1L);

        b.Free();
    }

    return TEST_RESULTS();
}
