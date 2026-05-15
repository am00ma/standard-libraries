#include "stz-c/builtin/buf.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/buf.h");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Buf), 24L); }

    TEST_CASE("buf_new(cap)")
    {
        Buf b = buf_new(1024);
        EXPECT_EQ_LONG(b.cap, 1024L);
        EXPECT_EQ_LONG(b.len, 0L);
        EXPECT_NEQ_NULL(b.buf);

        buf_reset(&b);
        i32* x_i32 = Make(&b, i32, 1, ALLOC_NOZERO);
        *x_i32     = 56;
        EXPECT_EQ_INT(*x_i32, 56);
        EXPECT_EQ_LONG(b.len, 4L);
        EXPECT_TRUE(buf_ontop(&b, x_i32, sizeof(i32)));

        buf_reset(&b);
        bool* x_bool = Make(&b, bool, 1, ALLOC_NOZERO);
        *x_bool      = true;
        EXPECT_TRUE(*x_bool);
        EXPECT_EQ_LONG(b.len, 1L);
        EXPECT_TRUE(buf_ontop(&b, x_bool, sizeof(bool)));

        u64* x_u64 = Make(&b, u64, 1, ALLOC_NOZERO);
        *x_u64     = 56;
        EXPECT_EQ_LONG(*x_u64, 56L);
        EXPECT_EQ_LONG(b.len, 16L); // accounts for alignment
        EXPECT_TRUE(buf_ontop(&b, x_u64, sizeof(u64)));

        buf_free(&b);
    }

    TEST_CASE("AllocFlags")
    {
        Buf b = buf_new(16);
        RANGE(i, b.len) { b.buf[i] = 'a'; }

        Buf b_zero = buf_new2(&b, 4, ALLOC_ZERO);
        RANGE(i, b_zero.len) { EXPECT_EQ_INT(b_zero.buf[i], 0); }

        Buf b_nozero = buf_new2(&b, 4, ALLOC_NOZERO);
        RANGE(i, b_nozero.len) { EXPECT_NEQ_INT(b_nozero.buf[i], 0); }

        Buf b_softfail = buf_new2(&b, 128, ALLOC_SOFTFAIL);
        EXPECT_EQ_NULL(b_softfail.buf);
        EXPECT_EQ_LONG(b_softfail.len, 0L);
        EXPECT_EQ_LONG(b_softfail.cap, 0L);

        buf_free(&b);
    }

    TEST_CASE("buf_stack(cap)")
    {
        buf_stack(b, 16);
        RANGE(i, b.len) { b.buf[i] = 'a'; }

        Buf b_zero = buf_new2(&b, 4, ALLOC_ZERO);
        RANGE(i, b_zero.len) { EXPECT_EQ_INT(b_zero.buf[i], 0); }

        Buf b_nozero = buf_new2(&b, 4, ALLOC_NOZERO);
        RANGE(i, b_nozero.len) { EXPECT_NEQ_INT(b_nozero.buf[i], 0); }

        Buf b_softfail = buf_new2(&b, 128, ALLOC_SOFTFAIL);
        EXPECT_EQ_NULL(b_softfail.buf);
        EXPECT_EQ_LONG(b_softfail.len, 0L);
        EXPECT_EQ_LONG(b_softfail.cap, 0L);
    }

    return TEST_RESULTS();
}
