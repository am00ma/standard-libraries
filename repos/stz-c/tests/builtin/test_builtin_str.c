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

        EXPECT_EQ_LONG(str_find(s1, _("hi")), 6L);
        EXPECT_EQ_LONG(str_find(s1, _("")), 0L);
        EXPECT_EQ_LONG(str_find(s1, StrNull), 0L);

        Str e = _("");

        EXPECT_EQ_STR(str_sub(e, 0, 0), _(""));

        EXPECT_TRUE(str_startswith(e, _("")));
        EXPECT_FALSE(str_startswith(e, StrNull));

        EXPECT_TRUE(str_endswith(e, _("")));
        EXPECT_FALSE(str_endswith(e, StrNull));

        EXPECT_EQ_LONG(str_find(e, _("")), 0L);
        EXPECT_EQ_LONG(str_find(e, StrNull), 0L);

        Str n = StrNull;

        EXPECT_EQ_STR(str_sub(n, 0, 0), StrNull);

        EXPECT_FALSE(str_startswith(n, _("")));
        EXPECT_TRUE(str_startswith(n, StrNull));

        EXPECT_FALSE(str_endswith(n, _("")));
        EXPECT_TRUE(str_endswith(n, StrNull));

        EXPECT_EQ_LONG(str_find(n, _("")), -1L);
        EXPECT_EQ_LONG(str_find(n, StrNull), 0L);
    }

    TEST_CASE("Trim")
    {
        EXPECT_EQ_STR(str_trim(_("\t x  \t"), TRIM_DEFAULT), _("x"));
        EXPECT_EQ_STR(str_trim(_("\n x  \n"), TRIM_LEFTRIGHT | TRIM_NEWLINES), _(" x  "));
        EXPECT_EQ_STR(str_trim(_("\n x  \n"), TRIM_RIGHT | TRIM_NEWLINES), _("\n x  "));
        EXPECT_EQ_STR(str_trim(_("\n x  \n"), TRIM_LEFT | TRIM_NEWLINES), _(" x  \n"));
        EXPECT_EQ_STR(str_trim(_("\n x  \n"), TRIM_RIGHT | TRIM_SPACES | TRIM_NEWLINES), _("\n x"));
        EXPECT_EQ_STR(str_trim(_("\n x  \n"), TRIM_LEFT | TRIM_SPACES | TRIM_NEWLINES), _("x  \n"));
        EXPECT_EQ_STR(str_trim(_("\n x  \n"), TRIM_DEFAULT | TRIM_NEWLINES), _("x"));
    }

    TEST_CASE("Split")
    {
        buf_stack(b, 128);

        // TODO: SPLIT_SUBSTITUTE_NULL
        struct
        {
            Str           src;
            isize         maxlen;
            StrSplitFlags flags;
            isize         expected;
        } cases[] = {
            {_(""), 0, 0, 0},
            {_("a b c"), 8, SPLIT_DEFAULT, 3},
            {_("a b c"), 2, SPLIT_DEFAULT, 2},
            {_("a    b c"), 8, SPLIT_DEFAULT, 6},
            {_("a    b c"), 8, SPLIT_IGNORE_EMPTY, 3},
        };

        RANGE(i, countof(cases))
        {
            buf_reset(&b);
            Arr(Str) got = str_splitc(&b, cases[i].src, ' ', cases[i].maxlen, cases[i].flags);
            EXPECT_EQ_LONG(got.len, cases[i].expected);
            EXPECT_EQ_LONG(b.len, (cases[i].expected * (isize)sizeof(Str)));
        }
    }

    TEST_CASE("Split lines")
    {
        buf_stack(b, 128);

        // TODO: SPLIT_SUBSTITUTE_NULL
        struct
        {
            Str   src;
            bool  ignore;
            isize expected;
        } cases[] = {
            {_(""), true, 0},
            {_(""), false, 1},
            {_("a\nb\nc"), false, 3},
            {_("a\n\n\n\nb\nc"), false, 6},
            {_("a\n\n\n\nb\nc"), true, 3},
        };

        RANGE(i, countof(cases))
        {
            buf_reset(&b);
            Arr(Str) got = str_split_lines(&b, cases[i].src, -1, cases[i].ignore);
            EXPECT_EQ_LONG(got.len, cases[i].expected);
            EXPECT_EQ_LONG(b.len, (cases[i].expected * (isize)sizeof(Str)));
        }
    }

    TEST_CASE("split_pair")
    {
        buf_stack(b, 128);

        // clang-format off
        struct
        {
            Str           src;
            char           sep;
            StrTrimFlags flags;
            Str           exp_key;
            Str           exp_val;
        } cases[] = {
            {StrNull,         ':', TRIM_DEFAULT, StrNull,  StrNull},
            {_(""),           ':', TRIM_DEFAULT, _(""),    StrNull},
            {_("abc"),        ':', TRIM_DEFAULT, _("abc"), StrNull},
            {_("a:b c"),      ':', TRIM_DEFAULT, _("a"),   _("b c")},
            {_("a:   b c"),   ':', TRIM_DEFAULT, _("a"),   _("b c")},
            {_("a:   b c  "), ':', TRIM_DEFAULT, _("a"),   _("b c")},
            {_("a:   b c  "), ':', TRIM_NONE,    _("a"),   _("   b c  ")},
        };
        // clang-format on

        RANGE(i, countof(cases))
        {
            buf_reset(&b);
            PAIR(Str, Str) got = str_split_pair(cases[i].src, cases[i].sep, cases[i].flags);
            EXPECT_EQ_STR(got.a, cases[i].exp_key);
            EXPECT_EQ_STR(got.b, cases[i].exp_val);
        }
    }

    TEST_CASE("Iterator")
    {
        Str src        = _(" hello   hi ");
        Str expected[] = {_(""), _("hello"), _(""), _(""), _("hi"), _("")};

        Str   word  = {};
        isize count = 0;
        while (src.len)
        {
            word = str_till_next(&src, ' ');
            EXPECT_EQ_STR(word, expected[count]);
            count++;
        }

        src   = _(" hello   hi ");
        word  = StrNull;
        count = 0;
        while (src.len)
        {
            word = str_till_next(&src, ',');
            count++;
        }
        EXPECT_EQ_LONG(count, 1L);
        EXPECT_EQ_STR(word, _(" hello   hi "));

        Str src2        = _(" hello   hi ");
        Str sep2        = _("hello");
        Str expected2[] = {_(" "), _("   hi ")};

        word  = StrNull;
        count = 0;
        while (src2.len)
        {
            word = str_till_next2(&src2, sep2);
            EXPECT_EQ_STR(word, expected2[count]);
            count++;
        }

        Str src3        = _(" hello   hi ");
        Str sep3        = _("how");
        Str expected3[] = {src3};

        word  = StrNull;
        count = 0;
        while (src3.len)
        {
            word = str_till_next2(&src3, sep3);
            EXPECT_EQ_STR(word, expected3[count]);
            count++;
        }

        Str src4        = _(" hello   hi ");
        Str sep4        = _("hi ");
        Str expected4[] = {_(" hello   ")};

        word  = StrNull;
        count = 0;
        while (src4.len)
        {
            word = str_till_next2(&src4, sep4);
            EXPECT_EQ_STR(word, expected4[count]);
            count++;
        }

        Str src5        = _(" hello   hi ");
        Str sep5        = _("hi    ");
        Str expected5[] = {src5};

        word  = StrNull;
        count = 0;
        while (src5.len)
        {
            word = str_till_next2(&src5, sep5);
            EXPECT_EQ_STR(word, expected5[count]);
            count++;
        }
    }

    return TEST_RESULTS();
}
