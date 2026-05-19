#include "stz-cpp/builtin/errors.hpp"
#include "stz-cpp/builtin/print_gen.hpp"
#include "stz-cpp/io/args.hpp"

struct
{
    bool help;
    Str  path_in;
    Str  path_out;
} cfg = {
    .help     = false,
    .path_in  = "./data/mdcode.md",
    .path_out = "./outputs/mdcode.html",
};

// clang-format off
Arg options[] = {
    {"help",     0, ArgType::Bool,   &cfg.help,     "Show help",           false},
    {"path_in",  0, ArgType::String, &cfg.path_in,  "Input markdown path", false},
    {"path_out", 0, ArgType::String, &cfg.path_out, "Output html path",    false},
};
// clang-format on

int main(int argc, char* argv[])
{
    int err = 0;

    Buf b(16 * 1024); // 16KB

    Args args(countof(options), options, "mdcode [OPTIONS]");

    err = args.Parse(&b, argc, (char**)argv, true);
    OnError_Fatal(err, "Failed: args.Parse(%d, ...)", argc);

    if (cfg.help)
    {
        args.DumpHelp();
        return EXIT_SUCCESS;
    }

    PrintVarLn(cfg.path_in);
    PrintVarLn(cfg.path_out);

    return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
