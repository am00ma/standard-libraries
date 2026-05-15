#include "stz-c/builtin/buffer.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/buffer.h");

    TEST_CASE("sizeof") { EXPECT_EQ_LONG(sizeof(Buf), 24L); }

    return TEST_RESULTS();
}
