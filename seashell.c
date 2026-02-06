#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

static void clearBuffer(char *, int);
static void clearBuffer(char *buffer, int size)
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

static void insertAndShiftBuffer(char *, int, int, char);
static void insertAndShiftBuffer(char *buffer, int size, int index, char c)
{
    if (buffer == NULL || size <= 0 || index < 0 || index >= size)
    {
        return;
    }
    for (int i = size - 1; i > index; i--)
    {
        // printf("buffer: %s\n", buffer);
        buffer[i] = buffer[i - 1];
    }
    buffer[index] = c;
    buffer[size - 1] = '\0';
}

static char **stringArrToExecArgs(StringArr *arr)
{
    if (arr == NULL || arr->num_strings == 0 || arr->strings == NULL)
    {
        return NULL;
    }
    char **return_value = malloc(sizeof(char *) * arr->num_strings + 1);
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

#define ESC_CHAR 27
#define COMMAND_BUFFER_SIZE 100
#define BACKSPACE_CHAR 127
int seaShellInteractive()
{
    char c;
    int i;
    Log(INFO, "Running Interactive");
    InitTerminal();

    // populate global pwd variable
    if (getcwd(GLOBAL_pwd, sizeof(GLOBAL_pwd)) == NULL)
    {
        printf("couldn't get initial cwd\n");
        return 1;
    }

    while (GLOBAL_seashell_running)
    {
        DisplayPrompt(GLOBAL_last_status);
        fflush(stdout);
        char command_buffer[COMMAND_BUFFER_SIZE] = {0};
        clearBuffer(command_buffer, COMMAND_BUFFER_SIZE);

        i = 0;
        while (ALWAYS)
        {
            c = getchar();

            if (c == EOF || c == NEWLINE_CHAR || c == NULL_CHAR || i == COMMAND_BUFFER_SIZE - 1)
            {
                break;
            }
            // handle arrow keys
            else if (c == ESC_CHAR)
            {
                char arrow_keys_buffer[2] = {NULL_CHAR, NULL_CHAR};
                arrow_keys_buffer[0] = getchar();
                arrow_keys_buffer[1] = getchar();

                if (arrow_keys_buffer[0] == BRACKET_OPEN_CHAR)
                {
                    // Up Arrow
                    if (arrow_keys_buffer[1] == 'A')
                    {
                    }
                    // Down Arrow
                    else if (arrow_keys_buffer[1] == 'B')
                    {
                    }
                    // Right arrow
                    else if (arrow_keys_buffer[1] == 'C')
                    {

                        if (command_buffer[i] != NULL_CHAR)
                        {
                            i++;
                            printf("\033[C");
                        }
                    }
                    // Left arrow
                    else if (arrow_keys_buffer[1] == 'D')
                    {

                        if (i > 0)
                        {
                            i--;
                            printf("\b");
                        }
                    }
                    else
                    {
                        // pass;
                    }
                }
            }
            else if (c == BACKSPACE_CHAR)
            {
                i--;
                // printf("i = %d\n", i);
                // TODO add assert here
                if (i < 0)
                {
                    i = 0;
                    clearBuffer(command_buffer, COMMAND_BUFFER_SIZE);
                }
                else
                {
                    command_buffer[i] = NULL_CHAR;
                    printf("\b \b");
                }

                // Log(TRACE, "backspace char");
            }
            else
            {
                if (command_buffer[i] != NULL_CHAR)
                {
                    insertAndShiftBuffer(command_buffer, COMMAND_BUFFER_SIZE, i, c);
                }
                else
                {
                    command_buffer[i] = c;
                }
                printf("%c", c);
                i++;
            }
        }
        command_buffer[i] = NULL_CHAR;
        if (GLOBAL_seashell_running)
        {
            printf("\n");
            if (command_buffer[0] != NULL_CHAR)
            {
                printf("\r");
                StringArr *buffer_seperated_by_spaces = EveryoneExplodeNow(command_buffer, SPACE_CHAR);
                PrintStringArr(buffer_seperated_by_spaces);
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
                clearBuffer(command_buffer, COMMAND_BUFFER_SIZE);
                FreeStringArr(buffer_seperated_by_spaces);
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
