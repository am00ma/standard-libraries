#include "stz-cpp/stz-cpp.hpp" // IWYU pragma: keep

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("stz-cpp");

    TEST_CASE("sizeof")
    {
        EXPECT_EQ_LONG(sizeof(int), 4L);
        EXPECT_EQ_LONG(sizeof(long), 8L);
    }

    return TEST_RESULTS();
}
