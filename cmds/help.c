#include "./cmds.h"

extern int SeashellHelp(StringArr *args)
{
    if (args == NULL || args->num_strings == 0)
    {
        return -1;
    }
    printf("seashell version: %s\n", SEASHELL_VERSION);
    return 0;
}
