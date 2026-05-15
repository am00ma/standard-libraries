#include "stz-cpp/builtin/buf.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/buf.hpp");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Buf), 24L); }

    TEST_CASE("Buf(cap)")
    {
        Buf b(16);
        EXPECT_EQ_LONG(b.cap, 16L);
        EXPECT_EQ_LONG(b.len, 0L);
        EXPECT_NEQ_NULL(b.buf);

        b.Reset();
        i32* x_i32 = b.Make<i32>(1, AllocFlags::NOZERO);
        *x_i32     = 56;
        EXPECT_EQ_INT(*x_i32, 56);
        EXPECT_EQ_LONG(b.len, 4L);
        EXPECT_TRUE(b.OnTop(x_i32, sizeof(i32)));

        b.Reset();
        bool* x_bool = b.Make<bool>(1, AllocFlags::NOZERO);
        *x_bool      = true;
        EXPECT_TRUE(*x_bool);
        EXPECT_EQ_LONG(b.len, 1L);
        EXPECT_TRUE(b.OnTop(x_bool, sizeof(bool)));

        u64* x_u64 = b.Make<u64>(1, AllocFlags::NOZERO);
        *x_u64     = 56;
        EXPECT_EQ_LONG(*x_u64, 56L);
        EXPECT_EQ_LONG(b.len, 16L); // accounts for alignment
        EXPECT_TRUE(b.OnTop(x_u64, sizeof(u64)));

        b.Free();
    }

    TEST_CASE("AllocFlags")
    {
        Buf b(16);
        RANGE(i, b.len) { b.buf[i] = 'a'; }

        Buf b_zero(&b, 4, AllocFlags::ZERO);
        RANGE(i, b_zero.len) { EXPECT_EQ_INT(b_zero.buf[i], 0); }

        Buf b_nozero(&b, 4, AllocFlags::NOZERO);
        RANGE(i, b_nozero.len) { EXPECT_NEQ_INT(b_nozero.buf[i], 0); }

        Buf b_softfail(&b, 128, AllocFlags::SOFTFAIL);
        EXPECT_EQ_NULL(b_softfail.buf);
        EXPECT_EQ_LONG(b_softfail.len, 0L);
        EXPECT_EQ_LONG(b_softfail.cap, 0L);

        b.Free();
    }

    TEST_CASE("buf_stack(cap)")
    {
        buf_stack(b, 16);
        RANGE(i, b.len) { b.buf[i] = 'a'; }

        Buf b_zero(&b, 4, AllocFlags::ZERO);
        RANGE(i, b_zero.len) { EXPECT_EQ_INT(b_zero.buf[i], 0); }

        Buf b_nozero(&b, 4, AllocFlags::NOZERO);
        RANGE(i, b_nozero.len) { EXPECT_NEQ_INT(b_nozero.buf[i], 0); }

        Buf b_softfail(&b, 128, AllocFlags::SOFTFAIL);
        EXPECT_EQ_NULL(b_softfail.buf);
        EXPECT_EQ_LONG(b_softfail.len, 0L);
        EXPECT_EQ_LONG(b_softfail.cap, 0L);
    }

    return TEST_RESULTS();
}
