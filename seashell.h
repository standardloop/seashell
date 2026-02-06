#ifndef STANDARDLOOP_SEASHELL_H
#define STANDARDLOOP_SEASHELL_H

#include <limits.h>

extern volatile sig_atomic_t GLOBAL_seashell_running;
extern volatile sig_atomic_t GLOBAL_last_status;
extern volatile sig_atomic_t GLOBAL_signal_clear_buffer;
extern char GLOBAL_pwd[PATH_MAX];

#endif
