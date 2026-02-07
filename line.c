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

extern int GetSeashellLine(char *command_buffer)
{
    int i = 0;
    char c;
    while (ALWAYS)
    {

        c = getchar();
        if (GLOBAL_signal_clear_buffer)
        {
            GLOBAL_signal_clear_buffer = false;
            ClearBuffer(command_buffer, COMMAND_BUFFER_SIZE);
            GLOBAL_last_status = 0;
            return 1;
        }

        // n = read(STDIN_FILENO, &c, 1);
        // if (n)
        //     ;
        // write(STDOUT_FILENO, &c, 1);

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
        else if (c == NEWLINE_CHAR || c == NULL_CHAR || i == COMMAND_BUFFER_SIZE - 1)
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
                ClearBuffer(command_buffer, COMMAND_BUFFER_SIZE);
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
    return 0;
}
