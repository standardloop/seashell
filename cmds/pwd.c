#include <unistd.h>
#include "./cmds.h"

extern int SeashellPWD(StringArr *args)
{
    if (args == NULL || args->num_strings == 0 || args->num_strings > 1)
    {
        return -1;
    }
    char cwd[1024];
    // TODO NULL check
    (void)getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    return 0;
}
