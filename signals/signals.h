#ifndef STANDARDLOOP_SEASHELL_SIGNALS_H
#define STANDARDLOOP_SEASHELL_SIGNALS_H

extern void SeaShellSigHandler(int);

// from global
extern volatile sig_atomic_t GLOBAL_seashell_running;
extern int GLOBAL_last_status;

#endif
