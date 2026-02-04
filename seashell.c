#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown

#include "./seashell.h"
#include "./cmds/cmds.h"
#include "./runner/runner.h"
#include "./terminal/terminal.h"

#include <standardloop/logger.h>
#include <standardloop/util.h>

#define CHILD_PID 0

int seaShellInteractive();
void seaShellNoInteractive();

static inline void displayPrompt(int);

static inline bool isInteractive();
static inline bool isInteractive()
{
    return isatty(STDIN_FILENO) == 1;
}

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ANSI_STYLE_BOLD "\e[1m"
#define ANSI_STYLE_ITALICS "\e[3m"

static inline void displayPrompt(int last_status)
{
    // TODO
    // read settings from config file
    // example, color, showing directory, etc...
    char cwd[1024];
    // TODO NULL check
    (void)getcwd(cwd, sizeof(cwd));

    if (last_status == 0)
    {
        printf(ANSI_COLOR_GREEN "➜ " ANSI_COLOR_YELLOW "(%s) " ANSI_COLOR_GREEN "seashell 🐚" ANSI_COLOR_YELLOW ": " ANSI_COLOR_RESET, cwd);
    }
    else
    {
        printf(ANSI_COLOR_RED "➜ " ANSI_COLOR_YELLOW "(%s) " ANSI_COLOR_GREEN "seashell 🐚" ANSI_COLOR_YELLOW ": " ANSI_COLOR_RESET, cwd);
    }
}

static void clearBuffer(char *, int);
static void clearBuffer(char *buffer, int size)
{
    if (buffer == NULL || size < 0)
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
        displayPrompt(GLOBAL_last_status);
        fflush(stdout);
        char command_buffer[COMMAND_BUFFER_SIZE];
        command_buffer[0] = NULL_CHAR;
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
                // TODO
                if (arrow_keys_buffer[0] == BRACKET_OPEN_CHAR)
                {
                    if (arrow_keys_buffer[1] == 'A')
                    {
                        // printf("Up arrow\n");
                    }
                    else if (arrow_keys_buffer[1] == 'B')
                    {
                        // printf("Down arrow\n");
                    }
                    else if (arrow_keys_buffer[1] == 'C')
                    {
                        // printf("Right arrow\n");
                    }
                    else if (arrow_keys_buffer[1] == 'D')
                    {
                        // printf("Left arrow\n");
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
                command_buffer[i] = (char)c;
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
                // PrintStringArr(buffer_seperated_by_spaces);
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

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SeaShellSigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        Log(FATAL, "sigaction SIGINT fail");
        return EXIT_FAILURE;
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        Log(FATAL, "sigaction SIGTERM fail");
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
