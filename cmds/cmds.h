#ifndef STANDARDLOOP_SEASHELL_CMDS_H
#define STANDARDLOOP_SEASHELL_CMDS_H

#include <standardloop/util.h>

#define SEASHELL_VERSION "v0.0.0"

extern volatile sig_atomic_t GLOBAL_seashell_running;

typedef int(SeashellFunction)(StringArr *);

extern SeashellFunction *FunctionStringToFunction(char *);

extern int SeashellHelp(StringArr *args);

extern int SeashellExit(StringArr *args);

#endif
