#include "stz-cpp/io/file.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("io/file.h");

    TEST_CASE("file_read(buf, path)")
    {
        Buf b(1024 * 1024); // 1MB

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

        // --------------- Directory ---------------

        b.Reset();

        res = file_read(&b, "./");
        e   = Str(strerror(res.err));
        EXPECT_EQ_STR(e, Str("Bad file descriptor"));

        // --------------- Free ---------------

        b.Free();
    }

    TEST_CASE("file_write(path, text)")
    {
        Buf b(1024 * 1024); // 1MB

        Res<Str> res_r1 = {}; // Initial read result
        Res<u64> res_w  = {}; // Write result
        Res<Str> res_r2 = {}; // Final read result

        res_r1 = file_read(&b, __FILE__);
        EXPECT_EQ_INT(res_r1.err, 0);

        res_w = file_write("/tmp/file_write_cpp", res_r1.data, "w");
        EXPECT_EQ_INT(res_w.err, 0);
        EXPECT_EQ_LONG(res_w.data, (u64)res_r1.data.len);

        res_r2 = file_read(&b, "/tmp/file_write_cpp");
        EXPECT_EQ_INT(res_r2.err, 0);

        EXPECT_EQ_STR(res_r1.data, res_r2.data);

        b.Free();
    }

    return TEST_RESULTS();
}
