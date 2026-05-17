#include "stz-cpp/io/cmd.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("io/cmd.hpp");

    TEST_CASE("cmd_exec(buf, path)")
    {
        Buf b(4 * 1024); // 4KB

        Res<CmdShell> res = {};

        res = cmd_exec(&b,                         //
                       "echo hello", CmdShellBash, //
                       3 * 1024, 1024);
        EXPECT_EQ_INT(res.err, 0);
        EXPECT_EQ_STR(res.data.out, Str("hello\n"));

        res = cmd_exec(&b,                        //
                       "ech hello", CmdShellBash, //
                       3 * 1024, 1024);
        EXPECT_EQ_INT(res.err, 0);
        EXPECT_EQ_STR(res.data.out, Str(""));
        EXPECT_EQ_STR(res.data.err, Str("/bin/bash: line 1: ech: command not found\n"));

        b.Free();
    }

    return TEST_RESULTS();
}
