#include "stz-c/builtin/set.h"

#include "stz-c/builtin/arr.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/set.h");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(SET(Str)), 24L); }

    TEST_CASE("set_new(T)(buf, exp)")
    {
        Buf b = buf_new(1024);

        SET(Str) set = set_new(Str)(&b, 4);
        EXPECT_EQ_LONG(CapFromExp(set.exp), 16L);

        Arr(Str) keys = arr_new(Str, &b, CapFromExp(set.exp), ALLOC_NOZERO);
        EXPECT_EQ_LONG(keys.len, 16L);
        // RANGE(i, CapFromExp(set.exp)) { keys.buf[i] = str_fmt(&b, "key-%ld", i); }

        buf_free(&b);
    }

    return TEST_RESULTS();
}
