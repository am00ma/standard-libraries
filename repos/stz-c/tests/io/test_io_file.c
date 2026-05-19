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

        // --------------- Directory ---------------

        buf_reset(&b);

        res = file_read(&b, _("./"));
        e   = _c(strerror(res.err));
        EXPECT_EQ_STR(e, _("Bad file descriptor"));

        // --------------- Free ---------------

        buf_free(&b);
    }

    TEST_CASE("file_write(path, text)")
    {
        Buf b = buf_new(1024 * 1024); // 1MB

        Res(Str) res_r1 = {}; // Initial read result
        Res(u64) res_w  = {}; // Write result
        Res(Str) res_r2 = {}; // Final read result

        res_r1 = file_read(&b, _(__FILE__));
        EXPECT_EQ_INT(res_r1.err, 0);

        res_w = file_write(_("/tmp/file_write_c"), res_r1.data, "w");
        EXPECT_EQ_INT(res_w.err, 0);
        EXPECT_EQ_LONG(res_w.data, (u64)res_r1.data.len);

        res_r2 = file_read(&b, _("/tmp/file_write_c"));
        EXPECT_EQ_INT(res_r2.err, 0);

        EXPECT_EQ_STR(res_r1.data, res_r2.data);

        buf_free(&b);
    }

    return TEST_RESULTS();
}
