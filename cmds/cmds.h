#ifndef STANDARDLOOP_SEASHELL_CMDS_H
#define STANDARDLOOP_SEASHELL_CMDS_H

#include <standardloop/util.h>
#include <limits.h>

#define SEASHELL_VERSION "v0.0.0"

extern volatile sig_atomic_t GLOBAL_seashell_running;
extern char GLOBAL_pwd[PATH_MAX];

typedef int(SeashellFunction)(StringArr *);

extern SeashellFunction *FunctionStringToFunction(char *);

extern int SeashellHelp(StringArr *args);

extern int SeashellExit(StringArr *args);

extern int SeashellPWD(StringArr *args);

extern int SeashellCD(StringArr *args);

extern int SeashellLS(StringArr *args);

#endif
