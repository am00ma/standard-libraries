#include "stz-c/io/file.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("io/file.h");

    TEST_CASE("file_read(buf, path)")
    {
        Buf b = buf_new(1024 * 1024 * 1024); // 1GB

        Res(Str) res = {}; // Result
        Str e        = {}; // Error string

        // --------------- Normal case ---------------

        res = file_read(&b, _(__FILE__));
        EXPECT_EQ_INT(res.err, 0);

        // --------------- No such file ---------------

        buf_reset(&b);

        res = file_read(&b, _("hello"));
        e   = _c(strerror(res.err));
        EXPECT_EQ_STR(e, _("No such file or directory"));

        // // --------------- Directory ---------------
        // // TODO: check it is not dir, else segfaults
        //
        // buf_reset(&b);
        //
        // res = file_read(&b, _("./"));
        // PrintVarLn(res.data);

        // --------------- Free ---------------

        buf_free(&b);
    }

    return TEST_RESULTS();
}
