#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./prompt.h"

extern void DisplayPrompt(int last_status)
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
