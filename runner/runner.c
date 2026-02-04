#include "./runner.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <standardloop/logger.h>

#define CHILD_PID 0

extern int RunCommand(char **args)
{
    if (args == NULL)
    {
        return -1;
    }
    int status;
    pid_t pid = fork();
    if (pid < 0)
    {
        // Error handling for fork failure
        printf("fork failed\n");
        return -1;
    }
    else if (pid == CHILD_PID)
    {
        signal(SIGINT, SIG_IGN);
        execvp(args[0], args);
        Log(ERROR, "%s\n", strerror(errno));
        exit(errno);
    }
    else
    {
        waitpid(pid, &status, WUNTRACED);

        if (WIFEXITED(status))
        {
            Log(DEBUG, "Child exited normally with status = %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            Log(ERROR, "Child terminated by signal %d\n", WTERMSIG(status));
        }
        else
        {
            Log(ERROR, "Child terminated abnormally for another reason\n");
        }
        return status;
    }
}
