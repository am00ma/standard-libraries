#include "stz-cpp/builtin/str.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/str.hpp");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Str), 16L); }

    TEST_CASE("constructors")
    {
        Str s_null = StrNull;
        EXPECT_EQ_NULL(s_null.buf);
        EXPECT_EQ_LONG(s_null.len, 0L);

        Str s_from_literal = "hello";
        EXPECT_EQ_LONG(s_from_literal.len, 5L);

        Str s_from_chars = (char*)"hi";
        EXPECT_EQ_LONG(s_from_chars.len, 2L);

        struct
        {
            isize       a;
            const char* buf;
            isize       len;
            isize       b;
        } temp = {10, "hello", 5, 20};

        Str s_from_buf = Str_(temp);
        EXPECT_EQ_LONG(s_from_buf.len, 5L);
        EXPECT_EQ_STR(s_from_buf, Str("hello"));
    }

    return TEST_RESULTS();
}
