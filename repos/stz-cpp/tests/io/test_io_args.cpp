#include "stz-cpp/io/args.hpp"

#include "stz-cpp/test.hpp"

int main(int argc, char* argv[])
{
    TEST_SUITE("stz-cpp/io/args.hpp");

    Buf b(16 * 1024); // 16KB

    TEST_CASE("Size")
    {
        EXPECT_EQ_LONG(sizeof(struct option), 32L);
        EXPECT_EQ_LONG(sizeof(Arg), 56L);
        EXPECT_EQ_LONG(sizeof(Args), 32L);
    }

    TEST_CASE("args_parse")
    {
        int err = 0;

        struct
        {
            bool  help;
            bool  nohelp;
            Str   path;
            float gain;
            i32   channel;
        } cfg = {
            .help    = false,
            .nohelp  = true,
            .path    = "./data",
            .gain    = 0,
            .channel = 2,
        };

        // clang-format off
        Arg options[] = {
            {"help",    0, ArgType::Bool,    &cfg.help   , "Show help",        false},
            {"nohelp",  0, ArgType::Bool,    &cfg.nohelp , "Show nohelp",      false},
            {"path",    0, ArgType::String,  &cfg.path   , "Input path",       false},
            {"gain",    0, ArgType::Float,   &cfg.gain   , "Capture gain",     false},
            {"channel-for-audio-device-with-long-name", 0, ArgType::Integer, &cfg.channel, "Capture channels", false},
        };
        // clang-format on

        Args args(countof(options), options, "some_program [OPTIONS]");

        args.DumpHelp();

        const char* argv[] = {__FILE__, "--path", "./", "--gain", "0.25", "--help", "--channel", "5"};
        int         argc   = countof(argv);

        err = args.Parse(&b, argc, (char**)argv, true);
        EXPECT_EQ_INT(err, 0);

        PrintLn("cfg:");
        PrintLn("  path   : %.*s", __(cfg.path));
        PrintLn("  gain   : %f", cfg.gain);
        PrintLn("  channel: %d", cfg.channel);
        PrintLn("  help   : %d", cfg.help);
        PrintLn("  nohelp : %d", cfg.nohelp);

        // NOTE: Somehow seems like getopt maintains state, so we cannot test twice
        // char* argv2[] = {__FILE__, "--gain", "0.25", "--help"};
    };

    b.Free();

    TEST_RESULTS();
}
