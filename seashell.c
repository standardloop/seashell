#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "./seashell.h"
#include "./cmds/cmds.h"
#include "./prompt/prompt.h"
#include "./runner/runner.h"
#include "./signals/signals.h"
#include "./terminal/terminal.h"

#include <standardloop/logger.h>
#include <standardloop/util.h>

#define CHILD_PID 0

int seaShellInteractive();
void seaShellNoInteractive();

static inline bool isInteractive();
static inline bool isInteractive()
{
    return isatty(STDIN_FILENO) == 1;
}

extern void ClearBuffer(char *buffer, int size)
{
    if (buffer == NULL || size <= 0)
    {
        return;
    }

    for (int reset_index = 0; reset_index < size - 1; reset_index++)
    {
        buffer[reset_index] = NULL_CHAR;
    }
}

static char **stringArrToExecArgs(StringArr *arr)
{
    if (arr == NULL || arr->num_strings == 0 || arr->strings == NULL)
    {
        return NULL;
    }
    char **return_value = malloc(sizeof(char *) * (arr->num_strings + 1)); // +1 for NULL at end
    if (return_value == NULL)
    {
        return NULL;
    }
    int i;
    for (i = 0; i < arr->num_strings; i++)
    {
        return_value[i] = arr->strings[i];
    }
    return_value[i] = NULL;
    return return_value;
}

int seaShellInteractive()
{
    ssize_t n = 0;
    if (n)
    {
        ;
    }
    Log(INFO, "Running Interactive");
    InitTerminal();

    // populate global pwd variable
    if (getcwd(GLOBAL_pwd, sizeof(GLOBAL_pwd)) == NULL)
    {
        Log(ERROR, "couldn't populate GLOBAL_pwd");
        return 1;
    }

    while (GLOBAL_seashell_running)
    {
        DisplayPrompt(GLOBAL_last_status);
        fflush(stdout);
        char command_buffer[COMMAND_BUFFER_SIZE] = {0};

        int err = GetSeashellLine(command_buffer);
        if (err != 0)
            ;

        if (GLOBAL_seashell_running)
        {
            printf("\n");
            if (GLOBAL_signal_clear_buffer)
            {
                // GLOBAL_signal_clear_buffer = false;
                // ClearBuffer(command_buffer, COMMAND_BUFFER_SIZE);
                // GLOBAL_last_status = 0;
            }
            else
            {
                printf("\r");
                if (command_buffer[0] != NULL_CHAR)
                {
                    StringArr *buffer_seperated_by_spaces = EveryoneExplodeNow(command_buffer, SPACE_CHAR);
                    PrintStringArr(buffer_seperated_by_spaces);

                    // see if command matches built in
                    SeashellFunction *built_in = FunctionStringToFunction(buffer_seperated_by_spaces->strings[0]);
                    if (built_in != NULL)
                    {
                        GLOBAL_last_status = built_in(buffer_seperated_by_spaces);
                    }
                    else if (buffer_seperated_by_spaces->num_strings >= 1 && buffer_seperated_by_spaces->strings[0] != NULL && buffer_seperated_by_spaces->strings[0][0] != NULL_CHAR)
                    {
                        // execvp requires char** instead of our custom StringArr for our custom <standardloop/util.h>
                        char **exec_args = stringArrToExecArgs(buffer_seperated_by_spaces);

                        // TODO: need to check if a command exists before trying to run it.
                        GLOBAL_last_status = RunCommand(exec_args);
                        free(exec_args);
                    }
                    ClearBuffer(command_buffer, COMMAND_BUFFER_SIZE);
                    FreeStringArr(buffer_seperated_by_spaces);
                }
                else
                {
                    GLOBAL_last_status = 0;
                }
            }
        }
    }
    RestoreTerminal();
    return GLOBAL_last_status;
}

void seaShellNoInteractive()
{
    Log(INFO, "Running Non Interactive");
}

int main(int argc, char **argv)
{
    InitLogger(TRACE, STANDARD_FMT, false, true, true, true);
    Log(TRACE, "歡迎光臨");

    for (int i = 0; i < argc; i++)
    {
        Log(INFO, "argv[%d]: %s", i, argv[i]);
    }

    if (SignalsInit() != 0)
    {
        return EXIT_FAILURE;
    }

    if (isInteractive())
    {
        seaShellInteractive();
        return GLOBAL_last_status;
    }
    else
    {
        // TODO
        seaShellNoInteractive();
    }

    return EXIT_SUCCESS;
}
