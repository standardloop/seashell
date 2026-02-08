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

static inline bool checkEOFOrEOT(char);
static inline void cursorForward(int);
static inline void cursorBackward(int);
static inline void clearEntireLine();
static inline void printCMDBuffer(char *);
static inline void moveBackToBeginningOfLine();
static void resetEntireLine(char *, int, int);

static void insertAndShiftBuffer(char *, int, int, char c);
static void deleteAndShiftBuffer(char *, int, int);

static inline void printCMDBuffer(char *buffer)
{
    if (buffer != NULL)
    {
        printf("%s", buffer);
    }
}

static void insertAndShiftBuffer(char *buffer, int size, int index, char c)
{
    if (buffer == NULL || size <= 0 || index < 0 || index >= size)
    {
        return;
    }
    for (int i = size; i >= index; i--)
    {
        buffer[i + 1] = buffer[i];
    }
    buffer[index] = c;
}

static void deleteAndShiftBuffer(char *buffer, int size, int index)
{
    if (buffer == NULL || size <= 0 || index < 0 || index >= size)
    {
        return;
    }
    buffer[index] = NULL_CHAR;
    for (int i = index; i < size; i++)
    {
        buffer[i] = buffer[i + 1];
    }
    buffer[size - 1] = '\0'; // just for safety
}

static inline bool checkEOFOrEOT(char c)
{
    return (c == EOT_CHAR || c == EOF);
}

static inline void cursorBackward(int x)
{
    if (x != 0)
    {
        printf("\033[%dD", (x));
    }
}

static inline void cursorForward(int x)
{
    if (x != 0)
    {
        printf("\033[%dC", (x));
    }
}

static inline void clearEntireLine()
{
    printf("\033[2K");
}

static inline void moveBackToBeginningOfLine()
{
    printf("\r");
}

static void resetEntireLine(char *cmd_buffer, int cmd_cursor_index, int cmd_buffer_curr_length)
{
    clearEntireLine();
    moveBackToBeginningOfLine();
    DisplayPrompt(GLOBAL_last_status);
    printCMDBuffer(cmd_buffer);
    cursorBackward(cmd_buffer_curr_length);
    cursorForward(cmd_cursor_index);
}

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
                                if (cmd_cursor_index < cmd_buffer_curr_length)
                                {
                                    cmd_cursor_index++;
                                    cursorForward(1);
                                }
                            }
                            // Left arrow
                            else if (arrow_keys_buffer[1] == 'D')
                            {
                                // Log(TRACE, "left");
                                if (cmd_cursor_index > 0)
                                {
                                    cmd_cursor_index--;
                                    cursorBackward(1);
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
                        deleteAndShiftBuffer(cmd_buffer, cmd_buffer_curr_length, cmd_cursor_index - 1);
                        cmd_buffer_curr_length--;
                        cmd_cursor_index--;
                        resetEntireLine(cmd_buffer, cmd_cursor_index, cmd_buffer_curr_length);
                    }
                }
                else
                {
                    // left arrow or right arrow was used and we need to insert and shift
                    if (cmd_buffer[cmd_cursor_index] != NULL_CHAR)
                    {
                        insertAndShiftBuffer(cmd_buffer, cmd_buffer_curr_length, cmd_cursor_index, c);
                        cmd_buffer_curr_length++;
                        cmd_cursor_index++;
                        resetEntireLine(cmd_buffer, cmd_cursor_index, cmd_buffer_curr_length);
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
    return 0;
}
