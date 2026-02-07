#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

#include "./seashell.h"
#include "./cmds/cmds.h"
#include "./prompt/prompt.h"
#include "./runner/runner.h"
#include "./signals/signals.h"
#include "./terminal/terminal.h"

#include <standardloop/logger.h>
#include <standardloop/util.h>

static void insertAndShiftBuffer(char *, int, int, char);
static bool checkEOFOrEOT(char);

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

static bool checkEOFOrEOT(char c)
{
    return (c == EOT_CHAR || c == EOF);
}

#define MACRO_cursorForward(x) printf("\033[%dC", (x))
#define MACRO_cursorBackward(x) printf("\033[%dD", (x))
extern int GetSeashellLine(char *cmd_buffer)
{
    int cmd_index = 0;
    char c = NULL_CHAR;
    int cmd_buffer_curr_length = 0;

    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    while (ALWAYS)
    {
        int ret = poll(fds, 1, -1);
        if (ret == -1)
        {
            if (errno == EINTR)
            {
                // The signal interrupted poll()
                if (GLOBAL_signal_clear_buffer)
                {
                    ClearBuffer(cmd_buffer, COMMAND_BUFFER_SIZE);
                    cmd_index = 0;
                    cmd_buffer_curr_length = 0;
                    GLOBAL_signal_clear_buffer = false;
                    printf("\n");
                    DisplayPrompt(GLOBAL_last_status);
                    fflush(stdout);
                }
                continue; // Restart the loop
            }
        }

        if (fds[0].revents & POLLIN)
        {
            if (read(STDIN_FILENO, &c, 1) > 0)
            {
                if (checkEOFOrEOT(c))
                {
                    GLOBAL_seashell_running = false;
                    // TODO: % is showing when running
                    break;
                }
                // TODO: autocomplete
                else if (c == TAB_CHAR)
                {
                    continue;
                }
                // NULL_CHAR 怎麼辦？
                else if (c == NEWLINE_CHAR || c == NULL_CHAR || cmd_index == COMMAND_BUFFER_SIZE - 1)
                {
                    break;
                }
                // handle arrow keys
                else if (c == ESC_CHAR)
                {
                    char arrow_keys_buffer[3] = {0};
                    if (read(STDIN_FILENO, &arrow_keys_buffer[0], 1) > 0 && read(STDIN_FILENO, &arrow_keys_buffer[1], 1) > 0)
                    {
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
                                // Log(TRACE, "Right");
                                if (cmd_buffer[cmd_index] != NULL_CHAR)
                                {
                                    cmd_index++;
                                    MACRO_cursorForward(1);
                                }
                            }
                            // Left arrow
                            else if (arrow_keys_buffer[1] == 'D')
                            {
                                // Log(TRACE, "left");
                                if (cmd_index > 0)
                                {
                                    cmd_index--;
                                    MACRO_cursorBackward(1);
                                }
                            }
                            else
                            {
                                continue;
                            }
                        }
                    }
                }
                else if (c == BACKSPACE_CHAR)
                {
                    if (cmd_index > 0)
                    {
                        cmd_index--;
                        cmd_buffer_curr_length--;
                        cmd_buffer[cmd_index] = NULL_CHAR;
                        printf("\b \b");
                        fflush(stdout);
                    }
                    // Log(TRACE, "backspace char");
                }
                else
                {
                    if (cmd_buffer[cmd_index] != NULL_CHAR)
                    {
                        // TODO
                        insertAndShiftBuffer(cmd_buffer, COMMAND_BUFFER_SIZE, cmd_index, c);
                    }
                    else
                    {
                        cmd_buffer[cmd_index] = c;
                    }
                    cmd_index++;
                    putchar(c); // Manual echo since we disabled it
                }
                fflush(stdout);
            }
        }
    }
    (void)cmd_buffer_curr_length;
    cmd_buffer[cmd_index] = NULL_CHAR;
    return 0;
}
