#include "stz-c/io/args.h"

#include "stz-c/test.h"

int main(int argc, char* argv[])
{
    TEST_SUITE("stz-c/io/args.h");

    Buf b = buf_new(16 * 1024); // 16KB

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
        } cfg = {.path = _("./data"), .channel = 2};

        // clang-format off
        Arg options[] = {
            {_("help"),    0, ARG_Bool,    &cfg.help   , _("Show help"),        false},
            {_("nohelp"),  0, ARG_Bool,    &cfg.nohelp , _("Show nohelp"),      false},
            {_("path"),    0, ARG_String,  &cfg.path   , _("Input path"),       false},
            {_("gain"),    0, ARG_Float,   &cfg.gain   , _("Capture gain"),     false},
            {_("channel-for-audio-device-with-long-name"), 0, ARG_Integer, &cfg.channel, _("Capture channels"), false},
        };
        // clang-format on

        Args args = {.len = countof(options), .buf = options, .usage = _("some_program [OPTIONS]")};

        args_dump_help(&args);

        char* argv[] = {__FILE__, "--path", "./", "--gain", "0.25", "--help", "--channel", "5"};
        isize argc   = countof(argv);

        err = args_parse(&b, &args, argc, argv, true);
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

    buf_free(&b);

    TEST_RESULTS();
}
