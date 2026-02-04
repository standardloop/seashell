#ifndef STANDARDLOOP_SEASHELL_H
#define STANDARDLOOP_SEASHELL_H

#include <limits.h>

extern volatile sig_atomic_t GLOBAL_seashell_running;
extern char GLOBAL_pwd[PATH_MAX];
extern int GLOBAL_last_status;
extern void SeaShellSigHandler(int signum);

#endif
