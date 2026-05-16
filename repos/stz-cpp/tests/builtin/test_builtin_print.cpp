#include "stz-cpp/builtin/print.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("builtin/print.hpp");

    TEST_CASE("Print variants")
    {
        PrintInline("PrintInline");
        PrintLn("PrintLn");
        PrintNewline();

        PrintTitle("PrintTitle");
        PrintWarn("PrintWarn");
        PrintInfo("PrintInfo");
        PrintError("PrintError");
    }

    return TEST_RESULTS();
}
