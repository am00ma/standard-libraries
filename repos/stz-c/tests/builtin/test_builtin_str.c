#include "stz-c/builtin/str.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/str.h");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Str), 16L); }

    TEST_CASE("constructors")
    {
        Str s_null = StrNull;
        EXPECT_EQ_NULL(s_null.buf);
        EXPECT_EQ_LONG(s_null.len, 0L);

        Str s_from_literal = _("hello");
        EXPECT_EQ_LONG(s_from_literal.len, 5L);

        Str s_from_chars = _c((char*)"hi");
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
        EXPECT_EQ_STR(s_from_buf, _("hello"));
    }

    TEST_CASE("str_equal")
    {
        Str empty_a = _("");
        Str empty_b = _("");
        Str null_a  = {};
        Str null_b  = StrNull;
        Str str_a   = _("hello");
        Str str_b   = _("hi");

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

    TEST_CASE("str_sub")
    {

        struct
        {
            Str   src;
            isize i;
            isize j;
            Str   exp;
        } tests[] = {

            // Empty source string
            // {_(""), -1, -1, Error},
            {_(""), 0, 0, _("")},
            // {_(""), 0, 1, Error},
            // {_(""), 1, 0, Error},
            // {_(""), 1, 1, Error},

            // Single char
            // {_("a"), -1, -1, Error},
            {_("a"), 0, 0, _("")},
            {_("a"), 0, 1, _("a")},
            // {_("a"), 0, 2, Error},
            // {_("a"), 1, 0, Error},
            {_("a"), 1, 1, _("")},
            // {_("a"), 1, 2, Error},
        };

        RANGE(i, countof(tests))
        {
            auto t   = &tests[i];
            Str  dst = str_sub(t->src, t->i, t->j);
            EXPECT_EQ_STR(dst, t->exp);
        }
    }

    TEST_CASE("str_fmtn, str_fmt")
    {
        buf_stack(temp, 128);
        memset(temp.buf, 'a', 128);

        str_fmtn(&temp, 10, "hellohello");
        EXPECT_EQ_LONG(temp.len, 10L);

        buf_reset(&temp);
        Str str3 = str_fmtn(&temp, 5, "hello");
        EXPECT_FALSE(IsNullTerm(str3));
        EXPECT_EQ_LONG(temp.len, 5L);

        // Unexpected behaviour of snprintf
        char buf[128];
        memset(buf, 'a', 128);
        EXPECT_EQ_INT(snprintf(buf, 128, "hello"), 5);
        EXPECT_EQ_INT(buf[5], 0); // And not 'a'

        // Leads to the following
        buf_reset(&temp);
        Str str4 = str_fmt(&temp, "hello");
        EXPECT_TRUE(IsNullTerm(str4));
        EXPECT_EQ_LONG(temp.len, 5L); // But buf is proper

        buf_reset(&temp);
        Str str5 = str_fmt(&temp, "hello: %03ld", 1L);
        EXPECT_EQ_STR(str5, _("hello: 001"));
    }

    TEST_CASE("str_sub, str_startswith, str_endswith")
    {
        Str s1 = _("hello hi how are you");

        EXPECT_EQ_STR(str_sub(s1, 0, 0), _(""));
        EXPECT_EQ_STR(str_sub(s1, 0, 5), _("hello"));

        EXPECT_TRUE(str_startswith(s1, _("")));
        EXPECT_FALSE(str_startswith(s1, StrNull));
        EXPECT_TRUE(str_startswith(s1, _("hello")));

        EXPECT_TRUE(str_endswith(s1, _("")));
        EXPECT_FALSE(str_endswith(s1, StrNull));
        EXPECT_TRUE(str_endswith(s1, _("you")));
    }

    return TEST_RESULTS();
}
