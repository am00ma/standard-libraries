#pragma once

#include "stz-cpp/builtin/print.hpp"
#include "stz-cpp/builtin/str.hpp"

#include <getopt.h>

// --------------- Definitions ---------------

// Mirroring cJSON Types:
enum class ArgType : i32
{
    Invalid       = 0,
    False         = 1 << 0,
    True          = 1 << 1,
    Null          = 1 << 2,
    Float         = 1 << 3, // Number in cJSON
    String        = 1 << 4,
    Array         = 1 << 5,
    Object        = 1 << 6,
    Raw_JSON      = 1 << 7,
    IsReference   = 1 << 8,
    StringIsConst = 1 << 9,
    Bool          = 1 << 10, // Extra detailed types
    Integer       = 1 << 11,

};

struct Arg
{
    Str     name;
    char    shortopt;
    ArgType type; // Mirroring cJSON
    void*   val;  // Should directly point to struct to avoid copy

    Str  help;  // For printing help
    bool found; // Getopt does not enforce required, se we have to
};

struct Args
{
    isize len;
    Arg*  buf;
    Str   usage;

    void DumpHelp();
    void DumpValues();
    int  Parse(Buf* b, int argc, char* argv[], bool strict);
};

// --------------- Implementation ---------------

inline void Args::DumpHelp()
{
    PrintLn("USAGE: %.*s", __(usage));

    isize maxname = 0;
    RANGE(i, len) { maxname = maxname < buf[i].name.len ? buf[i].name.len : maxname; }
    maxname = maxname > 30 ? 30 : maxname;

    buf_stack(temp, 128);
    Str fmt = str_fmt(&temp, "  --%%-%ld.*s    %%.*s\n", maxname);

    RANGE(i, len)
    {
        fprintf(stdout, fmt.buf, //
                __(buf[i].name), //
                __(buf[i].help));
        maxname = maxname < buf[i].name.len ? buf[i].name.len : maxname;
    }
}

inline void Args::DumpValues()
{
    RANGE(i, len)
    {
        if (buf[i].val)
        {
            union {
                bool  b;
                float f;
                i32   x;
                Str   s = {};
            } u;

            Str   name  = buf[i].name;
            void* val   = buf[i].val;
            bool  found = buf[i].found;
            switch (buf[i].type)
            {
            case ArgType::Invalid:
            case ArgType::Array:
            case ArgType::Object:
            case ArgType::IsReference:
            case ArgType::Raw_JSON:
            case ArgType::StringIsConst:
            case ArgType::True:  // These are invalid
            case ArgType::False: // as type is bool
            case ArgType::Null: break;

            case ArgType::Bool:
                u.b = *(bool*)val;
                PrintLn("%-10.*s [%d]= %s", __(name), found, u.b ? "true" : "false");
                break;

            case ArgType::Float:
                u.f = *(float*)val;
                PrintLn("%-10.*s [%d]= %f", __(name), found, u.f);
                break;

            case ArgType::Integer:
                u.x = *(i32*)val;
                PrintLn("%-10.*s [%d]= %d", __(name), found, u.x);
                break;

            case ArgType::String:
                u.s = *(Str*)val;
                PrintLn("%-10.*s [%d]= %.*s", __(name), found, __(u.s));
                break;
            }
        }
    }
}

inline int Args::Parse(Buf* b, int argc, char* argv[], bool strict)
{
    // Only place where arena is used, but keeps strings which we need
    struct option* long_options = b->Make<struct option>(len + 1, AllocFlags::ZERO);
    RANGE(i, len)
    {
        long_options[i] = (struct option){
            .name    = buf[i].name.buf,
            .has_arg = buf[i].type != ArgType::Bool,
            .flag    = 0,
            .val     = 0, // TODO No short options for now
        };
    }

    int current;

    while (1)
    {
        // TODO: Some say that setting this will reset, but what to set it to then??
        // https://stackoverflow.com/questions/60483737/whats-the-portable-way-to-reset-getopt
        int option_index = 0;

        current = getopt_long(argc, argv, "", long_options, &option_index);
        if (current == -1) break;

        switch (current)
        {
        case 0:
            if (optarg)
            {
                union {
                    bool*  b;
                    float* f;
                    i32*   x;
                    Str*   s = {};
                } u;

                buf[option_index].found = true;

                void* val = buf[option_index].val;
                switch (buf[option_index].type)
                {
                case ArgType::Invalid:       // Unhandled cases
                case ArgType::True:          //
                case ArgType::False:         //
                case ArgType::Bool:          // Only toggle, so no args
                case ArgType::Array:         //
                case ArgType::Object:        //
                case ArgType::IsReference:   //
                case ArgType::Raw_JSON:      //
                case ArgType::StringIsConst: //
                case ArgType::Null: break;   //

                case ArgType::Float: // TODO: error handling for atof
                    u.f  = (float*)val;
                    *u.f = static_cast<float>(atof(optarg));
                    break;

                case ArgType::Integer: // TODO: error handling for atoi
                    u.x  = (i32*)val;
                    *u.x = atoi(optarg);
                    break;

                case ArgType::String:
                    u.s  = (Str*)val;
                    *u.s = Str(static_cast<isize>(strlen(optarg)), optarg);
                    break;
                }
            }
            else
            {
                void* val = buf[option_index].val;
                if (buf[option_index].type == ArgType::Bool)
                {
                    bool* bt = (bool*)val;
                    *bt      = !(*bt); // Toggle the default value
                }
            }
            break;

        case '?': break;
        default: PrintLn("?? getopt returned character code 0%o ??\n", current);
        }
    }

    if (strict && (optind < argc)) return -1; // Arg list too big

    if (optind < argc)
    {
        PrintLn("non-option ARGV-elements: ");
        while (optind < argc) PrintLn("%s ", argv[optind++]);
        PrintLn("\n");
    }

    return 0;
}
