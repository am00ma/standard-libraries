#include "stz-c/io/cmd.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("io/cmd.h");

    TEST_CASE("cmd_exec(buf, path)")
    {
        Buf b = buf_new(4 * 1024); // 4KB

        Res(CmdShell) res = {};

        res = cmd_exec(&b,                            //
                       _("echo hello"), CmdShellBash, //
                       3 * 1024, 1024);
        EXPECT_EQ_INT(res.err, 0);
        EXPECT_EQ_STR(res.data.out, _("hello\n"));

        res = cmd_exec(&b,                           //
                       _("ech hello"), CmdShellBash, //
                       3 * 1024, 1024);
        EXPECT_EQ_INT(res.err, 0);
        EXPECT_EQ_STR(res.data.out, _(""));
        EXPECT_EQ_STR(res.data.err, _("/bin/bash: line 1: ech: command not found\n"));

        buf_free(&b);
    }

    return TEST_RESULTS();
}
