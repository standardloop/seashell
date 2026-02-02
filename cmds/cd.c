#include <unistd.h>
#include "./cmds.h"

extern int SeashellCD(StringArr *args)
{
    if (args == NULL)
    {
        return -1;
    }
    else if (args->num_strings == 1)
    {
        printf("cd: not enough arguments\n");
        return -1;
    }
    else if (args->num_strings != 2)
    {
        printf("cd: too many arguments\n");
        return -1;
    }
    // TODO
    return 0;
}
