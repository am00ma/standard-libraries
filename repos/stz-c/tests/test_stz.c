#include "stz-c/stz-c.h" // IWYU pragma: keep

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("stz-c");

    TEST_CASE("sizeof")
    {
        EXPECT_EQ_LONG(sizeof(int), 4L);
        EXPECT_EQ_LONG(sizeof(long), 8L);
    }

    return TEST_RESULTS();
}
