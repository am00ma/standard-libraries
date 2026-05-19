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
            // {"", -1, -1, Error},
            {"", 0, 0, ""},
            // {"", 1, 1, Error},
            // {"", 1, 0, Error},
            // {"", 0, 1, Error},

            // Single char
            // {"a", -1, -1, Error},
            {"a", 0, 0, ""},
            {"a", 0, 1, "a"},
            // {"a", 0, 2, Error},
            // {"a", 1, 0, Error},
            {"a", 1, 1, ""},
            // {"a", 1, 2, Error},
        };

        RANGE(i, countof(tests))
        {
            auto t   = &tests[i];
            Str  dst = t->src[t->i, t->j];
            EXPECT_EQ_STR(dst, t->exp);
        }
    }

    TEST_CASE("str_fmtn, str_fmt")
    {
        buf_stack(temp, 128);
        memset(temp.buf, 'a', 128);

        str_fmtn(&temp, 10, "hellohello");
        EXPECT_EQ_LONG(temp.len, 10L);

        temp.Reset();
        Str str3 = str_fmtn(&temp, 5, "hello");
        EXPECT_FALSE(IsNullTerm(str3));
        EXPECT_EQ_LONG(temp.len, 5L);

        // Unexpected behaviour of snprintf
        char buf[128];
        memset(buf, 'a', 128);
        EXPECT_EQ_INT(snprintf(buf, 128, "hello"), 5);
        EXPECT_EQ_INT(buf[5], 0); // And not 'a'

        // Leads to the following
        temp.Reset();
        Str str4 = str_fmt(&temp, "hello");
        EXPECT_TRUE(IsNullTerm(str4));
        EXPECT_EQ_LONG(temp.len, 5L); // But buf is proper

        temp.Reset();
        Str str5 = str_fmt(&temp, "hello: %03ld", 1L);
        EXPECT_EQ_STR(str5, Str("hello: 001"));
    }

    TEST_CASE("substrings")
    {
        Str s1 = "hello hi how are you";

        EXPECT_EQ_STR((s1[0, 0]), Str(""));
        EXPECT_EQ_STR((s1[0, 5]), Str("hello"));

        EXPECT_TRUE(s1.StartsWith(""));
        EXPECT_FALSE(s1.StartsWith(StrNull)); // Null != Empty
        EXPECT_TRUE(s1.StartsWith("hello"));

        EXPECT_TRUE(s1.EndsWith(""));
        EXPECT_FALSE(s1.EndsWith(StrNull));
        EXPECT_TRUE(s1.EndsWith("you"));

        EXPECT_EQ_LONG(s1.Find("hi"), 6L);
        EXPECT_EQ_LONG(s1.Find(""), 0L);
        EXPECT_EQ_LONG(s1.Find(StrNull), 0L);

        Str e = "";

        EXPECT_EQ_STR((e[0, 0]), Str(""));

        EXPECT_TRUE(e.StartsWith(""));
        EXPECT_FALSE(e.StartsWith(StrNull));

        EXPECT_TRUE(e.EndsWith(""));
        EXPECT_FALSE(e.EndsWith(StrNull));

        EXPECT_EQ_LONG(e.Find(""), 0L);
        EXPECT_EQ_LONG(e.Find(StrNull), 0L);

        Str n = StrNull;

        EXPECT_EQ_STR((n[0, 0]), StrNull);

        EXPECT_FALSE(n.StartsWith(""));
        EXPECT_TRUE(n.StartsWith(StrNull));

        EXPECT_FALSE(n.EndsWith(""));
        EXPECT_TRUE(n.EndsWith(StrNull));

        EXPECT_EQ_LONG(n.Find(""), -1L);
        EXPECT_EQ_LONG(n.Find(StrNull), 0L);
    }

    TEST_CASE("Trim")
    {

        using Tf = Str::TrimFlags;
#define F2I(flag) static_cast<i32>(flag)
#define I2F(flag) static_cast<Tf>(flag)

        EXPECT_EQ_STR(Str("\t x  \t").Trim(Tf::DEFAULT), Str("x"));
        EXPECT_EQ_STR(Str("\n x  \n").Trim(I2F(F2I(Tf::LEFTRIGHT) | F2I(Tf::NEWLINES))), Str(" x  "));
        EXPECT_EQ_STR(Str("\n x  \n").Trim(I2F(F2I(Tf::RIGHT) | F2I(Tf::NEWLINES))), Str("\n x  "));
        EXPECT_EQ_STR(Str("\n x  \n").Trim(I2F(F2I(Tf::LEFT) | F2I(Tf::NEWLINES))), Str(" x  \n"));
        EXPECT_EQ_STR(Str("\n x  \n").Trim(I2F(F2I(Tf::RIGHT) | F2I(Tf::SPACES) | F2I(Tf::NEWLINES))), Str("\n x"));
        EXPECT_EQ_STR(Str("\n x  \n").Trim(I2F(F2I(Tf::LEFT) | F2I(Tf::SPACES) | F2I(Tf::NEWLINES))), Str("x  \n"));
        EXPECT_EQ_STR(Str("\n x  \n").Trim(I2F(F2I(Tf::DEFAULT) | F2I(Tf::NEWLINES))), Str("x"));

#undef F2I
#undef I2F
    }

    TEST_CASE("Split")
    {
        buf_stack(b, 128);

        using Sf = Str::SplitFlags;

        // TODO: SPLIT_SUBSTITUTE_NULL
        struct
        {
            Str             src;
            isize           maxlen;
            Str::SplitFlags flags;
            isize           expected;
        } cases[] = {
            {"", 0, Sf::DEFAULT, 0},              //
            {"a b c", 8, Sf::DEFAULT, 3},         //
            {"a b c", 2, Sf::DEFAULT, 2},         //
            {"a    b c", 8, Sf::DEFAULT, 6},      //
            {"a    b c", 8, Sf::IGNORE_EMPTY, 3}, //
        };

        RANGE(i, countof(cases))
        {
            b.Reset();
            Arr<Str> got = cases[i].src.SplitC(&b, ' ', cases[i].maxlen, cases[i].flags);
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
            {"", true, 0},               //
            {"", false, 1},              //
            {"a\nb\nc", false, 3},       //
            {"a\n\n\n\nb\nc", false, 6}, //
            {"a\n\n\n\nb\nc", true, 3},  //
        };

        RANGE(i, countof(cases))
        {
            b.Reset();
            Arr<Str> got = cases[i].src.SplitLines(&b, -1, cases[i].ignore);
            EXPECT_EQ_LONG(got.len, cases[i].expected);
            EXPECT_EQ_LONG(b.len, (cases[i].expected * (isize)sizeof(Str)));
        }
    }

    TEST_CASE("split_pair")
    {
        buf_stack(b, 128);

        using Tf = Str::TrimFlags;

        // clang-format off
        struct
        {
            Str           src;
            char           sep;
            Str::TrimFlags flags;
            Str           exp_key;
            Str           exp_val;
        } cases[] = {
            {StrNull,      ':', Tf::DEFAULT, StrNull, StrNull},
            {"",           ':', Tf::DEFAULT, "",      StrNull},
            {"abc",        ':', Tf::DEFAULT, "abc",   StrNull},
            {"a:b c",      ':', Tf::DEFAULT, "a",     "b c"},
            {"a:   b c",   ':', Tf::DEFAULT, "a",     "b c"},
            {"a:   b c  ", ':', Tf::DEFAULT, "a",     "b c"},
            {"a:   b c  ", ':', Tf::NONE,    "a",     "   b c  "},
        };
        // clang-format on

        RANGE(i, countof(cases))
        {
            b.Reset();
            Pair<Str, Str> got = cases[i].src.SplitPair(cases[i].sep, cases[i].flags);
            EXPECT_EQ_STR(got.a, cases[i].exp_key);
            EXPECT_EQ_STR(got.b, cases[i].exp_val);
        }
    }

    TEST_CASE("Iterator")
    {
        Str src        = " hello   hi ";
        Str expected[] = {"", "hello", "", "", "hi", ""};

        Str   word  = {};
        isize count = 0;
        while (src.len)
        {
            word = src.TillNext(' ');
            EXPECT_EQ_STR(word, expected[count]);
            count++;
        }

        src   = " hello   hi ";
        word  = StrNull;
        count = 0;
        while (src.len)
        {
            word = src.TillNext(',');
            count++;
        }
        EXPECT_EQ_LONG(count, 1L);
        EXPECT_EQ_STR(word, Str(" hello   hi "));

        Str src2        = " hello   hi ";
        Str sep2        = "hello";
        Str expected2[] = {" ", "   hi "};

        word  = StrNull;
        count = 0;
        while (src2.len)
        {
            word = src2.TillNext(sep2);
            EXPECT_EQ_STR(word, expected2[count]);
            count++;
        }

        Str src3        = " hello   hi ";
        Str sep3        = "how";
        Str expected3[] = {src3};

        word  = StrNull;
        count = 0;
        while (src3.len)
        {
            word = src3.TillNext(sep3);
            EXPECT_EQ_STR(word, expected3[count]);
            count++;
        }

        Str src4        = " hello   hi ";
        Str sep4        = "hi ";
        Str expected4[] = {" hello   "};

        word  = StrNull;
        count = 0;
        while (src4.len)
        {
            word = src4.TillNext(sep4);
            EXPECT_EQ_STR(word, expected4[count]);
            count++;
        }

        Str src5        = " hello   hi ";
        Str sep5        = "hi    ";
        Str expected5[] = {src5};

        word  = StrNull;
        count = 0;
        while (src5.len)
        {
            word = src5.TillNext(sep5);
            EXPECT_EQ_STR(word, expected5[count]);
            count++;
        }
    }
    return TEST_RESULTS();
}
