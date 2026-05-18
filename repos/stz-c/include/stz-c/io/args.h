#pragma once

#include "stz-c/builtin/print.h"
#include "stz-c/builtin/str.h"

#include <getopt.h>

// --------------- Definitions ---------------

// Mirroring cJSON Types:
typedef enum
{
    ARG_Invalid       = 0,
    ARG_False         = 1 << 0,
    ARG_True          = 1 << 1,
    ARG_NULL          = 1 << 2,
    ARG_Float         = 1 << 3, // Number in cJSON
    ARG_String        = 1 << 4,
    ARG_Array         = 1 << 5,
    ARG_Object        = 1 << 6,
    ARG_Raw_JSON      = 1 << 7,
    ARG_IsReference   = 1 << 8,
    ARG_StringIsConst = 1 << 9,
    ARG_Bool          = 1 << 10, // Extra detailed types
    ARG_Integer       = 1 << 11,

} ArgType;

typedef struct
{
    Str     name;
    char    shortopt;
    ArgType type; // Mirroring cJSON
    void*   val;  // Should directly point to struct to avoid copy

    Str  help;  // For printing help
    bool found; // Getopt does not enforce required, se we have to

} Arg;

typedef struct
{
    Arg*  buf;
    isize len;

    Str usage;

} Args;

void args_dump_help(Args* c);
void args_dump_values(Args* c);
int  args_parse(Buf* b, Args* args, int argc, char* argv[], bool strict);

// --------------- Implementation ---------------

void args_dump_help(Args* c)
{
    PrintLn("USAGE: %.*s", __(c->usage));

    isize maxname = 0;
    RANGE(i, c->len) { maxname = maxname < c->buf[i].name.len ? c->buf[i].name.len : maxname; }
    maxname = maxname > 30 ? 30 : maxname;

    buf_stack(temp, 128);
    Str fmt = str_fmt(&temp, "  --%%-%ld.*s    %%.*s\n", maxname);

    RANGE(i, c->len)
    {
        fprintf(stdout, fmt.buf,    //
                __(c->buf[i].name), //
                __(c->buf[i].help));
        maxname = maxname < c->buf[i].name.len ? c->buf[i].name.len : maxname;
    }
}

void args_dump_values(Args* c)
{
    RANGE(i, c->len)
    {
        if (c->buf[i].val)
        {
            Str   name  = c->buf[i].name;
            void* val   = c->buf[i].val;
            bool  found = c->buf[i].found;
            switch (c->buf[i].type)
            {
            case ARG_Invalid:
            case ARG_Array:
            case ARG_Object:
            case ARG_IsReference:
            case ARG_Raw_JSON:
            case ARG_StringIsConst:
            case ARG_True:  // These are invalid
            case ARG_False: // as type is bool
            case ARG_NULL: break;

            case ARG_Bool:
                bool b = *(bool*)val;
                PrintLn("%-10.*s [%d]= %s", __(name), found, b ? "true" : "false");
                break;

            case ARG_Float:
                float f = *(float*)val;
                PrintLn("%-10.*s [%d]= %f", __(name), found, f);
                break;

            case ARG_Integer:
                i32 x = *(i32*)val;
                PrintLn("%-10.*s [%d]= %d", __(name), found, x);
                break;

            case ARG_String:
                Str s = *(Str*)val;
                PrintLn("%-10.*s [%d]= %.*s", __(name), found, __(s));
                break;
            }
        }
    }
}

int args_parse(Buf* b, Args* args, int argc, char* argv[], bool strict)
{
    // Only place where arena is used, but keeps strings which we need
    struct option* long_options = Make(b, struct option, args->len + 1, ALLOC_ZERO);
    RANGE(i, args->len)
    {
        long_options[i] = (struct option){
            .name    = args->buf[i].name.buf,
            .has_arg = args->buf[i].type != ARG_Bool,
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
                args->buf[option_index].found = true;

                void* val = args->buf[option_index].val;
                switch (args->buf[option_index].type)
                {
                case ARG_Invalid:       // Unhandled cases
                case ARG_True:          //
                case ARG_False:         //
                case ARG_Bool:          // Only toggle, so no args
                case ARG_Array:         //
                case ARG_Object:        //
                case ARG_IsReference:   //
                case ARG_Raw_JSON:      //
                case ARG_StringIsConst: //
                case ARG_NULL: break;   //

                case ARG_Float: // TODO: error handling for atof
                    float* f = (float*)val;
                    *f       = atof(optarg);
                    break;

                case ARG_Integer: // TODO: error handling for atoi
                    i32* x = (i32*)val;
                    *x     = atoi(optarg);
                    break;

                case ARG_String:
                    Str* s = (Str*)val;
                    *s     = (Str){.buf = optarg, .len = strlen(optarg)};
                    break;
                }
            }
            else
            {
                void* val = args->buf[option_index].val;
                if (args->buf[option_index].type == ARG_Bool)
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
