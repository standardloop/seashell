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

static bool checkEOFOrEOT(char);
static void insertAndShiftBuffer(char *, int, int, char c);
static void deleteAndShiftBuffer(char *, int, int);


static void insertAndShiftBuffer(char *buffer, int max_size, int index, char c)
{
    if (buffer == NULL || max_size <= 0 || index < 0 || index >= max_size)
    {
        return;
    }
    for (int i = max_size - 1; i > index; i--)
    {
        // printf("buffer: %s\n", buffer);
        buffer[i] = buffer[i - 1];
    }
    buffer[index] = c;
    buffer[max_size - 1] = '\0'; // just for safety
}

static void deleteAndShiftBuffer(char *buffer, int max_size, int index)
{
    if (buffer == NULL || max_size <= 0 || index < 0 || index >= max_size)
    {
        return;
    }
    buffer[index] = NULL_CHAR;
    for (int i = index; i > max_size; i++)
    {
        // printf("buffer: %s\n", buffer);
        buffer[i] = buffer[i + 1];
    }
    buffer[max_size - 1] = '\0'; // just for safety
}

static bool checkEOFOrEOT(char c)
{
    return (c == EOT_CHAR || c == EOF);
}

#define MACRO_cursorForward(x) printf("\033[%dC", (x))
#define MACRO_cursorBackward(x) printf("\033[%dD", (x))
extern int GetSeashellLine(char *cmd_buffer)
{
    int cmd_cursor_index = 0;
    char c = NULL_CHAR;
    int cmd_buffer_curr_length = 0;

    struct pollfd stdin_poll_in;
    stdin_poll_in.fd = STDIN_FILENO; // pull stdin
    stdin_poll_in.events = POLLIN;   // let us know when there is data
    int poll_time = -1;              // poll forever (unless interuppted)

    while (ALWAYS)
    {
        int ret = poll(&stdin_poll_in, 1, poll_time);
        if (ret == -1)
        {
            if (errno == EINTR)
            {
                // The signal interrupted poll(3)
                // We need to clear the buffer and reset the flag that was set in signals/signals.c
                if (GLOBAL_signal_clear_buffer)
                {
                    ClearBuffer(cmd_buffer, COMMAND_BUFFER_SIZE);
                    cmd_cursor_index = 0;
                    cmd_buffer_curr_length = 0;
                    GLOBAL_signal_clear_buffer = false;
                    printf("\n");
                    DisplayPrompt(GLOBAL_last_status);
                    fflush(stdout);
                }
                continue;
            }
        }

        if (stdin_poll_in.revents & POLLIN)
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
                else if (c == NEWLINE_CHAR || c == NULL_CHAR || cmd_cursor_index == COMMAND_BUFFER_SIZE - 1)
                {
                    break;
                }
                // Handle arrow keys
                else if (c == ESC_CHAR)
                {
                    char arrow_keys_buffer[3] = {0}; // TODO, how large should this be?
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
                                if (cmd_buffer[cmd_cursor_index] != NULL_CHAR)
                                {
                                    cmd_cursor_index++;
                                    MACRO_cursorForward(1);
                                }
                            }
                            // Left arrow
                            else if (arrow_keys_buffer[1] == 'D')
                            {
                                // Log(TRACE, "left");
                                if (cmd_cursor_index > 0)
                                {
                                    cmd_cursor_index--;
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
                    if (cmd_cursor_index > 0)
                    {

                        cmd_cursor_index--;
                        cmd_buffer_curr_length--;
                        printf("\b \b");
                        deleteAndShiftBuffer(cmd_buffer, COMMAND_BUFFER_SIZE, cmd_cursor_index);
                        printf("\r");
                        DisplayPrompt(GLOBAL_last_status);
                        printf("%s", cmd_buffer);
                        // printf("\n%d\n", cmd_buffer_curr_length);
                        int curr_buff_size_temp = (int)strlen(cmd_buffer); // can we track this without strlen? cmd_buffer_curr_length;
                        // int curr_buff_size_temp = cmd_buffer_curr_length; // can we track this without strlen?
                        MACRO_cursorBackward(curr_buff_size_temp);
                        MACRO_cursorForward(cmd_cursor_index);
                    }
                    // Log(TRACE, "backspace char");
                }
                else
                {
                    // left arrow or right arrow was used and we need to insert and shift
                    if (cmd_buffer[cmd_cursor_index] != NULL_CHAR)
                    {
                        // Wrap this in a function
                        // printf("\n%d\n", cmd_buffer_curr_length);
                        insertAndShiftBuffer(cmd_buffer, COMMAND_BUFFER_SIZE, cmd_cursor_index, c);
                        cmd_buffer_curr_length++;
                        cmd_cursor_index++;
                        printf("\r");
                        DisplayPrompt(GLOBAL_last_status);
                        printf("%s", cmd_buffer);
                        // printf("\n%d\n", cmd_buffer_curr_length);
                        int curr_buff_size_temp = (int)strlen(cmd_buffer); // can we track this without strlen? cmd_buffer_curr_length;
                        // int curr_buff_size_temp = cmd_buffer_curr_length; // can we track this without strlen?
                        MACRO_cursorBackward(curr_buff_size_temp);
                        MACRO_cursorForward(cmd_cursor_index);
                    }
                    // Normal operation
                    else
                    {
                        cmd_buffer[cmd_cursor_index] = c;
                        putchar(c); // Manual echo since we disabled it in terminal/terminal.c
                        cmd_buffer_curr_length++;
                        cmd_cursor_index++;
                    }
                }
                fflush(stdout);
            }
        }
    }
    (void)cmd_buffer_curr_length;
    // cmd_buffer[cmd_cursor_index] = NULL_CHAR;
    return 0;
}
