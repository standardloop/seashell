#ifndef STANDARDLOOP_SEASHELL_H
#define STANDARDLOOP_SEASHELL_H

#include <limits.h>

#define ESC_CHAR 27
#define COMMAND_BUFFER_SIZE 100
#define BACKSPACE_CHAR 127

extern volatile sig_atomic_t GLOBAL_seashell_running;
extern volatile sig_atomic_t GLOBAL_last_status;
extern volatile sig_atomic_t GLOBAL_signal_clear_buffer;
extern char GLOBAL_pwd[PATH_MAX];

extern int GetSeashellLine(char *);
extern void ClearBuffer(char *, int);

#endif
