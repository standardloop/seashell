#include "./cmds.h"

extern int SeashellExit(StringArr *args)
{
    // use assert here
    if (args == NULL || args->num_strings == 0)
    {
        return -1;
    }
    if (args->num_strings > 2)
    {
        printf("exit: too many arguments\n");
        return -1;
    }
    int exit_status = 0;
    // have status code in args
    if (args->num_strings > 1)
    {
        exit_status = strtol(args->strings[1], NULL, 10);
    }
    GLOBAL_seashell_running = false;
    return exit_status;
}