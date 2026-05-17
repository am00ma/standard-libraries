#include "stz-cpp/io/file.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("io/file.h");

    TEST_CASE("file_read(buf, path)")
    {
        Buf b(1024 * 1024 * 1024); // 1GB

        Res<Str> res = {}; // Result
        Str      e   = {}; // Error string

        // --------------- Normal case ---------------

        res = file_read(&b, __FILE__);
        EXPECT_EQ_INT(res.err, 0);

        // --------------- No such file ---------------

        b.Reset();

        res = file_read(&b, "hello");
        e   = Str(strerror(res.err));
        EXPECT_EQ_STR(e, Str("No such file or directory"));

        // // --------------- Directory ---------------
        // // TODO: check it is not dir, else segfaults
        //
        // b.Reset();
        //
        // res = file_read(&b, _("./"));
        // PrintVarLn(res.data);

        // --------------- Free ---------------

        b.Free();
    }

    return TEST_RESULTS();
}
