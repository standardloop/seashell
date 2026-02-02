#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown
#include <errno.h>

#include <termios.h>

#include "./seashell.h"

#include <standardloop/logger.h>
#include <standardloop/util.h>

#define CHILD_PID 0

volatile sig_atomic_t seashell_running = true;

void seaShellSigHandler(int);
void seaShellInteractive();
void seaShellNoInteractive();
static inline void displayPrompt();

static struct termios oldtio, newtio;

typedef void(seashellFunction)(void);

void seashellHelp();
void seashellHelp()
{
    printf("seashell version: %s\n", SEASHELL_VERSION);
}

void seashellExit();
void seashellExit()
{
    seashell_running = false;
}

seashellFunction *functionStringToFunction(char *function_name)
{
    const int seashellFunctionListLength = 4;
    char *seashellFunctionList[] = {
        "exit",
        "cd",
        "env",
        "help",
    };
    for (int i = 0; i < seashellFunctionListLength; i++)
    {
        if (strcmp(seashellFunctionList[i], function_name) == 0)
        {
            switch (i)
            {
            case 0:
                return seashellExit;
            case 1:
                return NULL;
            case 2:
                return NULL;
            case 3:
                return seashellHelp;
            }
        }
    }
    return NULL;
}

/* Initialize terminal to non-canonical and no-echo mode */
void initTerminal();
void initTerminal()
{
    tcgetattr(STDIN_FILENO, &oldtio);          // Save current terminal settings
    newtio = oldtio;                           // Make a copy
    newtio.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    newtio.c_cc[VMIN] = 1;                     // Read at least 1 character
    newtio.c_cc[VTIME] = 0;                    // No timeout (wait indefinitely)
    tcsetattr(STDIN_FILENO, TCSANOW, &newtio); // Apply new settings immediately
}

/* Restore terminal to original settings */
void restoreTerminal();
void restoreTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtio);
}

// NOTE: SIGKILL cannot be trapped
void seaShellSigHandler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        Log(ERROR, "SIGINT received!");
        // this only cancel the process that seashell is running
        break;
    case SIGTERM:
        Log(ERROR, "SIGTERM received!");
        break;
    default:
        break;
    }
    restoreTerminal();
    seashell_running = false;
}

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ANSI_STYLE_BOLD "\e[1m"
#define ANSI_STYLE_ITALICS "\e[3m"

static inline void displayPrompt()
{
    printf(ANSI_COLOR_GREEN "➜ seashell 🐚" ANSI_COLOR_YELLOW ": " ANSI_COLOR_RESET);
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

static int runCommand(char *buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
    return 0;
    // analyis buffer
    // fork
    // wait for child process to finish
    // return exit code of child process
}

#define ESC_CHAR 27
#define LINE_BUFFER_SIZE 100
#define BACKSPACE_CHAR 127
void seaShellInteractive()
{
    char c;
    int i;
    Log(INFO, "Running Interactive");
    initTerminal();

    while (seashell_running)
    {
        displayPrompt();
        fflush(stdout);
        char buffer[LINE_BUFFER_SIZE];
        buffer[0] = NULL_CHAR;
        i = 0;
        while (ALWAYS)
        {
            c = getchar();

            if (c == EOF || c == NEWLINE_CHAR || c == NULL_CHAR || i == LINE_BUFFER_SIZE - 1)
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
                    clearBuffer(buffer, LINE_BUFFER_SIZE);
                }
                else
                {
                    buffer[i] = NULL_CHAR;
                    printf("\b \b");
                }

                // Log(TRACE, "backspace char");
            }
            else
            {
                buffer[i] = (char)c;
                printf("%c", c);
                i++;
            }
        }
        buffer[i] = NULL_CHAR;
        printf("\n\r");
        seashellFunction *built_in = functionStringToFunction(buffer);
        if (built_in != NULL)
        {
            built_in();
        }
        else
        {
            printf("\nBuffer = %s\n", buffer);
            runCommand(buffer);
        }
        clearBuffer(buffer, LINE_BUFFER_SIZE);
    }
    restoreTerminal();
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
    sa.sa_handler = seaShellSigHandler;
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

    if (isatty(STDIN_FILENO) == 1)
    {
        seaShellInteractive();
    }
    else
    {
        // TODO
        seaShellNoInteractive();
    }

    return EXIT_SUCCESS;
}
