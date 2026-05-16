#include "stz-cpp/builtin/arr.hpp"
#include "stz-cpp/builtin/buf.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/arr.hpp");

    TEST_CASE("Constructors")
    {
        buf_stack(b, 32);

        Arr<i32> a_i32(&b, 4, AllocFlags::ZERO);
        EXPECT_EQ_LONG(a_i32.len, 4L);
        EXPECT_EQ_LONG(b.len, 16L);

        RANGE(i, a_i32.len) { EXPECT_EQ_INT(a_i32.buf[i], 0); }

        a_i32.Shrink(&b, 3);
        EXPECT_EQ_LONG(b.len, 12L);
    }

    return TEST_RESULTS();
}
