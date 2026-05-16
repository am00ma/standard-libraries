#include "stz-c/builtin/arr.h"
#include "stz-c/builtin/buf.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/arr.h");

    TEST_CASE("Constructors")
    {
        buf_stack(b, 32);

        Arr(i32) a_i32 = arr_new(i32, &b, 4, ALLOC_ZERO);
        EXPECT_EQ_LONG(a_i32.len, 4L);
        EXPECT_EQ_LONG(b.len, 16L);

        RANGE(i, a_i32.len) { EXPECT_EQ_INT(a_i32.buf[i], 0); }

        arr_shrink(a_i32, &b, 3);
        EXPECT_EQ_LONG(b.len, 12L);
    }

    return TEST_RESULTS();
}
