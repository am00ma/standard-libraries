#pragma once

#include "stz-c/stz-c.h" // IWYU pragma: keep

#include <errno.h>    // IWYU pragma: keep
#include <sys/wait.h> // waitpid, pid
#include <unistd.h>   // pipe, dup2, fork, close

// --------------- Definitions ---------------

#define CmdShellBash _("/bin/bash")

typedef struct
{
    Str out;
    Str err;
    i32 status;

} CmdShell;

DECLARE_RESULT(CmdShell);

Res(CmdShell) cmd_exec( //
    Buf*  a,
    Str   command,
    Str   shell,
    isize out_maxlen,
    isize err_maxlen);

// --------------- Implementations ---------------

inline Res(CmdShell) cmd_exec( //
    Buf*  a,
    Str   command,
    Str   shell,
    isize out_maxlen,
    isize err_maxlen)
{
    int      err = 0;
    CmdShell res = {};

    int   stdout_pipe[2], stderr_pipe[2];
    pid_t pid;

    // Create pipes for stdout and stderr
    err = (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1);
    if (err) return (Res(CmdShell)){err, {}};

    // Fork the process
    pid = fork();
    err = pid < 0;
    if (err) return (Res(CmdShell)){err, {}};

    if (pid == 0)
    { // Child process
        // Redirect stdout to the write end of the stdout pipe
        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        // Redirect stderr to the write end of the stderr pipe
        dup2(stderr_pipe[1], STDERR_FILENO);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);

        // Execute the command
        err = execl(shell.buf, shell.buf, "-c", command.buf, (char*)NULL);
        if (err) { exit(EXIT_FAILURE); }
    }
    else
    { // Parent process
        // Close the write ends of the pipes
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Keep track of capacity for out of memory error
        // Fortunately, capacity is signed (ptrdiff_t)
        isize cap, num_bytes;

        // TODO: Not reading out and error simultaneously
        Buf bout  = buf_new2(a, out_maxlen, ALLOC_NOZERO);
        cap       = bout.cap;
        num_bytes = 0;
        do
        {
            if (cap <= 0) return (Res(CmdShell)){ENOMEM, {}};
            num_bytes  = read(stdout_pipe[0], bout.buf + bout.len, cap);
            bout.len  += num_bytes;
            cap       -= num_bytes;
        } while (num_bytes != 0);
        buf_shrink(a, &bout);
        res.out = Str_(bout);
        close(stdout_pipe[0]);

        // TODO: Not reading out and error simultaneously
        Buf berr  = buf_new2(a, err_maxlen, ALLOC_NOZERO);
        cap       = berr.cap;
        num_bytes = 0;
        do
        {
            if (cap <= 0) return (Res(CmdShell)){ENOMEM, {}};
            num_bytes  = read(stderr_pipe[0], berr.buf + berr.len, cap);
            berr.len  += num_bytes;
            cap       -= num_bytes;
        } while (num_bytes != 0);
        buf_shrink(a, &berr);
        res.err = Str_(berr);
        close(stderr_pipe[0]);

        // Wait for the child process to finish and get the exit status
        waitpid(pid, (int*)&res.status, 0);
        if (WIFEXITED(res.status)) { res.status = WEXITSTATUS(res.status); }
    }

    return (Res(CmdShell)){0, res};
}
