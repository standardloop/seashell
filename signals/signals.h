#ifndef STANDARDLOOP_SEASHELL_SIGNALS_H
#define STANDARDLOOP_SEASHELL_SIGNALS_H

extern int SignalsInit();

// from global
extern volatile sig_atomic_t GLOBAL_seashell_running;
extern volatile sig_atomic_t GLOBAL_last_status;
extern volatile sig_atomic_t GLOBAL_signal_clear_buffer;

#endif
