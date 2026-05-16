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

    TEST_CASE("str_equal")
    {
        Str empty_a = "";
        Str empty_b = "";
        Str null_a  = {};
        Str null_b  = StrNull;
        Str str_a   = "hello";
        Str str_b   = "hi";

        EXPECT_EQ_STR(empty_a, empty_b);
        EXPECT_EQ_STR(null_a, null_b);

        EXPECT_NEQ_STR(empty_a, null_a);
        EXPECT_NEQ_STR(empty_a, null_b);
        EXPECT_NEQ_STR(empty_b, null_a);
        EXPECT_NEQ_STR(empty_b, null_b);

        EXPECT_EQ_STR(str_a, str_a);
        EXPECT_EQ_STR(str_b, str_b);

        EXPECT_NEQ_STR(str_a, str_b);
        EXPECT_NEQ_STR(str_b, str_a);

        EXPECT_NEQ_STR(str_a, null_a);
        EXPECT_NEQ_STR(str_b, null_a);
        EXPECT_NEQ_STR(str_a, empty_a);
        EXPECT_NEQ_STR(str_b, empty_a);
    }

    return TEST_RESULTS();
}
