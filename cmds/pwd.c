#include <unistd.h>
#include "./cmds.h"

extern int SeashellPWD(StringArr *args)
{
    if (args == NULL || args->num_strings == 0 || args->num_strings > 1)
    {
        return -1;
    }
    printf("%s\n", GLOBAL_pwd);
    return 0;
}
