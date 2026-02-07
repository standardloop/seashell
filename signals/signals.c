#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown
#include <stdbool.h>
#include <errno.h>

#include <standardloop/logger.h>

#include "./signals.h"
#include "../prompt/prompt.h"

static void seaShellSigHandler(int);

extern int SignalsInit()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = seaShellSigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        Log(ERROR, "sigaction SIGCHLD fail");
        return 1;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        Log(ERROR, "sigaction SIGINT fail");
        return 1;
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        Log(ERROR, "sigaction SIGTERM fail");
        return 1;
    }
    return 0;
}

// NOTE: SIGKILL cannot be trapped
// printf is not safe in signal handlers
// FIXME
static void seaShellSigHandler(int signum)
{
    // bool is_child = false;
    switch (signum)
    {
    case SIGALRM:
        Log(WARN, "SIGALRM received!");
        break;
    case SIGHUP:
        Log(WARN, "SIGHUP received!");
        break;
    case SIGPIPE:
        Log(WARN, "SIGPIPE received!");
        break;
    case SIGCHLD:
        Log(WARN, "SIGCHLD received!");
        break;
    case SIGINT:
        // SIGINT should clear the command buffer and then make the status none 0
        // Log(WARN, "SIGINT received!");
        GLOBAL_last_status = 1;
        GLOBAL_signal_clear_buffer = true;
        // FIXME we don't want this here
        printf("errno == %d\n", errno);
        printf("\n");
        DisplayPrompt(GLOBAL_last_status);
        break;
    case SIGTERM:
        Log(ERROR, "SIGTERM received!");
        GLOBAL_last_status = 1;
        GLOBAL_seashell_running = false;
        break;
    default:
        break;
    }
}
