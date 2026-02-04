#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> // signal polling for graceful shutdown
#include <stdbool.h>
#include <limits.h>

volatile sig_atomic_t GLOBAL_seashell_running = true;
int GLOBAL_last_status = 0;
char GLOBAL_pwd[PATH_MAX];
