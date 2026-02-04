#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown
#include <termios.h>
#include <dirent.h>

static struct termios oldtio, newtio;

/* Initialize terminal to non-canonical and no-echo mode */
extern void InitTerminal()
{
    tcgetattr(STDIN_FILENO, &oldtio);          // Save current terminal settings
    newtio = oldtio;                           // Make a copy
    newtio.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    newtio.c_cc[VMIN] = 1;                     // Read at least 1 character
    newtio.c_cc[VTIME] = 0;                    // No timeout (wait indefinitely)
    tcsetattr(STDIN_FILENO, TCSANOW, &newtio); // Apply new settings immediately
}

extern void RestoreTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtio);
}
