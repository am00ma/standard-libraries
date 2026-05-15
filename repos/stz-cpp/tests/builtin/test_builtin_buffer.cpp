#include "stz-cpp/builtin/buffer.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/buffer.hpp");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Buf), 24L); }

    return TEST_RESULTS();
}
