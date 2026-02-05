#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown
#include <stdbool.h>

#include <standardloop/logger.h>

#include "./signals.h"


// NOTE: SIGKILL cannot be trapped
extern void SeaShellSigHandler(int signum)
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
        Log(WARN, "SIGINT received!");
        GLOBAL_last_status = 1;
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
