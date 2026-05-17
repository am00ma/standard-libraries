#include "stz-cpp/builtin/arr.hpp"
#include "stz-cpp/builtin/set.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/set.hpp");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Set<Str>), 24L); }

    TEST_CASE("new, lookup, insert, delete")
    {
        Buf b(1024);

        auto m    = Set<Str>(&b, 4);
        auto keys = Arr<Str>(&b, CapFromExp(m.exp), AllocFlags::NOZERO);
        RANGE(i, CapFromExp(m.exp)) { keys.buf[i] = str_fmt(&b, "key-%ld", i); }

        // Insert till capacity
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(m.Insert(keys.buf[i]) >= 0); }
        EXPECT_FALSE(m.Insert("a") >= 0);

        // Check that all are found
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(m.Lookup(keys.buf[i]) >= 0); }
        EXPECT_FALSE(m.Lookup("a") >= 0);

        // Check again, to make sure no mutation during lookup
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(m.Lookup(keys.buf[i]) >= 0); }
        EXPECT_FALSE(m.Lookup("a") >= 0);

        // Delete all keys
        RANGE(i, CapFromExp(m.exp)) { EXPECT_TRUE(m.Delete(keys.buf[i]) >= 0); }
        EXPECT_FALSE(m.Lookup("a") >= 0);

        // Ensure all deleted
        RANGE(i, CapFromExp(m.exp)) { EXPECT_FALSE(m.Lookup(keys.buf[i]) >= 0); }
        EXPECT_FALSE(m.Lookup("a") >= 0);

        b.Free();
    }
    return TEST_RESULTS();
}
