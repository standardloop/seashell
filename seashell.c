#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown
#include <errno.h>

#include <standardloop/logger.h>
#include <standardloop/util.h>

#define CHILD_PID 0

volatile sig_atomic_t seashell_running = true;

void seaShellSigHandler(int);
void seaShellInteractive();
void seaShellNoInteractive();

// NOTE: SIGKILL cannot be trapped
void seaShellSigHandler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        // Log(ERROR, "SIGINT received!");
        // this only cancel the process that seashell is running
        break;
    case SIGTERM:
        Log(ERROR, "SIGTERM received!");
        break;
    default:
        break;
    }
    seashell_running = false;
}

static inline void displayPrompt();

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"


static inline void displayPrompt()
{
    printf(ANSI_COLOR_GREEN "➜ seashell 🐚" ANSI_COLOR_YELLOW ": " ANSI_COLOR_RESET);
    // printf("seashell-v0.0.0 -> ");
}

char *readLine();

char *readLine()
{
    // getline will allocation memory for us
    char *line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1)
    {
        if (feof(stdin))
        {
            free(line);
            exit(EXIT_SUCCESS);
        }
        else
        {
            free(line); /* avoid memory leaks when getline fails */
            perror("error while reading the line from stdin");
            exit(EXIT_FAILURE);
        }
    }
    return (line);
}

void seaShellInteractive()
{
    Log(INFO, "Running Interactive");
    while (seashell_running)
    {
        displayPrompt();
        char *line = readLine();
        Log(TRACE, "line: %s", line);
        // parse input
        // spawn child process
        // wait for child process to finish
        // free line
        // do everything again
    }
}

void seaShellNoInteractive()
{
    Log(INFO, "Running Non Interactive");
}

int main(void)
{
    InitLogger(TRACE, STANDARD_FMT, false, true, true, true);
    Log(TRACE, "歡迎光臨");

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
