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

    // TEST_CASE("Print generics")
    // {
    //     u32 v_u32 = 10;
    //     i32 v_i32 = -10;
    //     f32 v_f32 = 10.0;
    //     PrintVarLn(v_u32);
    //     PrintVarLn(v_i32);
    //     PrintVarLn(v_f32);
    //
    //     Str a = "hello";
    //     PrintVarLn(a);
    // }

    return TEST_RESULTS();
}
